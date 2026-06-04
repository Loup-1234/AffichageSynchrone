#include "Modele/M_VideoComplexe.h"
#include <fftw3.h>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <thread>
#include <iomanip>
#include <iostream>
#include <memory>
#include <mutex>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;

// Verrou global pour securiser l'utilisation de FFTW
mutex verrouFftw;

void M_VideoComplexe::genererVideoComplexe(const string *listeFichiersEntree, size_t nbVideos,
                                           const string &fichierSortie, bool masquerReference, int idLecteur) {

    if (nbVideos > 0) {
        string cheminRef = listeFichiersEntree[0];
        ranges::transform(cheminRef, cheminRef.begin(), ::tolower);

        if (cheminRef.ends_with(".mp3") || cheminRef.ends_with(".wav")) {
            masquerReference = false;
            cout << "[DEBUG] [Video Complexe - ID " << idLecteur << "] Fichier audio detecte comme reference." << endl;
        }

        cout << "[DEBUG] [Video Complexe - ID " << idLecteur << "] Masquage de la video de reference : "
             << (masquerReference ? "Oui" : "Non") << endl;
    }

    const auto tempsDebut = chrono::high_resolution_clock::now();
    cout << "[DEBUG] [Video Complexe - ID " << idLecteur << "] Debut de la generation de la video complexe..." << endl;

    try {
        if (nbVideos < 1) throw invalid_argument("La liste d'entree est vide.");

        // Etape 1 : Extraction des pistes audio
        cout << "[DEBUG] [Video Complexe - ID " << idLecteur << "] Etape 1/3 : Extraction des audios (reduite a 20s)..." << endl;
        auto audios = extraireEtChargerAudios(listeFichiersEntree, nbVideos, idLecteur);

        // Etape 2 : Calcul de la synchronisation
        cout << "[DEBUG] [Video Complexe - ID " << idLecteur << "] Etape 2/3 : Calcul des decalages par rapport a la reference..." << endl;
        auto decalagesEnSecondes = calculerDecalages(audios, nbVideos, listeFichiersEntree, idLecteur);

        // Etape 3 : Generation de la mosaique via FFmpeg
        cout << "[DEBUG] [Video Complexe - ID " << idLecteur << "] Etape 3/3 : Construction de la video complexe..." << endl;
        const string commande = construireCommandeFFmpeg(listeFichiersEntree, nbVideos, decalagesEnSecondes.data(),
                                                         fichierSortie, masquerReference, idLecteur);

        cout << "[DEBUG] [Video Complexe - ID " << idLecteur << "] Execution de la commande FFmpeg..." << endl;

        if (system(commande.c_str()) != 0) {
            throw runtime_error("L'execution de FFmpeg a echoue.");
        }

        cout << "[DEBUG] [Video Complexe - ID " << idLecteur << "] Video complexe generee avec succes." << endl;
    } catch (const exception &e) {
        cerr << "[DEBUG] [Video Complexe - ID " << idLecteur << "] [ERREUR FATALE] : " << e.what() << endl;
    }

    cout << "[DEBUG] [Video Complexe - ID " << idLecteur << "] Nettoyage des fichiers binaires temporaires..." << endl;
    nettoyerTemporaires(static_cast<int>(nbVideos), idLecteur);

    const auto tempsFin = chrono::high_resolution_clock::now();
    cout << "[DEBUG] [Video Complexe - ID " << idLecteur << "] Temps total : "
         << fixed << setprecision(2) << chrono::duration<float>(tempsFin - tempsDebut).count()
         << " secondes." << endl;
}

vector<double> M_VideoComplexe::calculerDecalages(const vector<vector<float>>& audios, size_t nbVideos,
                                                  const string *listeFichiers, int idLecteur) {
    cout << "[DEBUG] [Video Complexe - ID " << idLecteur << "] Initialisation des taches de correlation..." << endl;

    vector<double> decalages(nbVideos, 0.0);
    vector<thread> lesThreads;
    lesThreads.reserve(nbVideos);
    int indexRef = 0;

    for (size_t i = 0; i < nbVideos; ++i) {
        if (i == static_cast<size_t>(indexRef)) continue;

        lesThreads.push_back(thread([&, i, indexRef]() {
            if (!audios[indexRef].empty() && !audios[i].empty()) {
                const auto resultatXcorr = -static_cast<double>(xcorr(audios[indexRef].data(), audios[indexRef].size(),
                                                                      audios[i].data(), audios[i].size()));
                decalages[i] = resultatXcorr / FREQUENCE_ECHANTILLONNAGE;
            }
        }));
    }

    for (auto &t : lesThreads) {
        if (t.joinable()) t.join();
    }

    for (size_t i = 0; i < nbVideos; ++i) {
        if (i != static_cast<size_t>(indexRef)) {
            cout << "[DEBUG] [Video Complexe - ID " << idLecteur << "] Decalage calcule pour l'index " << i << " : " << decalages[i] << " secondes." << endl;
        }
    }

    return decalages;
}

