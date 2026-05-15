#include "../../include/Modele/M_VideoComplexe.h"
#include <fftw3.h>
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

// Mutex global pour FFTW car la création de plans n'est pas thread-safe
mutex verrouFftw;

void M_VideoComplexe::genererVideoComplexe(const string *listeFichiersEntree, size_t nbVideos,
                                           const string &fichierSortie) {
    const auto tempsDebut = chrono::high_resolution_clock::now();

    float **audios = nullptr;
    size_t *taillesAudios = nullptr;
    double *decalagesEnSecondes = nullptr;

    try {
        if (nbVideos < 1) throw invalid_argument("La liste d'entree est vide.");

        taillesAudios = new size_t[nbVideos]();

        // Etape 1 : Extraction parallèle des flux audio vers des fichiers temporaires .bin
        cout << "Etape 1/3 : Extraction des audios (incluant la reference)" << endl;
        audios = extraireEtChargerAudios(listeFichiersEntree, nbVideos, taillesAudios);

        // Etape 2 : Analyse mathématique (X-Corr via FFT) pour trouver les délais de synchronisation
        cout << "Etape 2/3 : Calcul des decalages par rapport a la reference" << endl;
        decalagesEnSecondes = calculerDecalages(audios, taillesAudios, nbVideos, listeFichiersEntree);

        // Etape 3 : Composition finale avec FFmpeg (Mosaïque synchronisée)
        cout << "Etape 3/3 : Construction de la video complexe" << endl;
        const string commande = construireCommandeFFmpeg(listeFichiersEntree, nbVideos, decalagesEnSecondes,
                                                         fichierSortie);

        if (system(commande.c_str()) != 0) {
            throw runtime_error("L'execution de FFmpeg a echoue.");
        }

        cout << "Video complexe generee avec succes." << endl;
    } catch (const exception &e) {
        cerr << "[ERREUR FATALE] : " << e.what() << endl;
    }

    // Nettoyage de la mémoire dynamique
    if (audios != nullptr) {
        for (size_t i = 0; i < nbVideos; ++i) {
            delete[] audios[i];
        }
        delete[] audios;
    }
    delete[] taillesAudios;
    delete[] decalagesEnSecondes;

    nettoyerTemporaires(static_cast<int>(nbVideos) + 1);

    const auto tempsFin = chrono::high_resolution_clock::now();
    cout << format("\nTemps total : {:.2f} secondes.\n", chrono::duration<float>(tempsFin - tempsDebut).count());
}

double *M_VideoComplexe::calculerDecalages(const float *const*audios, const size_t *taillesAudios, size_t nbVideos,
                                           const string *listeFichiers) {
    double *decalages = new double[nbVideos]();
    future<void> *tachesFutures = new future<void>[nbVideos];
    int indexRef = 0; // La première vidéo sert de base temporelle (T=0)

    for (size_t i = 0; i < nbVideos; ++i) {
        if (i == static_cast<size_t>(indexRef)) continue;

        // Calcul des corrélations en parallèle pour gagner du temps sur les gros fichiers
        tachesFutures[i] = async(launch::async, [&, i, indexRef] {
            if (audios[indexRef] != nullptr && taillesAudios[indexRef] > 0 &&
                audios[i] != nullptr && taillesAudios[i] > 0) {
                // Le signe négatif permet d'obtenir le décalage relatif correct pour FFmpeg (-ss)
                const auto resultatXcorr = -static_cast<double>(xcorr(audios[indexRef], taillesAudios[indexRef],
                                                                      audios[i], taillesAudios[i]));
                decalages[i] = resultatXcorr / FREQUENCE_ECHANTILLONNAGE;
            }
        });
    }

    for (size_t i = 0; i < nbVideos; ++i) {
        if (i != static_cast<size_t>(indexRef) && tachesFutures[i].valid()) {
            tachesFutures[i].get();
        }
    }

    delete[] tachesFutures;
    return decalages;
}

