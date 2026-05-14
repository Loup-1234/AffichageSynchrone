#include "Controleur/C_LecteurPhysiqueLocal.h"
#include <filesystem>
#include <iostream>

using namespace std;

C_LecteurPhysiqueLocal::C_LecteurPhysiqueLocal(const string &ipMulticast, const int port,
                                               const vector<vector<string> > &configLecteurs)
    : udp(ipMulticast, port), configLecteurs(configLecteurs) {
    // Charge la vidéo par défaut si elle existe déjà au démarrage
    if (filesystem::exists(CHEMIN_VIDEO)) {
        modeleLecteur.lireVideo(CHEMIN_VIDEO);
    }
}

C_LecteurPhysiqueLocal::~C_LecteurPhysiqueLocal() {
    // Assure que le thread de génération est terminé avant la destruction de l'objet
    if (threadGeneration.joinable()) {
        threadGeneration.join();
    }
}

void C_LecteurPhysiqueLocal::initialiserSession(const vector<string> &fichiers) {
    // Protection contre les clics multiples ou les sessions vides
    if (generationEnCours || fichiers.size() < 2) return;

    modeleLecteur.pause();
    generationEnCours = true;

    // Nettoyage du thread précédent s'il existe
    if (threadGeneration.joinable()) threadGeneration.join();

    // Lancement du traitement asynchrone (Génération + Upload)
    threadGeneration = thread([this, fichiers]() {
        try {
            if (!filesystem::exists("videosComplexes")) {
                filesystem::create_directory("videosComplexes");
            }

            // 1. Conversion du vecteur de config en structure C-style pour le moteur de session
            size_t nbLecteurs = configLecteurs.size();
            LecteurSpec *specs = new LecteurSpec[nbLecteurs];

            for (size_t i = 0; i < nbLecteurs; ++i) {
                if (configLecteurs[i].size() >= 3) {
                    specs[i].id = configLecteurs[i][0];
                    specs[i].ip = configLecteurs[i][1];
                    specs[i].nbVideos = configLecteurs[i][2];
                }
            }

            // 2. Préparation des métadonnées de la session
            session.preparerSessionLecture(specs, nbLecteurs);
            delete[] specs;

            // 3. Synthèse de la vidéo complexe (appel système/FFmpeg via la session)
            session.genererVideoComplexe(fichiers.data(), fichiers.size());

            // 4. Envoi des fichiers aux différents lecteurs distants
            transfertEnCours = true;
            session.uploaderVideoComplexe();
            transfertEnCours = false;

            // Signale au thread principal que la vidéo peut être chargée
            videoGeneree = true;
        } catch (const exception &e) {
            cerr << "Erreur Génération: " << e.what() << '\n';
        }
        generationEnCours = false;
    });
}

void C_LecteurPhysiqueLocal::basculerPlayPause() {
    const bool enLecture = modeleLecteur.estEnLecture();

    if (enLecture) {
        modeleLecteur.pause();
        udp.transmettreCommande(Expediteur::MASTER, TypeCommande::ORDRE, Action::PAUSE, 0.0f);
    } else {
        if (modeleLecteur.estTermine()) {
            // Si la vidéo est finie, on recommence au début
            modeleLecteur.stop();
            modeleLecteur.demarrer();
            udp.transmettreCommande(Expediteur::MASTER, TypeCommande::ORDRE, Action::PROGRESSION, 0.0f);
        } else {
            modeleLecteur.play();
            udp.transmettreCommande(Expediteur::MASTER, TypeCommande::ORDRE, Action::PLAY, 0.0f);
        }
    }
}

void C_LecteurPhysiqueLocal::modifierVolume(const float volume, const bool muet) {
    volumeCourant = muet ? 0.0f : volume;
    modeleLecteur.setVolume(static_cast<int>(volumeCourant));

    udp.transmettreCommande(Expediteur::MASTER, TypeCommande::ORDRE, Action::VOLUME, volumeCourant);
}

void C_LecteurPhysiqueLocal::modifierProgression(const float progression, const bool enGlissement,
                                                 const bool restaurerLecture) {
    modeleLecteur.setTime(progression);
    udp.transmettreCommande(Expediteur::MASTER, TypeCommande::ORDRE, Action::PROGRESSION, progression);

    if (enGlissement) {
        // Optimisation : coupe le son et met en pause pendant que l'utilisateur déplace le curseur
        modeleLecteur.pause();
        modeleLecteur.setVolume(0);
    } else {
        // Fin du déplacement : on rétablit l'état sonore et de lecture
        modeleLecteur.setVolume(static_cast<int>(volumeCourant));
        if (restaurerLecture) {
            modeleLecteur.play();
        } else {
            modeleLecteur.pause();
        }
    }
}

void C_LecteurPhysiqueLocal::modifierVitesse(const float vitesse) {
    modeleLecteur.setVitesse(vitesse);
    udp.transmettreCommande(Expediteur::MASTER, TypeCommande::ORDRE, Action::VITESSE, vitesse);
}

void C_LecteurPhysiqueLocal::mettreAJour() {
    // Cette méthode doit être appelée dans la boucle principale (UI thread)
    // Elle permet de charger la vidéo une fois que le thread de génération a terminé
    if (videoGeneree) {
        modeleLecteur.lireVideo(CHEMIN_VIDEO);
        videoGeneree = false;
    }
}

void C_LecteurPhysiqueLocal::stopper() {
    modeleLecteur.stop();
    udp.transmettreCommande(Expediteur::MASTER, TypeCommande::ORDRE, Action::STOP, 0.0f);
}