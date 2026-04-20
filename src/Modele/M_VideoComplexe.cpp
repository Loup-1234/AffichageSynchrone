#include "../../include/Modele/M_VideoComplexe.h"

// Bibliothèques tierces
#include <fftw3.h>

// Bibliothèques standard C++
#include <algorithm>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <format>
#include <fstream>
#include <future>
#include <iomanip>
#include <iostream>
#include <memory>
#include <mutex>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

mutex fftwMutex;

void M_VideoComplexe::genererVideoComplexe(const vector<string> &listeFichierEntree, const string &fichierSortie) {
    const auto tempsDebut = chrono::high_resolution_clock::now();
    const int nbVideos = static_cast<int>(listeFichierEntree.size());

    try {
        if (nbVideos < 1) throw invalid_argument("La liste d'entree est vide.");

        cout << "Etape 1/3 : Extraction des audios (incluant la reference)" << endl;

        const vector<vector<float> > audios = extraireEtChargerAudios(listeFichierEntree);

        cout << "Etape 2/3 : Calcul des decalages par rapport a la reference" << endl;

        const vector<double> decalagesEnSecondes = calculerDecalages(audios, listeFichierEntree);

        cout << "Etape 3/3 : Construction de la video complexe" << endl;

        const string cmd = construireCommandeFFmpeg(listeFichierEntree, decalagesEnSecondes, fichierSortie);

        if (system(cmd.c_str()) != 0) {
            throw runtime_error("L'execution de FFmpeg a echoue.");
        }

        cout << "Video complexe generee avec succes." << endl;
    } catch (const exception &e) {
        cerr << "[ERREUR FATALE] : " << e.what() << endl;
    }

    nettoyerTemporaires(nbVideos + 1);

    const auto tempsFin = chrono::high_resolution_clock::now();

    cout << format("\nTemps total : {:.2f} secondes.\n", chrono::duration<float>(tempsFin - tempsDebut).count());
}

vector<double> M_VideoComplexe::calculerDecalages(const vector<vector<float> > &audios,
                                                  const vector<string> &listeFichiers) {
    const size_t nbVideos = audios.size();
    vector decalages(nbVideos, 0.0);
    vector<future<void> > futures;
    int indexRef = 0;

    for (size_t i = 0; i < nbVideos; ++i) {
        if (i == static_cast<size_t>(indexRef)) continue;

        futures.push_back(async(launch::async, [&, i, indexRef] {
            if (!audios[indexRef].empty() && !audios[i].empty()) {
                auto xcorrResult = -static_cast<double>(xcorr(audios[indexRef], audios[i]));
                decalages[i] = xcorrResult / FREQUENCE_ECHANTILLONNAGE;
            }
        }));
    }

    for (auto &f: futures) f.get();

    for (size_t i = 0; i < nbVideos; ++i) {
        if (i == static_cast<size_t>(indexRef)) {
            bool estAudio = listeFichiers[0].ends_with(".mp3");
            cout << " -> " << (estAudio ? "Son " : "Video ") << i + 1 << " [REFERENCE]" << endl;
        } else {
            cout << " -> Decalage video " << i + 1 << " : " << decalages[i] << " s" << endl;
        }
    }

    return decalages;
}

string M_VideoComplexe::construireCommandeFFmpeg(const vector<string> &listeFichierEntree,
                                                 const vector<double> &decalagesEnSecondes,
                                                 const string &fichierSortie) {
    constexpr int indexRef = 0;

    const string &refPath = listeFichierEntree[indexRef];
    const bool refEstAudioSeul = refPath.ends_with(".mp3");

    vector<int> indexVideos;
    for (int i = 0; i < static_cast<int>(listeFichierEntree.size()); ++i) {
        if (i == indexRef && refEstAudioSeul) continue;
        indexVideos.push_back(i);
    }

    const size_t nbVideosMosaique = indexVideos.size();
    const int nbColonnes = ceil(sqrt(nbVideosMosaique));
    constexpr int largeurVideo = 640;
    constexpr int hauteurVideo = 360;

    ostringstream cmd;
    cmd << "ffmpeg -y -hide_banner -loglevel error ";

    for (size_t i = 0; i < listeFichierEntree.size(); ++i) {
        if (decalagesEnSecondes[i] > 0) cmd << "-ss " << decalagesEnSecondes[i] << " ";
        cmd << "-i \"" << listeFichierEntree[i] << "\" ";
    }

    cmd << "-filter_complex \"";

    for (size_t i = 0; i < nbVideosMosaique; ++i) {
        const int idx = indexVideos[i];
        cmd << "[" << idx << ":v]fps=30,scale=" << largeurVideo << ":" << hauteurVideo
                << ",setsar=1,format=yuv420p[v" << i << "];";
    }

    for (size_t i = 0; i < nbVideosMosaique; ++i) cmd << "[v" << i << "]";

    cmd << "xstack=inputs=" << nbVideosMosaique << ":shortest=1:fill=black:layout=";

    for (size_t i = 0; i < nbVideosMosaique; ++i) {
        const int ligne = static_cast<int>(i) / nbColonnes;
        const int colonne = static_cast<int>(i) % nbColonnes;
        if (i > 0) cmd << "|";
        cmd << (colonne * largeurVideo) << "_" << (ligne * hauteurVideo);
    }

    cmd << "[vout]\" ";

    cmd << "-map \"[vout]\" "
            << "-map " << indexRef << ":a "
            << "-c:v libx264 -preset ultrafast -pix_fmt yuv420p "
            << "\"" << fichierSortie << "\"";

    return cmd.str();
}