string M_VideoComplexe::construireCommandeFFmpeg(const string *listeFichiersEntree, size_t nbVideos,
                                                 const double *decalagesEnSecondes, const string &fichierSortie) {
    constexpr int indexRef = 0;
    const string &cheminRef = listeFichiersEntree[indexRef];

    // Détermination si la référence est une piste audio pure (sans flux vidéo)
    string cheminMinuscule = cheminRef;
    ranges::transform(cheminMinuscule, cheminMinuscule.begin(), ::tolower);
    const bool refEstAudioSeul = cheminMinuscule.ends_with(".mp3") || cheminMinuscule.ends_with(".wav");

    int *indexVideos = new int[nbVideos];
    size_t nbVideosMosaique = 0;

    for (size_t i = 0; i < nbVideos; ++i) {
        if (i == indexRef && refEstAudioSeul) continue;
        indexVideos[nbVideosMosaique++] = static_cast<int>(i);
    }

    // Calcul de la géométrie de la mosaïque (grille carrée optimisée)
    const int nbColonnes = ceil(sqrt(nbVideosMosaique));
    constexpr int largeurVideo = 640;
    constexpr int hauteurVideo = 360;

    ostringstream fluxCommande;
    fluxCommande << "ffmpeg -y -hide_banner -loglevel error ";

    // Ajout des entrées avec leurs décalages respectifs calculés
    for (size_t i = 0; i < nbVideos; ++i) {
        if (decalagesEnSecondes[i] > 0) fluxCommande << "-ss " << decalagesEnSecondes[i] << " ";
        fluxCommande << "-i \"" << listeFichiersEntree[i] << "\" ";
    }

    fluxCommande << "-filter_complex \"";

    // Mise à l'échelle et normalisation du framerate pour chaque flux vidéo
    for (size_t i = 0; i < nbVideosMosaique; ++i) {
        const int idx = indexVideos[i];
        fluxCommande << "[" << idx << ":v]fps=30,scale=" << largeurVideo << ":" << hauteurVideo
                << ",setsar=1,format=yuv420p[v" << i << "];";
    }

    for (size_t i = 0; i < nbVideosMosaique; ++i) fluxCommande << "[v" << i << "]";

    // Assemblage spatial via xstack
    fluxCommande << "xstack=inputs=" << nbVideosMosaique << ":shortest=1:fill=black:layout=";

    for (size_t i = 0; i < nbVideosMosaique; ++i) {
        const int ligne = static_cast<int>(i) / nbColonnes;
        const int colonne = static_cast<int>(i) % nbColonnes;
        if (i > 0) fluxCommande << "|";
        fluxCommande << (colonne * largeurVideo) << "_" << (ligne * hauteurVideo);
    }

    fluxCommande << "[vout]\" ";

    // Mapping final : vidéo mosaïque + piste audio de la vidéo de référence uniquement
    fluxCommande << "-map \"[vout]\" "
            << "-map " << indexRef << ":a "
            << "-c:v libx264 -preset ultrafast -pix_fmt yuv420p "
            << "\"" << fichierSortie << "\"";

    delete[] indexVideos;
    return fluxCommande.str();
}

float **M_VideoComplexe::extraireEtChargerAudios(const string *listeFichiersEntree, size_t nbVideos,
                                                 size_t *taillesAudios) {
    struct ResultatAudio {
        float *donnees;
        size_t taille;
    };

    future<ResultatAudio> *tachesFutures = new future<ResultatAudio>[nbVideos];

    for (size_t i = 0; i < nbVideos; ++i) {
        tachesFutures[i] = async(launch::async, [i, listeFichiersEntree] {
            string fichierTemporaire = TEMP_AUDIO_PREFIX + to_string(i) + ".bin";

            // Extraction audio en format Raw Float 32-bit Little Endian, Mono
            string commande = "ffmpeg -y -hide_banner -loglevel error -i \"" + listeFichiersEntree[i] +
                              "\" -vn -f f32le -ac 1 -ar " + to_string(FREQUENCE_ECHANTILLONNAGE) +
                              " -t 60 \"" + fichierTemporaire + "\"";

            if (system(commande.c_str()) != 0) {
                throw runtime_error("Erreur FFmpeg : " + listeFichiersEntree[i]);
            }

            const auto tailleEnOctets = filesystem::file_size(fichierTemporaire);
            const size_t nbEchantillons = tailleEnOctets / sizeof(float);
            float *echantillons = new float[nbEchantillons];

            ifstream fichierAudio(fichierTemporaire, ios::binary);
            if (!fichierAudio) {
                delete[] echantillons;
                throw runtime_error("Erreur d'ouverture : " + fichierTemporaire);
            }

            fichierAudio.read(reinterpret_cast<char *>(echantillons), tailleEnOctets);
            return ResultatAudio{echantillons, nbEchantillons};
        });
    }

    float **audios = new float *[nbVideos];
    for (size_t i = 0; i < nbVideos; ++i) {
        ResultatAudio res = tachesFutures[i].get();
        audios[i] = res.donnees;
        taillesAudios[i] = res.taille;
    }

    delete[] tachesFutures;
    return audios;
}