string M_VideoComplexe::construireCommandeFFmpeg(const string *listeFichiersEntree, size_t nbVideos,
                                                 const double *decalagesEnSecondes, const string &fichierSortie,
                                                 bool masquerReference, int idLecteur) {
    cout << "[DEBUG] [Video Complexe - ID " << idLecteur << "] Construction de la commande FFmpeg..." << endl;
    constexpr int indexRef = 0;
    const string &cheminRef = listeFichiersEntree[indexRef];

    string cheminMinuscule = cheminRef;
    ranges::transform(cheminMinuscule, cheminMinuscule.begin(), ::tolower);
    const bool refEstAudioSeul = cheminMinuscule.ends_with(".mp3") || cheminMinuscule.ends_with(".wav");

    vector<int> indexVideos;
    indexVideos.reserve(nbVideos);

    for (size_t i = 0; i < nbVideos; ++i) {
        if (i == indexRef && (refEstAudioSeul || masquerReference)) {
            continue;
        }
        indexVideos.push_back(static_cast<int>(i));
    }

    const size_t nbVideosMosaique = indexVideos.size();
    ostringstream fluxCommande;

    fluxCommande << "ffmpeg -y -hide_banner -loglevel error -threads 0 ";

    for (size_t i = 0; i < nbVideos; ++i) {
        if (decalagesEnSecondes[i] > 0) fluxCommande << "-ss " << decalagesEnSecondes[i] << " ";
        fluxCommande << "-skip_loop_filter all -i \"" << listeFichiersEntree[i] << "\" ";
    }

    fluxCommande << "-filter_complex \"";

    if (nbVideosMosaique == 1) {
        const int idxSeul = indexVideos[0];

        if (masquerReference && idxSeul != indexRef && !refEstAudioSeul) {
            fluxCommande << "[" << indexRef << ":v]fps=30,scale=640:360:flags=fast_bilinear,setsar=1,format=yuv420p[refv];"
                         << "[" << idxSeul << ":v]fps=30,scale=640:360:flags=fast_bilinear,setsar=1,format=yuv420p[clientv];"
                         << "[refv][clientv]overlay=shortest=1[vout]";
        } else {
            fluxCommande << "[" << idxSeul << ":v]fps=30,scale=640:360:flags=fast_bilinear,setsar=1,format=yuv420p[vout]";
        }
        fluxCommande << "\" -map [vout] -map " << indexRef << ":a -c:a aac -b:a 128k ";
    }
    else if (nbVideosMosaique > 1) {
        for (size_t i = 0; i < nbVideosMosaique; ++i) {
            const int idx = indexVideos[i];
            fluxCommande << "[" << idx << ":v]fps=30,scale=640:360:flags=fast_bilinear,setsar=1,format=yuv420p[v" << i << "];";
        }

        for (size_t i = 0; i < nbVideosMosaique; ++i) fluxCommande << "[v" << i << "]";

        const int nbColonnes = ceil(sqrt(nbVideosMosaique));
        constexpr int largeurVideo = 640;
        constexpr int hauteurVideo = 360;

        fluxCommande << "xstack=inputs=" << nbVideosMosaique << ":shortest=1:fill=black:layout=";

        for (size_t i = 0; i < nbVideosMosaique; ++i) {
            const int ligne = static_cast<int>(i) / nbColonnes;
            const int colonne = static_cast<int>(i) % nbColonnes;
            if (i > 0) fluxCommande << "|";
            fluxCommande << (colonne * largeurVideo) << "_" << (ligne * hauteurVideo);
        }
        fluxCommande << "[vout]\" -map [vout] -map " << indexRef << ":a -c:a aac -b:a 128k ";
    }
    else {
        fluxCommande << "color=c=black:s=640x360:r=30[vout]\" -map [vout] -map " << indexRef << ":a -c:a aac -b:a 128k ";
    }

    fluxCommande << "-c:v libx264 -preset ultrafast -tune fastdecode -bf 0 -pix_fmt yuv420p -max_interleave_delta 0 -shortest "
                 << "\"" << fichierSortie << "\"";

    return fluxCommande.str();
}

