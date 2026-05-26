#include "Modele/M_SessionLecture.h"
#include "Modele/M_TFTP_W.h"
#include "Modele/M_JsonUtil.h"
#include <fstream>
#include <iostream>

using namespace std;

M_SessionLecture::M_SessionLecture(const string &ip, int port) : config(&bdd, ip, port) {}

void M_SessionLecture::configurerLecteurs(const vector<LecteurConfig>& configs) {
    m_lecteurs = configs;
}

void M_SessionLecture::genererVideoComplexe(const vector<string>& listeFichiersEntree, const string& dossierSortie) {
    if (listeFichiersEntree.empty() || m_lecteurs.empty()) return;

    // Matrice propre avec vector au lieu de pointeurs de pointeurs bruts
    vector<vector<string>> videosParLecteur(m_lecteurs.size());
    int placesDisponibles = 0;

    for (size_t i = 0; i < m_lecteurs.size(); ++i) {
        if (m_lecteurs[i].nbVideosCapacite > 0) {
            videosParLecteur[i].push_back(listeFichiersEntree[0]); // Vidéo maître / fond
        }
        placesDisponibles += (m_lecteurs[i].nbVideosCapacite - static_cast<int>(videosParLecteur[i].size()));
    }

    size_t indexVideo = 1;
    size_t lecteurActuel = 0;

    while (indexVideo < listeFichiersEntree.size() && placesDisponibles > 0) {
        if (static_cast<int>(videosParLecteur[lecteurActuel].size()) < m_lecteurs[lecteurActuel].nbVideosCapacite) {
            videosParLecteur[lecteurActuel].push_back(listeFichiersEntree[indexVideo]);
            indexVideo++;
            placesDisponibles--;
        }
        lecteurActuel = (lecteurActuel + 1) % m_lecteurs.size(); // Round-Robin simplifié
    }

    // Génération effective
    for (size_t i = 0; i < m_lecteurs.size(); ++i) {
        if (!videosParLecteur[i].empty()) {
            string cheminSortie = dossierSortie + "/VideoComplexe_" + to_string(m_lecteurs[i].id) + ".mp4";
            instanceVideoComplexe.genererVideoComplexe(videosParLecteur[i].data(), videosParLecteur[i].size(), cheminSortie);
        }
    }
}

void M_SessionLecture::uploaderVideoComplexe(const string& dossierSource) const {
    vector<pair<string, string>> listeTransferts; // pair<IP, NomFichier>

    for (const auto& lecteur : m_lecteurs) {
        if (lecteur.id == 0) continue; // Ignorer le master local
        if (lecteur.nbVideosCapacite > 0) {
            listeTransferts.push_back({lecteur.ip, "VideoComplexe_" + to_string(lecteur.id) + ".mp4"});
        }
    }

    if (listeTransferts.empty()) return;

#ifdef _WIN32
    try {
        M_TFTP_W tftp;
        for (const auto& [ip, fichier] : listeTransferts) {
            tftp.envoyer(ip, dossierSource + "/" + fichier);
        }
    } catch (const exception &e) {
        cerr << "[SESSION TFTP] Erreur: " << e.what() << endl;
    }
#else
    cout << "[SESSION] Transfert TFTP ignoré sur Linux (non supporté de base)." << endl;
#endif
}

// Implémentation mise à jour de rechercherLecteurs
vector<map<string, string>> M_SessionLecture::rechercherLecteurs() {
    config.rechercherLecteurPhysique("JSON_recue"); // Récupère et stocke les données de la BDD

    config.visualiserLecteurPhysique();

    vector<vector<string>> rawConfig = config.getConfigReseau();
    vector<map<string, string>> lecteursDetectes;

    // Les noms de colonnes doivent être en minuscules pour correspondre aux accès type .at("ip")
    const vector<string> colonnes = {"id", "ip", "mac", "nb_videos"};

    for (const auto& ligne : rawConfig) {
        map<string, string> lecteur;
        for (size_t i = 0; i < ligne.size() && i < colonnes.size(); ++i) {
            lecteur[colonnes[i]] = ligne[i];
        }
        if (!lecteur.empty()) {
            lecteursDetectes.push_back(lecteur);
        }
    }

    return lecteursDetectes;
}
