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
        }
        cout << "[DEBUG] [Video Complexe - ID " << idLecteur << "] Fichier audio detecte comme reference." << endl;
    }

    const auto tempsDebut = chrono::high_resolution_clock::now();
    cout << "[DEBUG] [Video Complexe - ID " << idLecteur << "] Debut de la generation de la video complexe..." << endl;

    float **audios = nullptr;
    size_t *taillesAudios = nullptr;
    double *decalagesEnSecondes = nullptr;

    try {
        if (nbVideos < 1) throw invalid_argument("La liste d'entree est vide.");

        taillesAudios = new size_t[nbVideos]();

        // Etape 1 : Extraction des pistes audio
        cout << "[DEBUG] [Video Complexe - ID " << idLecteur << "] Etape 1/3 : Extraction des audios (reduite a 20s)..." << endl;
        audios = extraireEtChargerAudios(listeFichiersEntree, nbVideos, taillesAudios, idLecteur);

        // Etape 2 : Calcul de la synchronisation
        cout << "[DEBUG] [Video Complexe - ID " << idLecteur << "] Etape 2/3 : Calcul des decalages par rapport a la reference..." << endl;
        decalagesEnSecondes = calculerDecalages(audios, taillesAudios, nbVideos, listeFichiersEntree, idLecteur);

        // Etape 3 : Generation de la mosaique via FFmpeg
        cout << "[DEBUG] [Video Complexe - ID " << idLecteur << "] Etape 3/3 : Construction de la video complexe..." << endl;
        const string commande = construireCommandeFFmpeg(listeFichiersEntree, nbVideos, decalagesEnSecondes,
                                                         fichierSortie, masquerReference, idLecteur);

        cout << "[DEBUG] [Video Complexe - ID " << idLecteur << "] Execution de la commande FFmpeg multithread..." << endl;
        if (system(commande.c_str()) != 0) {
            throw runtime_error("L'execution de FFmpeg a echoue.");
        }

        cout << "[DEBUG] [Video Complexe - ID " << idLecteur << "] Video complexe generee avec succes." << endl;
    } catch (const exception &e) {
        cerr << "[DEBUG] [Video Complexe - ID " << idLecteur << "] [ERREUR FATALE] : " << e.what() << endl;
    }

    if (audios != nullptr) {
        for (size_t i = 0; i < nbVideos; ++i) {
            delete[] audios[i];
        }
        delete[] audios;
    }
    delete[] taillesAudios;
    delete[] decalagesEnSecondes;

    cout << "[DEBUG] [Video Complexe - ID " << idLecteur << "] Nettoyage des fichiers binaires temporaires..." << endl;
    nettoyerTemporaires(static_cast<int>(nbVideos));

    const auto tempsFin = chrono::high_resolution_clock::now();
    cout << format("[DEBUG] [Video Complexe - ID {}] Temps total : {:.2f} secondes.\n", idLecteur, chrono::duration<float>(tempsFin - tempsDebut).count());
}

double *M_VideoComplexe::calculerDecalages(const float *const*audios, const size_t *taillesAudios, size_t nbVideos,
                                           const string *listeFichiers, int idLecteur) {
    cout << "[DEBUG] [Video Complexe - ID " << idLecteur << "] Initialisation des taches asynchrones de correlation..." << endl;
    double *decalages = new double[nbVideos]();
    future<void> *tachesFutures = new future<void>[nbVideos];
    int indexRef = 0;

    for (size_t i = 0; i < nbVideos; ++i) {
        if (i == static_cast<size_t>(indexRef)) continue;

        tachesFutures[i] = async(launch::async, [&, i, indexRef] {
            if (audios[indexRef] != nullptr && taillesAudios[indexRef] > 0 &&
                audios[i] != nullptr && taillesAudios[i] > 0) {
                const auto resultatXcorr = -static_cast<double>(xcorr(audios[indexRef], taillesAudios[indexRef],
                                                                      audios[i], taillesAudios[i]));
                decalages[i] = resultatXcorr / FREQUENCE_ECHANTILLONNAGE;
            }
        });
    }

    for (size_t i = 0; i < nbVideos; ++i) {
        if (i != static_cast<size_t>(indexRef) && tachesFutures[i].valid()) {
            tachesFutures[i].get();
            cout << "[DEBUG] [Video Complexe - ID " << idLecteur << "] Decalage calcule pour l'index " << i << " : " << decalages[i] << " secondes." << endl;
        }
    }

    delete[] tachesFutures;
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

    int *indexVideos = new int[nbVideos];
    size_t nbVideosMosaique = 0;

    for (size_t i = 0; i < nbVideos; ++i) {
        if (i == indexRef && (refEstAudioSeul || masquerReference)) {
            continue;
        }
        indexVideos[nbVideosMosaique++] = static_cast<int>(i);
    }

    ostringstream fluxCommande;
    fluxCommande << "ffmpeg -y -hide_banner -loglevel error -threads 0 ";

    for (size_t i = 0; i < nbVideos; ++i) {
        if (decalagesEnSecondes[i] > 0) fluxCommande << "-ss " << decalagesEnSecondes[i] << " ";
        fluxCommande << "-i \"" << listeFichiersEntree[i] << "\" ";
    }

    // OPTIMISATION : Ajout de flags=fast_bilinear pour alleger le redimensionnement et -c:a copy pour cloner l'audio
    if (nbVideosMosaique == 1) {
        const int idxSeul = indexVideos[0];
        fluxCommande << "-filter_complex \"[" << idxSeul << ":v]fps=30,scale=640:360:flags=fast_bilinear,setsar=1,format=yuv420p[vout]\" "
                     << "-map \"[vout]\" -map " << indexRef << ":a -c:a copy ";
    }
    else if (nbVideosMosaique > 1) {
        fluxCommande << "-filter_complex \"";

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

        fluxCommande << "[vout]\" -map \"[vout]\" -map " << indexRef << ":a -c:a copy ";
    }
    else {
        cout << "[DEBUG] [Video Complexe - ID " << idLecteur << "] Aucune piste video detectee pour la mosaique. Generation d'un fond noir." << endl;
        fluxCommande << "-filter_complex \"color=c=black:s=640x360:r=30[vout]\" -map \"[vout]\" -map " << indexRef << ":a -c:a copy ";
    }

    // OPTIMISATION : Ajout de -tune fastdecode et -bf 0 pour accelerer l'encodage x264 brut
    fluxCommande << "-c:v libx264 -preset ultrafast -tune fastdecode -bf 0 -pix_fmt yuv420p "
                 << "\"" << fichierSortie << "\"";

    delete[] indexVideos;
    return fluxCommande.str();
}

