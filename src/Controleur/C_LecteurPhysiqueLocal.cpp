#include "Controleur/C_LecteurPhysiqueLocal.h"
#include "Modele/M_ReceveurUDP.h"

#include <filesystem>
#include <iostream>

using namespace std;

C_LecteurPhysiqueLocal::C_LecteurPhysiqueLocal(const string &ipMulticast, const int portCommandes, const int portDecouverte,
                                               const int portReponse, const vector<vector<string> > &configLecteurs)
    : udp(ipMulticast, portCommandes), configLecteurs(configLecteurs), m_adresseMulticast(ipMulticast),
      m_portDecouverte(portDecouverte), m_portReponse(portReponse) {
    modeleLecteur.collecterInfosLocales();
    demarrerEcouteMulticast();

    if (filesystem::exists(CHEMIN_VIDEO)) {
        modeleLecteur.lireVideo(CHEMIN_VIDEO);
    }
}

C_LecteurPhysiqueLocal::~C_LecteurPhysiqueLocal() {
    arreterEcouteMulticast(); // Coupe le thread réseau proprement

    if (threadGeneration.joinable()) threadGeneration.join();
    if (threadRecherche.joinable()) threadRecherche.join();
    if (threadEcouteMulticast.joinable()) threadEcouteMulticast.join();
}

// Fonction d'origine restaurée
void C_LecteurPhysiqueLocal::arreterEcouteMulticast() {
    ecouteMulticastActive = false;
}

void C_LecteurPhysiqueLocal::demarrerEcouteMulticast() {
    ecouteMulticastActive = true;

    threadEcouteMulticast = thread([this]() {
        // On instancie notre nouveau composant réseau de manière locale au thread
        M_ReceveurUDP receveurMulticast(m_portDecouverte, m_adresseMulticast);

        char buffer[512];
        string ipEmetteur;

        while (ecouteMulticastActive) {
            // On attend jusqu'à 500ms. Le thread dort et ne consomme pas de CPU.
            int nbOctets = receveurMulticast.recevoirAvecTimeout(buffer, sizeof(buffer) - 1, ipEmetteur, 500);

            if (nbOctets > 0) {
                // Si on a reçu quelque chose, on prépare notre réponse JSON
                string jsonInfos = modeleLecteur.versJson();

                // On utilise la nouvelle méthode pour répondre sans polluer le contrôleur avec des sockets
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
            // CORRECTION : On passe bien les paramètres injectés à la session
            cacheLecteurs = session.rechercherLecteursComplets(m_adresseMulticast, m_portDecouverte, m_portReponse);
            resultatsRecherchePrets = true;
        } catch (const exception &e) {
            cerr << "Erreur Recherche Réseau: " << e.what() << '\n';
        }
        rechercheEnCours = false;
    });
}

vector<map<string, string> > C_LecteurPhysiqueLocal::getDerniersLecteursTrouves() {
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
            if (!filesystem::exists("videosComplexes")) filesystem::create_directory("videosComplexes");

            size_t nbLecteurs = configLecteurs.size();
            LecteurSpec *specs = new LecteurSpec[nbLecteurs];

            for (size_t i = 0; i < nbLecteurs; ++i) {
                if (configLecteurs[i].size() >= 3) {
                    specs[i].id = configLecteurs[i][0];
                    specs[i].ip = configLecteurs[i][1];
                    specs[i].nbVideos = configLecteurs[i][2];
                }
            }

            session.preparerSessionLecture(specs, nbLecteurs);
            delete[] specs;

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
    const bool enLecture = modeleLecteur.estEnLecture();
    if (enLecture) {
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

void C_LecteurPhysiqueLocal::modifierProgression(const float progression, const bool enGlissement,
                                                 const bool restaurerLecture) {
    modeleLecteur.setTime(progression);
    udp.transmettreCommande(Expediteur::MASTER, TypeCommande::ORDRE, Action::PROGRESSION, progression);

    if (enGlissement) {
        modeleLecteur.pause();
        modeleLecteur.setVolume(0);
    } else {
        modeleLecteur.setVolume(static_cast<int>(volumeCourant));
        if (restaurerLecture) modeleLecteur.play();
        else modeleLecteur.pause();
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