vector<vector<float>> M_VideoComplexe::extraireEtChargerAudios(const string *listeFichiersEntree, size_t nbVideos, int idLecteur) {
    vector<vector<float>> audios(nbVideos);
    vector<thread> lesThreads;
    lesThreads.reserve(nbVideos);

    string messageErreurGlobal = "";
    mutex verrouErreur;

    for (size_t i = 0; i < nbVideos; ++i) {
        string cheminFichier = listeFichiersEntree[i];

        lesThreads.push_back(thread([i, idLecteur, cheminFichier, &audios, &messageErreurGlobal, &verrouErreur]() {
            try {
                string fichierTemporaire = TEMP_AUDIO_PREFIX + to_string(idLecteur) + "_" + to_string(i) + ".bin";

                string commande = "ffmpeg -y -hide_banner -loglevel error -threads 1 -i \"" + cheminFichier +
                                  "\" -vn -f f32le -ac 1 -ar " + to_string(FREQUENCE_ECHANTILLONNAGE) +
                                  " -t 20 \"" + fichierTemporaire + "\"";

                if (system(commande.c_str()) != 0) {
                    throw runtime_error("Fichier introuvable ou illisible par FFmpeg -> " + cheminFichier);
                }

                if (!filesystem::exists(fichierTemporaire) || filesystem::file_size(fichierTemporaire) == 0) {
                    throw runtime_error("Extraction audio echouee ou fichier verrouille -> " + fichierTemporaire);
                }

                const auto tailleEnOctets = filesystem::file_size(fichierTemporaire);
                const size_t nbEchantillons = tailleEnOctets / sizeof(float);

                vector<float> echantillons(nbEchantillons);

                ifstream fichierAudio(fichierTemporaire, ios::binary);
                if (!fichierAudio) {
                    throw runtime_error("Erreur d'ouverture du fichier binaire -> " + fichierTemporaire);
                }

                fichierAudio.read(reinterpret_cast<char *>(echantillons.data()), nbEchantillons * sizeof(float));

                audios[i] = move(echantillons);
            } catch (const exception &e) {
                lock_guard lock(verrouErreur);
                messageErreurGlobal = e.what();
            }
        }));
    }

    for (auto &t : lesThreads) {
        if (t.joinable()) t.join();
    }

    if (!messageErreurGlobal.empty()) {
        throw runtime_error(messageErreurGlobal);
    }

    for (size_t i = 0; i < nbVideos; ++i) {
        cout << "[DEBUG] [Video Complexe - ID " << idLecteur << "] Audio extrait - Index " << i << " : " << audios[i].size() << " echantillons charges." << endl;
    }

    return audios;
}