vector<vector<float> > M_VideoComplexe::extraireEtChargerAudios(const vector<string> &listeFichierEntree) {
    const size_t nbVideos = listeFichierEntree.size();

    vector<future<vector<float> > > futures;
    futures.reserve(nbVideos);

    for (size_t i = 0; i < nbVideos; ++i) {
        futures.push_back(async(launch::async, [i, &listeFichierEntree] {
            // Extraire

            string fichierTemporaire = TEMP_AUDIO_PREFIX + to_string(i) + ".bin";

            string cmd = "ffmpeg -y -hide_banner -loglevel error -i \"" + listeFichierEntree[i] +
                         "\" -vn -f f32le -ac 1 -ar " + to_string(FREQUENCE_ECHANTILLONNAGE) +
                         " -t 60 \"" + fichierTemporaire + "\"";

            if (system(cmd.c_str()) != 0) {
                throw runtime_error("Erreur FFmpeg : " + listeFichierEntree[i]);
            }

            // Charger

            const auto tailleEnOctets = filesystem::file_size(fichierTemporaire);

            vector<float> echantillons(tailleEnOctets / sizeof(float));

            ifstream fichierAudio(fichierTemporaire, ios::binary);

            if (!fichierAudio) {
                throw runtime_error("Erreur d'ouverture : " + fichierTemporaire);
            }

            fichierAudio.read(reinterpret_cast<char *>(echantillons.data()), tailleEnOctets);

            return echantillons;
        }));
    }

    vector<vector<float> > audios(nbVideos);

    for (size_t i = 0; i < nbVideos; ++i) {
        audios[i] = futures[i].get();
        cout << " -> Extraction terminee pour la video " << i + 1 << endl;
    }

    return audios;
}

