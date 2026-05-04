#include "Controleur/C_LecteurPhysiqueLocal.h"
#include <filesystem>
#include <iostream>

using namespace std;

C_LecteurPhysiqueLocal::C_LecteurPhysiqueLocal(const string &ipMulticast, const int port,
                                               const vector<vector<string> > &configLecteurs)
    : udp(ipMulticast, port), configLecteurs(configLecteurs) {
    if (filesystem::exists(CHEMIN_VIDEO)) modeleLecteur.lireVideo(CHEMIN_VIDEO);
}

C_LecteurPhysiqueLocal::~C_LecteurPhysiqueLocal() {
    if (threadGeneration.joinable()) threadGeneration.join();
}

void C_LecteurPhysiqueLocal::initialiserSession(const vector<string> &fichiers) {
    if (generationEnCours || fichiers.size() < 2) return;

    modeleLecteur.pause();
    generationEnCours = true;

    if (threadGeneration.joinable()) threadGeneration.join();

    threadGeneration = thread([this, fichiers]() {
        try {
            if (!filesystem::exists("videosComplexes")) filesystem::create_directory("videosComplexes");

            // 1. Conversion de la configuration en tableau de LecteurSpec
            size_t nbLecteurs = configLecteurs.size();
            LecteurSpec* specs = new LecteurSpec[nbLecteurs];

            for (size_t i = 0; i < nbLecteurs; ++i) {
                // Vérification de sécurité pour s'assurer que le vecteur contient bien les 3 éléments attendus
                if (configLecteurs[i].size() >= 3) {
                    specs[i].id = configLecteurs[i][0];
                    specs[i].ip = configLecteurs[i][1];
                    specs[i].nbVideos = configLecteurs[i][2];
                }
            }

            // 2. Préparation de la session avec le nouveau format
            session.preparerSessionLecture(specs, nbLecteurs);

            // Nettoyage immédiat du tableau temporaire
            delete[] specs;

            // 3. Génération de la vidéo
            // fichiers.data() fournit le const string* attendu, fichiers.size() fournit la taille
            session.genererVideoComplexe(fichiers.data(), fichiers.size());

            // 4. Upload
            session.uploaderVideoComplexe();

            videoGeneree = true;
        } catch (const exception &e) {
            cerr << "Erreur Génération: " << e.what() << '\n';
        }
        generationEnCours = false;
    });
}

void C_LecteurPhysiqueLocal::mettreAJour() {
    if (videoGeneree) {
        modeleLecteur.lireVideo(CHEMIN_VIDEO);
        videoGeneree = false;
    }
}

void C_LecteurPhysiqueLocal::basculerPlayPause() {
    const bool enLecture = modeleLecteur.estEnLecture();

    if (enLecture) {
        modeleLecteur.pause();
        udp.transmettreCommande(Expediteur::MASTER, TypeCommande::ORDRE, Action::PAUSE, 0.0f);
    } else {
        modeleLecteur.play();
        udp.transmettreCommande(Expediteur::MASTER, TypeCommande::ORDRE, Action::PLAY, 0.0f);
    }
}

void C_LecteurPhysiqueLocal::modifierVolume(const float volume, const bool muet) {
    volumeCourant = muet ? 0.0f : volume;
    modeleLecteur.setVolume(static_cast<int>(volumeCourant));

    udp.transmettreCommande(Expediteur::MASTER, TypeCommande::ORDRE, Action::VOLUME, volumeCourant);
}

void C_LecteurPhysiqueLocal::modifierProgression(const float progression, const bool enGlissement, const bool restaurerLecture) {
    modeleLecteur.setTime(progression);

    udp.transmettreCommande(Expediteur::MASTER, TypeCommande::ORDRE, Action::PROGRESSION, progression);

    if (enGlissement) {
        modeleLecteur.pause();
        modeleLecteur.setVolume(0);
    } else {
        modeleLecteur.setVolume(static_cast<int>(volumeCourant));

        if (restaurerLecture) {
            modeleLecteur.play();
        } else {
            modeleLecteur.pause();
        }
    }
}
