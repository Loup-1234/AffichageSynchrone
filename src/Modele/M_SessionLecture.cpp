#include "Modele/M_SessionLecture.h"
#include "Modele/M_TFTP_W.h"
#include "Modele/M_JsonUtil.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <map>

using namespace std;

M_SessionLecture::M_SessionLecture(const string &ip, int port) : config(&bdd, ip, port) {}

void M_SessionLecture::configurerLecteurs(const vector<LecteurConfig>& configs) {
    m_lecteurs = configs;
}

void M_SessionLecture::calculerCapacitesVideo(int nombreTotalVideos) {
    if (m_lecteurs.empty() || nombreTotalVideos <= 0) return;

    if (m_lecteurs.size() == 1 && m_lecteurs[0].id == 0) {
        m_lecteurs[0].nbVideosCapacite = nombreTotalVideos;
        return;
    }

    for (auto& lecteur : m_lecteurs) {
        lecteur.nbVideosCapacite = 1;
    }

    vector<vector<string>> dataBDD = bdd.recupereDonnees("ip, ecran_largeur, ecran_hauteur", "config_reseau", "");
    map<string, long long> surfaceParIP;

    if (!dataBDD.empty()) {
        for (const auto& ligne : dataBDD) {
            if (ligne.size() >= 3) {
                string ipBDD = ligne[0];
                long long surface = stoll(ligne[1]) * stoll(ligne[2]);
                if (surface > 0) surfaceParIP[ipBDD] = surface;
            }
        }
    }

    long long surfaceTotaleGlobale = 0;
    for (auto& lecteur : m_lecteurs) {
        if (surfaceParIP.find(lecteur.ip) == surfaceParIP.end()) {
            surfaceParIP[lecteur.ip] = 1920LL * 1080LL;
        }
        surfaceTotaleGlobale += surfaceParIP[lecteur.ip];
    }

    int totalAttribue = 0;
    for (auto& lecteur : m_lecteurs) {
        if (surfaceTotaleGlobale > 0) {
            double ratio = static_cast<double>(surfaceParIP[lecteur.ip]) / surfaceTotaleGlobale;
            lecteur.nbVideosCapacite = max(1, static_cast<int>(ratio * nombreTotalVideos));
        } else {
            lecteur.nbVideosCapacite = 1;
        }
        totalAttribue += lecteur.nbVideosCapacite;
    }

    // Processus de lissage pour attribuer le reste des videos manquantes
    size_t idxArrondi = 0;
    while (totalAttribue < nombreTotalVideos) {
        m_lecteurs[idxArrondi].nbVideosCapacite++;
        totalAttribue++;
        idxArrondi = (idxArrondi + 1) % m_lecteurs.size();
    }
}

void M_SessionLecture::genererVideoComplexe(const vector<string>& listeFichiersEntree, const string& dossierSortie, const vector<string>& ipsSelectionnees) {
    if (listeFichiersEntree.empty()) return;

    m_lecteurs.clear();

    LecteurConfig master;
    master.id = 0;
    master.ip = "127.0.0.1";
    master.nbVideosCapacite = 0;
    m_lecteurs.push_back(master);

    int prochainId = 1;
    for (const string& ip : ipsSelectionnees) {
        if (ip != "127.0.0.1" && !ip.empty()) {
            bool existeDeja = false;
            for (const auto& l : m_lecteurs) {
                if (l.ip == ip) { existeDeja = true; break; }
            }
            if (!existeDeja) {
                LecteurConfig client;
                client.id = prochainId++;
                client.ip = ip;
                client.nbVideosCapacite = 0;
                m_lecteurs.push_back(client);
            }
        }
    }

    calculerCapacitesVideo(static_cast<int>(listeFichiersEntree.size()));

    vector<vector<string>> videosParLecteur(m_lecteurs.size());
    int placesDisponibles = 0;

    for (size_t i = 0; i < m_lecteurs.size(); ++i) {
        if (m_lecteurs[i].nbVideosCapacite > 0) {
            videosParLecteur[i].push_back(listeFichiersEntree[0]);
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
        lecteurActuel = (lecteurActuel + 1) % m_lecteurs.size();
    }

    for (size_t i = 0; i < m_lecteurs.size(); ++i) {
        if (!videosParLecteur[i].empty()) {
            string cheminSortie = dossierSortie + "/VideoComplexe_" + to_string(m_lecteurs[i].id) + ".mp4";
            instanceVideoComplexe.genererVideoComplexe(videosParLecteur[i].data(), videosParLecteur[i].size(), cheminSortie);
        }
    }
}

void M_SessionLecture::uploaderVideoComplexe(const string& dossierSource) const {
    vector<pair<string, string>> listeTransferts;

    for (const auto& lecteur : m_lecteurs) {
        if (lecteur.id == 0) continue;
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
        cerr << "[SESSION TFTP ERROR] " << e.what() << endl;
    }
#endif
}

vector<map<string, string>> M_SessionLecture::rechercherLecteurs() {
    config.rechercherLecteurPhysique("JSON_recue");
    config.visualiserLecteurPhysique();

    vector<vector<string>> rawConfig = config.getConfigReseau();
    vector<map<string, string>> lecteursDetectes;

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