int M_VideoComplexe::xcorr(const float *sig1, size_t taille1, const float *sig2, size_t taille2) {
    // 1. Planification des dimensions pour la FFT
    const size_t tailleInitiale = taille1 + taille2 - 1; // Taille théorique du signal après corrélation
    // On cherche la puissance de 2 supérieure pour maximiser les performances de l'algorithme FFT
    const auto tailleFFT = static_cast<size_t>(pow(2, ceil(log2(tailleInitiale))));
    const size_t tailleFrequence = tailleFFT / 2 + 1; // Format R2C (Real to Complex) de FFTW

    // Destructeurs personnalisés pour sécuriser la mémoire FFTW avec des unique_ptr
    auto destructeurReel = [](double *p) { fftw_free(p); };
    auto destructeurComplexe = [](fftw_complex *p) { fftw_free(p); };

    // Allocation des buffers temporels et fréquentiels
    const unique_ptr<double, decltype(destructeurReel)> signalTemporel1(fftw_alloc_real(tailleFFT), destructeurReel);
    const unique_ptr<double, decltype(destructeurReel)> signalTemporel2(fftw_alloc_real(tailleFFT), destructeurReel);
    const unique_ptr<double, decltype(destructeurReel)> signalResultat(fftw_alloc_real(tailleFFT), destructeurReel);

    const unique_ptr<fftw_complex, decltype(destructeurComplexe)> spectre1(
        fftw_alloc_complex(tailleFrequence), destructeurComplexe);
    const unique_ptr<fftw_complex, decltype(destructeurComplexe)> spectre2(
        fftw_alloc_complex(tailleFrequence), destructeurComplexe);
    const unique_ptr<fftw_complex, decltype(destructeurComplexe)> spectreCroise(
        fftw_alloc_complex(tailleFrequence), destructeurComplexe);

    // 2. Préparation des signaux (Centrage et Zero-Padding)
    // On calcule la moyenne pour soustraire la composante continue (éviter les biais d'amplitude)
    const double moyenne1 = accumulate(sig1, sig1 + taille1, 0.0) / static_cast<double>(taille1);
    const double moyenne2 = accumulate(sig2, sig2 + taille2, 0.0) / static_cast<double>(taille2);

    for (size_t i = 0; i < taille1; ++i) signalTemporel1.get()[i] = sig1[i] - moyenne1;
    for (size_t i = 0; i < taille2; ++i) signalTemporel2.get()[i] = sig2[i] - moyenne2;

    // Remplissage de zéros (Zero-padding) pour atteindre la taille FFT et éviter le repliement circulaire
    fill(signalTemporel1.get() + taille1, signalTemporel1.get() + tailleFFT, 0.0);
    fill(signalTemporel2.get() + taille2, signalTemporel2.get() + tailleFFT, 0.0);

    // 3. Passage dans le domaine fréquentiel (FFT)
    fftw_plan planAller1, planAller2;
    {
        // FFTW n'étant pas thread-safe sur la création de plans, on verrouille
        lock_guard verrou(verrouFftw);
        planAller1 = fftw_plan_dft_r2c_1d(static_cast<int>(tailleFFT), signalTemporel1.get(), spectre1.get(),
                                          FFTW_ESTIMATE);
        planAller2 = fftw_plan_dft_r2c_1d(static_cast<int>(tailleFFT), signalTemporel2.get(), spectre2.get(),
                                          FFTW_ESTIMATE);
    }

    fftw_execute(planAller1);
    fftw_execute(planAller2);

    // 4. Multiplication des spectres (Corrélation <=> Produit par le conjugué complexe)
    for (size_t i = 0; i < tailleFrequence; ++i) {
        const double reel1 = spectre1.get()[i][0];
        const double imag1 = spectre1.get()[i][1];
        const double reel2 = spectre2.get()[i][0];
        const double imag2 = spectre2.get()[i][1];

        // Équivalent fréquentiel de la corrélation : Spectre1 * Conjugué(Spectre2)
        spectreCroise.get()[i][0] = reel1 * reel2 + imag1 * imag2;
        spectreCroise.get()[i][1] = imag1 * reel2 - reel1 * imag2;
    }

    // 5. Retour dans le domaine temporel (IFFT)
    fftw_plan planRetour;
    {
        lock_guard verrou(verrouFftw);
        planRetour = fftw_plan_dft_c2r_1d(static_cast<int>(tailleFFT), spectreCroise.get(), signalResultat.get(),
                                          FFTW_ESTIMATE);
    }

    fftw_execute(planRetour);

    // 6. Extraction du décalage (Lag)
    // Le pic d'amplitude maximale représente le point de corrélation maximale (le meilleur alignement)
    const auto iterateurMax = max_element(signalResultat.get(), signalResultat.get() + tailleFFT);
    const size_t indexMax = distance(signalResultat.get(), iterateurMax);

    // Libération des plans de calcul (Section critique)
    {
        lock_guard verrou(verrouFftw);
        fftw_destroy_plan(planAller1);
        fftw_destroy_plan(planAller2);
        fftw_destroy_plan(planRetour);
    }

    const int indexMaxSigne = static_cast<int>(indexMax);
    const int tailleFFTSigne = static_cast<int>(tailleFFT);

    // Ajustement pour les décalages négatifs : à cause de la périodicité de la FFT,
    // la seconde moitié du tableau correspond à des retards (lags négatifs).
    return indexMaxSigne > tailleFFTSigne / 2 ? indexMaxSigne - tailleFFTSigne : indexMaxSigne;
}

void M_VideoComplexe::nettoyerTemporaires(const int nbVideos, int idLecteur) {
    for (int i = 0; i < nbVideos; ++i) {
        string nomFichier = TEMP_AUDIO_PREFIX + to_string(idLecteur) + "_" + to_string(i) + ".bin";
        remove(nomFichier.c_str());
    }
}