int M_VideoComplexe::xcorr(const vector<float> &sig1, const vector<float> &sig2) {
    // Étape 1 : Vérification des données
    // On s'assure qu'aucun des deux signaux n'est vide avant de commencer les calculs.
    if (sig1.empty() || sig2.empty()) {
        throw invalid_argument("Un ou plusieurs signaux sont vides, correlation impossible.");
    }

    // Étape 2 : Calcul des tailles optimales pour la FFT
    // On détermine la taille nécessaire et on l'arrondit à la puissance de 2 supérieure.
    // Cela permet à la FFT d'être beaucoup plus rapide et d'éviter les erreurs mathématiques.
    const size_t tailleInitiale = sig1.size() + sig2.size() - 1;
    const auto tailleFFT = static_cast<size_t>(pow(2, ceil(log2(tailleInitiale))));
    const size_t tailleFrequence = tailleFFT / 2 + 1;

    // Étape 3 : Allocation sécurisée de la mémoire
    // On crée des fonctions de nettoyage sur mesure et on utilise des pointeurs intelligents (unique_ptr).
    // Ainsi, la mémoire sera toujours libérée correctement à la fin de la fonction.
    auto deleterReel = [](double *p) { fftw_free(p); };
    auto deleterComplexe = [](fftw_complex *p) { fftw_free(p); };

    const unique_ptr<double, decltype(deleterReel)> signalTemporel1(fftw_alloc_real(tailleFFT), deleterReel);
    const unique_ptr<double, decltype(deleterReel)> signalTemporel2(fftw_alloc_real(tailleFFT), deleterReel);
    const unique_ptr<double, decltype(deleterReel)> signalResultat(fftw_alloc_real(tailleFFT), deleterReel);

    const unique_ptr<fftw_complex, decltype(deleterComplexe)> spectre1(fftw_alloc_complex(tailleFrequence),
                                                                       deleterComplexe);
    const unique_ptr<fftw_complex, decltype(deleterComplexe)> spectre2(fftw_alloc_complex(tailleFrequence),
                                                                       deleterComplexe);
    const unique_ptr<fftw_complex, decltype(deleterComplexe)> spectreCroise(
        fftw_alloc_complex(tailleFrequence), deleterComplexe);

    // On vérifie que la mémoire a bien été allouée pour tous les tableaux.
    if (!signalTemporel1 || !signalTemporel2 || !signalResultat || !spectre1 || !spectre2 || !spectreCroise) {
        throw bad_alloc();
    }

    // Étape 4 : Centrage des signaux
    // On calcule la moyenne de chaque signal pour la soustraire ensuite.
    // Cela permet d'aligner les signaux autour de zéro.
    const double moyenne1 = accumulate(sig1.begin(), sig1.end(), 0.0) / static_cast<double>(sig1.size());
    const double moyenne2 = accumulate(sig2.begin(), sig2.end(), 0.0) / static_cast<double>(sig2.size());

    for (size_t i = 0; i < sig1.size(); ++i) signalTemporel1.get()[i] = sig1[i] - moyenne1;
    for (size_t i = 0; i < sig2.size(); ++i) signalTemporel2.get()[i] = sig2[i] - moyenne2;

    // Étape 5 : Remplissage avec des zéros (Zero-padding)
    // On remplit le reste des tableaux agrandis avec des zéros.
    fill(signalTemporel1.get() + sig1.size(), signalTemporel1.get() + tailleFFT, 0.0);
    fill(signalTemporel2.get() + sig2.size(), signalTemporel2.get() + tailleFFT, 0.0);

    // Étape 6 : Transformation dans le domaine fréquentiel (FFT Aller)
    // On crée les plans FFTW de manière sécurisée (avec le mutex) puis on exécute la transformation.
    // Les signaux temporels deviennent des spectres fréquentiels.
    fftw_plan planAller1, planAller2;
    {
        lock_guard lock(fftwMutex);
        planAller1 = fftw_plan_dft_r2c_1d(static_cast<int>(tailleFFT), signalTemporel1.get(), spectre1.get(),
                                          FFTW_ESTIMATE);
        planAller2 = fftw_plan_dft_r2c_1d(static_cast<int>(tailleFFT), signalTemporel2.get(), spectre2.get(),
                                          FFTW_ESTIMATE);
    }

    fftw_execute(planAller1);
    fftw_execute(planAller2);

    // Étape 7 : Calcul du spectre croisé (Multiplication complexe)
    // On multiplie le premier spectre par le conjugué complexe du second spectre.
    // C'est l'opération mathématique centrale de la corrélation croisée.
    for (size_t i = 0; i < tailleFrequence; ++i) {
        const double reel1 = spectre1.get()[i][0];
        const double imag1 = spectre1.get()[i][1];
        const double reel2 = spectre2.get()[i][0];
        const double imag2 = spectre2.get()[i][1];

        spectreCroise.get()[i][0] = reel1 * reel2 + imag1 * imag2;
        spectreCroise.get()[i][1] = imag1 * reel2 - reel1 * imag2;
    }

    // Étape 8 : Retour au domaine temporel (FFT Retour)
    // On transforme le spectre croisé résultant en un signal temporel classique.
    fftw_plan planRetour;
    {
        lock_guard lock(fftwMutex);
        planRetour = fftw_plan_dft_c2r_1d(static_cast<int>(tailleFFT), spectreCroise.get(), signalResultat.get(),
                                          FFTW_ESTIMATE);
    }

    fftw_execute(planRetour);

    // Étape 9 : Recherche du meilleur décalage
    // On trouve l'indice (la position) de la valeur maximale dans le résultat.
    // Ce pic correspond à l'endroit où les deux signaux se ressemblent le plus.
    const auto maxIt = max_element(signalResultat.get(), signalResultat.get() + tailleFFT);
    const size_t indexMax = distance(signalResultat.get(), maxIt);

    // Étape 10 : Nettoyage et formatage du résultat
    // On détruit les plans FFTW proprement (toujours protégé par le mutex).
    {
        lock_guard lock(fftwMutex);
        fftw_destroy_plan(planAller1);
        fftw_destroy_plan(planAller2);
        fftw_destroy_plan(planRetour);
    }

    // Si le décalage dépasse la moitié de la taille, c'est qu'il s'agit d'un décalage négatif ("enroulement" de la FFT).
    // On ajuste le calcul pour retourner un index positif ou négatif cohérent.
    const int indexMaxSigne = static_cast<int>(indexMax);
    const int tailleFFTSigne = static_cast<int>(tailleFFT);

    return indexMaxSigne > tailleFFTSigne / 2 ? indexMaxSigne - tailleFFTSigne : indexMaxSigne;
}

void M_VideoComplexe::nettoyerTemporaires(const int nbVideos) {
    for (int i = 0; i < nbVideos; ++i) {
        string nomFichier = TEMP_AUDIO_PREFIX + to_string(i) + ".bin";
        remove(nomFichier.c_str());
    }
}