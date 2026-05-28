#include "Controleur/C_LecteurPhysiqueLocal.h"
#include <filesystem>
#include <iostream>
#include <map>

using namespace std;

C_LecteurPhysiqueLocal::C_LecteurPhysiqueLocal(const string &ipMulticast, int portCommandes, int portDecouverte,
                                               int portReponse, const string &cheminVideoMaster)
    : m_cheminVideoMaster(cheminVideoMaster), udp(ipMulticast, portCommandes), session(ipMulticast, portDecouverte),
      m_adresseMulticast(ipMulticast), m_portDecouverte(portDecouverte), m_portReponse(portReponse) {
    modeleLecteur.collecterInfosLocales();

    if (filesystem::exists(m_cheminVideoMaster)) {
        modeleLecteur.lireVideo(m_cheminVideoMaster);
    }
}

C_LecteurPhysiqueLocal::~C_LecteurPhysiqueLocal() {
    if (threadGeneration.joinable()) threadGeneration.join();
    if (threadRecherche.joinable()) threadRecherche.join();
}

void C_LecteurPhysiqueLocal::lancerRechercheLecteurs() {
    if (rechercheEnCours) return;
    rechercheEnCours = true;
    resultatsRecherchePrets = false;

    if (threadRecherche.joinable()) threadRecherche.join();

    threadRecherche = thread([this]() {
        map<string, string> localInfos = {
            {"ip", modeleLecteur.getIp()},
            {"mac", modeleLecteur.getMac()},
            {"os", modeleLecteur.getOs()},
            {"largeurEcran", to_string(modeleLecteur.getLargeurEcran())},
            {"hauteurEcran", to_string(modeleLecteur.getHauteurEcran())},
        };

        cacheLecteurs.clear();
        cacheLecteurs.push_back(localInfos);

        vector<map<string, string>> lecteursReseau = session.rechercherLecteurs();
        cacheLecteurs.insert(cacheLecteurs.end(), lecteursReseau.begin(), lecteursReseau.end());

        resultatsRecherchePrets = true;
        rechercheEnCours = false;
    });
}

vector<map<string, string>> C_LecteurPhysiqueLocal::getDerniersLecteursTrouves() {
    resultatsRecherchePrets = false;
    return cacheLecteurs;
}

void C_LecteurPhysiqueLocal::initialiserSession(const vector<string> &fichiers, const vector<string> &lecteursSelectionnes) {
    if (generationEnCours) return;

    modeleLecteur.pause();
    generationEnCours = true;

    if (threadGeneration.joinable()) threadGeneration.join();

    threadGeneration = thread([this, fichiers, lecteursSelectionnes]() {
        try {
            if (!filesystem::exists(m_dossierSortie)) {
                filesystem::create_directory(m_dossierSortie);
            }

            session.genererVideoComplexe(fichiers, m_dossierSortie, lecteursSelectionnes);

            transfertEnCours = true;
            session.uploaderVideoComplexe(m_dossierSortie);
            transfertEnCours = false;

            videoGeneree = true;
        } catch (const exception &e) {
            cerr << "[CONTROLEUR] Erreur de generation: " << e.what() << '\n';
        }
        generationEnCours = false;
    });
}

void C_LecteurPhysiqueLocal::mettreAJour() {
    if (videoGeneree) {
        modeleLecteur.lireVideo(m_cheminVideoMaster);
        videoGeneree = false;
    }
}

void C_LecteurPhysiqueLocal::basculerPlayPause() {
    if (modeleLecteur.estEnLecture()) {
        modeleLecteur.pause();
        udp.transmettreCommande(Expediteur::MASTER, TypeCommande::ORDRE, Action::PAUSE, 0.0f);
    } else {
        if (modeleLecteur.estTermine()) {
            modeleLecteur.stop();
            modeleLecteur.demarrer();
        } else {
            modeleLecteur.play();
            udp.transmettreCommande(Expediteur::MASTER, TypeCommande::ORDRE, Action::PLAY, 0.0f);
        }
    }
}

void C_LecteurPhysiqueLocal::modifierVolume(float volume, bool muet) {
    volumeCourant = muet ? 0.0f : volume;
    modeleLecteur.setVolume(static_cast<int>(volumeCourant));
    udp.transmettreCommande(Expediteur::MASTER, TypeCommande::ORDRE, Action::VOLUME, volumeCourant);
}

void C_LecteurPhysiqueLocal::modifierProgression(float progression, bool enGlissement, bool restaurerLecture) {
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

void C_LecteurPhysiqueLocal::modifierVitesse(float vitesse) {
    modeleLecteur.setVitesse(vitesse);
    udp.transmettreCommande(Expediteur::MASTER, TypeCommande::ORDRE, Action::VITESSE, vitesse);
}

void C_LecteurPhysiqueLocal::stopper() {
    modeleLecteur.stop();
    udp.transmettreCommande(Expediteur::MASTER, TypeCommande::ORDRE, Action::STOP, 0.0f);
}