int M_VideoComplexe::xcorr(const float *sig1, size_t taille1, const float *sig2, size_t taille2) {
    // Calcul de la taille de FFT (prochaine puissance de 2 pour la performance)
    const size_t tailleInitiale = taille1 + taille2 - 1;
    const auto tailleFFT = static_cast<size_t>(pow(2, ceil(log2(tailleInitiale))));
    const size_t tailleFrequence = tailleFFT / 2 + 1;

    auto destructeurReel = [](double *p) { fftw_free(p); };
    auto destructeurComplexe = [](fftw_complex *p) { fftw_free(p); };

    // Allocation via FFTW pour garantir l'alignement mémoire (SIMD)
    const unique_ptr<double, decltype(destructeurReel)> signalTemporel1(fftw_alloc_real(tailleFFT), destructeurReel);
    const unique_ptr<double, decltype(destructeurReel)> signalTemporel2(fftw_alloc_real(tailleFFT), destructeurReel);
    const unique_ptr<double, decltype(destructeurReel)> signalResultat(fftw_alloc_real(tailleFFT), destructeurReel);

    const unique_ptr<fftw_complex, decltype(destructeurComplexe)> spectre1(
        fftw_alloc_complex(tailleFrequence), destructeurComplexe);
    const unique_ptr<fftw_complex, decltype(destructeurComplexe)> spectre2(
        fftw_alloc_complex(tailleFrequence), destructeurComplexe);
    const unique_ptr<fftw_complex, decltype(destructeurComplexe)> spectreCroise(
        fftw_alloc_complex(tailleFrequence), destructeurComplexe);

    // Centrage des signaux (soustraction de la moyenne) pour améliorer la précision de corrélation
    const double moyenne1 = accumulate(sig1, sig1 + taille1, 0.0) / static_cast<double>(taille1);
    const double moyenne2 = accumulate(sig2, sig2 + taille2, 0.0) / static_cast<double>(taille2);

    for (size_t i = 0; i < taille1; ++i) signalTemporel1.get()[i] = sig1[i] - moyenne1;
    for (size_t i = 0; i < taille2; ++i) signalTemporel2.get()[i] = sig2[i] - moyenne2;

    // Zero-padding pour atteindre la taille de FFT
    fill(signalTemporel1.get() + taille1, signalTemporel1.get() + tailleFFT, 0.0);
    fill(signalTemporel2.get() + taille2, signalTemporel2.get() + tailleFFT, 0.0);

    fftw_plan planAller1, planAller2;
    {
        lock_guard verrou(verrouFftw);
        planAller1 = fftw_plan_dft_r2c_1d(static_cast<int>(tailleFFT), signalTemporel1.get(), spectre1.get(),
                                          FFTW_ESTIMATE);
        planAller2 = fftw_plan_dft_r2c_1d(static_cast<int>(tailleFFT), signalTemporel2.get(), spectre2.get(),
                                          FFTW_ESTIMATE);
    }

    fftw_execute(planAller1);
    fftw_execute(planAller2);

    // Multiplication dans le domaine fréquentiel (Convolution avec signal 2 conjugué)
    for (size_t i = 0; i < tailleFrequence; ++i) {
        const double reel1 = spectre1.get()[i][0];
        const double imag1 = spectre1.get()[i][1];
        const double reel2 = spectre2.get()[i][0];
        const double imag2 = spectre2.get()[i][1];

        spectreCroise.get()[i][0] = reel1 * reel2 + imag1 * imag2;
        spectreCroise.get()[i][1] = imag1 * reel2 - reel1 * imag2;
    }

    fftw_plan planRetour;
    {
        lock_guard verrou(verrouFftw);
        planRetour = fftw_plan_dft_c2r_1d(static_cast<int>(tailleFFT), spectreCroise.get(), signalResultat.get(),
                                          FFTW_ESTIMATE);
    }

    fftw_execute(planRetour);

    // Recherche du pic de corrélation
    const auto iterateurMax = max_element(signalResultat.get(), signalResultat.get() + tailleFFT);
    const size_t indexMax = distance(signalResultat.get(), iterateurMax);

    {
        lock_guard verrou(verrouFftw);
        fftw_destroy_plan(planAller1);
        fftw_destroy_plan(planAller2);
        fftw_destroy_plan(planRetour);
    }

    // Gestion de la circularité de la FFT pour obtenir un décalage négatif ou positif
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