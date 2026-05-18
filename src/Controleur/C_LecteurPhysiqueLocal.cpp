#include "Controleur/C_LecteurPhysiqueLocal.h"
#include "Modele/M_ReceveurUDP.h"

#include <filesystem>
#include <iostream>

using namespace std;

C_LecteurPhysiqueLocal::C_LecteurPhysiqueLocal(const string &ipMulticast, const int portCommandes, const int portDecouverte,
                                               const int portReponse, const vector<vector<string>> &configLecteurs)
    : udp(ipMulticast, portCommandes), configLecteurs(configLecteurs), m_adresseMulticast(ipMulticast),
      m_portDecouverte(portDecouverte), m_portReponse(portReponse) {

    modeleLecteur.collecterInfosLocales();
    demarrerEcouteMulticast();

    if (filesystem::exists(CHEMIN_VIDEO)) {
        modeleLecteur.lireVideo(CHEMIN_VIDEO);
    }
}

C_LecteurPhysiqueLocal::~C_LecteurPhysiqueLocal() {
    arreterEcouteMulticast();

    if (threadGeneration.joinable()) threadGeneration.join();
    if (threadRecherche.joinable()) threadRecherche.join();
    if (threadEcouteMulticast.joinable()) threadEcouteMulticast.join();
}

void C_LecteurPhysiqueLocal::arreterEcouteMulticast() {
    ecouteMulticastActive = false;
}

void C_LecteurPhysiqueLocal::demarrerEcouteMulticast() {
    ecouteMulticastActive = true;

    threadEcouteMulticast = thread([this]() {
        M_ReceveurUDP receveurMulticast(m_portDecouverte, m_adresseMulticast);
        char buffer[512];
        string ipEmetteur;

        while (ecouteMulticastActive) {
            int nbOctets = receveurMulticast.recevoirAvecTimeout(buffer, sizeof(buffer) - 1, ipEmetteur, 500);

            if (nbOctets > 0) {
                string jsonInfos = modeleLecteur.versJson();
                receveurMulticast.envoyerReponse(ipEmetteur, m_portReponse, jsonInfos);
            }
        }
    });
}

void C_LecteurPhysiqueLocal::lancerRechercheLecteurs() {
    if (rechercheEnCours) return;

    rechercheEnCours = true;
    resultatsRecherchePrets = false;

    if (threadRecherche.joinable()) threadRecherche.join();

    threadRecherche = thread([this]() {
        try {
            cacheLecteurs = session.rechercherLecteurs(m_adresseMulticast, m_portDecouverte, m_portReponse);
            resultatsRecherchePrets = true;
        } catch (const exception &e) {
            cerr << "Erreur Recherche Réseau: " << e.what() << '\n';
        }
        rechercheEnCours = false;
    });
}

vector<map<string, string>> C_LecteurPhysiqueLocal::getDerniersLecteursTrouves() {
    resultatsRecherchePrets = false;
    return cacheLecteurs;
}

void C_LecteurPhysiqueLocal::initialiserSession(const vector<string> &fichiers) {
    if (generationEnCours || fichiers.size() < 2) return;

    modeleLecteur.pause();
    generationEnCours = true;

    if (threadGeneration.joinable()) threadGeneration.join();

    threadGeneration = thread([this, fichiers]() {
        try {
            if (!filesystem::exists("videosComplexes")) {
                filesystem::create_directory("videosComplexes");
            }

            size_t nbLecteurs = configLecteurs.size();

            // OPTIMISATION ICI : Utilisation de vector au lieu de pointeurs bruts (new/delete)
            // Cela garantit qu'il n'y aura aucune fuite de mémoire si une erreur survient.
            vector<LecteurSpec> specs(nbLecteurs);

            for (size_t i = 0; i < nbLecteurs; ++i) {
                if (configLecteurs[i].size() >= 3) {
                    specs[i].id = configLecteurs[i][0];
                    specs[i].ip = configLecteurs[i][1];
                    specs[i].nbVideos = configLecteurs[i][2];
                }
            }

            // On utilise .data() pour passer le pointeur brut attendu par ta méthode
            session.preparerSessionLecture(specs.data(), nbLecteurs);
            session.genererVideoComplexe(fichiers.data(), fichiers.size());

            transfertEnCours = true;
            session.uploaderVideoComplexe();
            transfertEnCours = false;

            videoGeneree = true;
        } catch (const exception &e) {
            cerr << "Erreur Génération: " << e.what() << '\n';
        }
        generationEnCours = false;
    });
}

void C_LecteurPhysiqueLocal::basculerPlayPause() {
    if (modeleLecteur.estEnLecture()) {
        modeleLecteur.pause();
        udp.transmettreCommande(Expediteur::MASTER, TypeCommande::ORDRE, Action::PAUSE, 0.0f);
    } else {
        if (modeleLecteur.estTermine()) {
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

void C_LecteurPhysiqueLocal::modifierVitesse(const float vitesse) {
    modeleLecteur.setVitesse(vitesse);
    udp.transmettreCommande(Expediteur::MASTER, TypeCommande::ORDRE, Action::VITESSE, vitesse);
}

void C_LecteurPhysiqueLocal::mettreAJour() {
    if (videoGeneree) {
        modeleLecteur.lireVideo(CHEMIN_VIDEO);
        videoGeneree = false;
    }
}

void C_LecteurPhysiqueLocal::stopper() {
    modeleLecteur.stop();
    udp.transmettreCommande(Expediteur::MASTER, TypeCommande::ORDRE, Action::STOP, 0.0f);
}