float **M_VideoComplexe::extraireEtChargerAudios(const string *listeFichiersEntree, size_t nbVideos,
                                                 size_t *taillesAudios, int idLecteur) {
    struct ResultatAudio {
        float *donnees;
        size_t taille;
    };

    vector<future<ResultatAudio>> tachesFutures;
    tachesFutures.reserve(nbVideos);

    for (size_t i = 0; i < nbVideos; ++i) {
        tachesFutures.push_back(async(launch::async, [i, listeFichiersEntree] {
            string fichierTemporaire = TEMP_AUDIO_PREFIX + to_string(i) + ".bin";

            string commande = "ffmpeg -y -hide_banner -loglevel error -i \"" + listeFichiersEntree[i] +
                              "\" -vn -f f32le -ac 1 -ar " + to_string(FREQUENCE_ECHANTILLONNAGE) +
                              " -t 20 \"" + fichierTemporaire + "\"";

            if (system(commande.c_str()) != 0) {
                throw runtime_error("Fichier introuvable ou illisible par FFmpeg -> " + listeFichiersEntree[i]);
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
        }));
    }

    float **audios = new float *[nbVideos]();

    try {
        for (size_t i = 0; i < nbVideos; ++i) {
            ResultatAudio res = tachesFutures[i].get();
            audios[i] = res.donnees;
            taillesAudios[i] = res.taille;
            cout << "[DEBUG] [Video Complexe] Audio extrait - Index " << i << " : " << res.taille << " echantillons charges." << endl;
        }
    } catch (...) {
        for (size_t i = 0; i < nbVideos; ++i) {
            if (audios[i] != nullptr) delete[] audios[i];
        }
        delete[] audios;
        throw;
    }

    return audios;
}

int M_VideoComplexe::xcorr(const float *sig1, size_t taille1, const float *sig2, size_t taille2) {
    const size_t tailleInitiale = taille1 + taille2 - 1;
    const auto tailleFFT = static_cast<size_t>(pow(2, ceil(log2(tailleInitiale))));
    const size_t tailleFrequence = tailleFFT / 2 + 1;

    auto destructeurReel = [](double *p) { fftw_free(p); };
    auto destructeurComplexe = [](fftw_complex *p) { fftw_free(p); };

    const unique_ptr<double, decltype(destructeurReel)> signalTemporel1(fftw_alloc_real(tailleFFT), destructeurReel);
    const unique_ptr<double, decltype(destructeurReel)> signalTemporel2(fftw_alloc_real(tailleFFT), destructeurReel);
    const unique_ptr<double, decltype(destructeurReel)> signalResultat(fftw_alloc_real(tailleFFT), destructeurReel);

    const unique_ptr<fftw_complex, decltype(destructeurComplexe)> spectre1(
        fftw_alloc_complex(tailleFrequence), destructeurComplexe);
    const unique_ptr<fftw_complex, decltype(destructeurComplexe)> spectre2(
        fftw_alloc_complex(tailleFrequence), destructeurComplexe);
    const unique_ptr<fftw_complex, decltype(destructeurComplexe)> spectreCroise(
        fftw_alloc_complex(tailleFrequence), destructeurComplexe);

    const double moyenne1 = accumulate(sig1, sig1 + taille1, 0.0) / static_cast<double>(taille1);
    const double moyenne2 = accumulate(sig2, sig2 + taille2, 0.0) / static_cast<double>(taille2);

    for (size_t i = 0; i < taille1; ++i) signalTemporel1.get()[i] = sig1[i] - moyenne1;
    for (size_t i = 0; i < taille2; ++i) signalTemporel2.get()[i] = sig2[i] - moyenne2;

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

    const auto iterateurMax = max_element(signalResultat.get(), signalResultat.get() + tailleFFT);
    const size_t indexMax = distance(signalResultat.get(), iterateurMax);

    {
        lock_guard verrou(verrouFftw);
        fftw_destroy_plan(planAller1);
        fftw_destroy_plan(planAller2);
        fftw_destroy_plan(planRetour);
    }

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