#include "Modele/M_SessionLecture.h"
#ifdef _WIN32
#include "Modele/M_TFTP_W.h"
#endif
#include <raylib.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <cmath>
#include <map>
#include <iomanip>

using namespace std;

M_SessionLecture::M_SessionLecture(const string &ip, int port) : config(&bdd, ip, port) {}

void M_SessionLecture::configurerLecteurs(const vector<LecteurConfig>& configs) {
    cout << "[DEBUG] [Session Lecture] Configuration manuelle de " << configs.size() << " lecteur(s)." << endl;
    m_lecteurs = configs;
}

void M_SessionLecture::calculerCapacitesVideo(int nombreTotalVideos) {
    cout << "[DEBUG] [Session Lecture] Debut du calcul des capacites video pour " << nombreTotalVideos << " video(s)." << endl;

    if (m_lecteurs.empty() || nombreTotalVideos <= 0) {
        cout << "[DEBUG] [Session Lecture] Annulation : Aucun lecteur ou aucune video a traiter." << endl;
        return;
    }

    if (m_lecteurs.size() == 1 && m_lecteurs[0].id == 0) {
        m_lecteurs[0].nbVideosCapacite = nombreTotalVideos;
        cout << "[DEBUG] [Session Lecture] Un seul lecteur maitre detecte. Attribution de la totalite ("
             << nombreTotalVideos << " videos)." << endl;
        return;
    }

    // Initialisation de toutes les capacités de vidéos secondaires à 0
    for (size_t i = 0; i < m_lecteurs.size(); ++i) {
        m_lecteurs[i].nbVideosCapacite = 0;
    }

    // Nombre de vidéos secondaires à distribuer (la vidéo de référence index 0 est à part)
    int videosRestantes = nombreTotalVideos - 1;

    // --- PALIER 1 : Garantie anti-ecran noir pour les clients distants (Index 1 a N-1) ---
    for (size_t i = 1; i < m_lecteurs.size() && videosRestantes > 0; ++i) {
        m_lecteurs[i].nbVideosCapacite++;
        videosRestantes--;
    }

    // --- PALIER 2 : Garantie d'au moins une video secondaire pour le Master (Index 0) ---
    if (videosRestantes > 0) {
        m_lecteurs[0].nbVideosCapacite++;
        videosRestantes--;
    }

    // Récupération des surfaces d'affichage depuis la BDD
    vector<vector<string>> dataBDD = bdd.recupereDonnees("adresse_ip, ecran_largeur, ecran_hauteur", "config_reseau", "");
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

    // Capture dynamique de la surface de l'écran du Master via Raylib
    long long surfaceMaster = 1920LL * 1080LL;
    if (IsWindowReady()) {
        int largeurMaster = GetMonitorWidth(0);
        int hauteurMaster = GetMonitorHeight(0);
        if (largeurMaster > 0 && hauteurMaster > 0) {
            surfaceMaster = static_cast<long long>(largeurMaster) * static_cast<long long>(hauteurMaster);
        }
    }
    surfaceParIP[""] = surfaceMaster; // L'IP du master est vide "" dans la configuration

    // Calcul de la surface totale de TOUS les lecteurs configurés
    long long surfaceTotaleGlobale = 0;
    for (size_t i = 0; i < m_lecteurs.size(); ++i) {
        if (!surfaceParIP.contains(m_lecteurs[i].ip)) {
            surfaceParIP[m_lecteurs[i].ip] = 1920LL * 1080LL;
            cout << "[DEBUG] [Session Lecture] Dimensions non trouvees pour le lecteur " << m_lecteurs[i].ip
                 << ". Application de la resolution par defaut (1920x1080)." << endl;
        }
        surfaceTotaleGlobale += surfaceParIP[m_lecteurs[i].ip];
    }

    // --- PALIER 3 : Distribution du reste au prorata exact pour TOUT LE MONDE ---
    if (videosRestantes > 0 && surfaceTotaleGlobale > 0) {
        int aDistribuerAuProrata = videosRestantes;
        for (size_t i = 0; i < m_lecteurs.size(); ++i) {
            double ratio = static_cast<double>(surfaceParIP[m_lecteurs[i].ip]) / surfaceTotaleGlobale;
            int supplement = static_cast<int>(ratio * aDistribuerAuProrata);
            m_lecteurs[i].nbVideosCapacite += supplement;
            videosRestantes -= supplement;
        }
    }

    // --- PALIER 4 : Lissage des résidus (Round-Robin) avec priorité aux clients distants ---
    if (videosRestantes > 0) {
        vector<size_t> ordrePriorite;
        // On enregistre d'abord les clients distants (1 à N-1)
        for (size_t i = 1; i < m_lecteurs.size(); ++i) {
            ordrePriorite.push_back(i);
        }
        // On ajoute le Master (0) en toute fin de liste de priorité
        ordrePriorite.push_back(0);

        size_t pointeurInterne = 0;
        while (videosRestantes > 0) {
            size_t idxLecteur = ordrePriorite[pointeurInterne];
            m_lecteurs[idxLecteur].nbVideosCapacite++;
            videosRestantes--;
            pointeurInterne = (pointeurInterne + 1) % ordrePriorite.size();
        }
    }

    // --- ETAPE FINAL : Injection de la vidéo de référence (+1 obligatoire pour tous) ---
    for (size_t i = 0; i < m_lecteurs.size(); ++i) {
        m_lecteurs[i].nbVideosCapacite += 1;
    }

    cout << endl;

    for (const auto& lecteur : m_lecteurs) {
        string nomLecteur = (lecteur.id == 0) ? "Master (0)" : "Client (" + to_string(lecteur.id) + ")";
        string adresseIP  = (lecteur.id == 0) ? " " : lecteur.ip;
        string surfacePx  = to_string(surfaceParIP[lecteur.ip]) + " px";

        cout << "[DEBUG] [Session Lecture] "
             << left << setw(17)  << nomLecteur
             << setw(17) << adresseIP
             << setw(17) << surfacePx
             << setw(17) << lecteur.nbVideosCapacite << endl;
    }

    cout << endl;

    cout << "[DEBUG] [Session Lecture] Fin du calcul des capacites." << endl;
}

void M_SessionLecture::genererVideoComplexe(const vector<string>& listeFichiersEntree, const string& dossierSortie, const vector<string>& ipsSelectionnees) {
    cout << "[DEBUG] [Session Lecture] Debut de la generation des videos complexes..." << endl;

    if (listeFichiersEntree.empty()) {
        cerr << "[DEBUG] [Session Lecture] [ERROR] La liste des fichiers d'entree est vide. Abandon." << endl;
        return;
    }

    try {
        if (filesystem::exists("videosComplexes")) {
            cout << "[DEBUG] [Session Lecture] Nettoyage complet du dossier : videosComplexes" << endl;
            for (const auto& element : filesystem::directory_iterator("videosComplexes")) {
                filesystem::remove_all(element.path());
            }
        } else {
            filesystem::create_directories("videosComplexes");
        }
    } catch (const exception& e) {
        cerr << "[DEBUG] [Session Lecture] [WARNING] Impossible de vider 'videosComplexes' : " << e.what() << endl;
    }

    m_lecteurs.clear();

    LecteurConfig master;
    master.id = 0;
    master.ip = "";
    master.nbVideosCapacite = 0;
    m_lecteurs.push_back(master);

    int prochainId = 1;
    for (const string& ip : ipsSelectionnees) {
        if (!ip.empty()) {
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
                cout << "[DEBUG] [Session Lecture] Ajout du Lecteur Client - ID: " << client.id << ", IP: " << client.ip << endl;
            }
        }
    }

    calculerCapacitesVideo(static_cast<int>(listeFichiersEntree.size()));

    vector<vector<string>> videosParLecteur(m_lecteurs.size());

    // Injection obligatoire de la video de reference (index 0) en tete de chaque liste
    for (size_t i = 0; i < m_lecteurs.size(); ++i) {
        if (m_lecteurs[i].nbVideosCapacite > 0) {
            videosParLecteur[i].push_back(listeFichiersEntree[0]);
        }
    }

    // Distribution entrelacee en privilegiant également le depart sur les lecteurs distants (index 1)
    size_t indexVideoSource = 1;
    size_t lecteurActuel = (m_lecteurs.size() > 1) ? 1 : 0;

    while (indexVideoSource < listeFichiersEntree.size()) {
        if (static_cast<int>(videosParLecteur[lecteurActuel].size()) < m_lecteurs[lecteurActuel].nbVideosCapacite) {
            videosParLecteur[lecteurActuel].push_back(listeFichiersEntree[indexVideoSource]);
            indexVideoSource++;
        }

        lecteurActuel++;
        if (lecteurActuel >= m_lecteurs.size()) {
            lecteurActuel = 0;
        }
    }

    // Traitement séquentiel optimisé (Chaque FFmpeg prend 100% du CPU l'un après l'autre)
    for (size_t i = 0; i < m_lecteurs.size(); ++i) {
        if (!videosParLecteur[i].empty()) {
            string cheminSortie = "videosComplexes/VideoComplexe_" + to_string(m_lecteurs[i].id) + ".mp4";
            bool masquerRef = (m_lecteurs[i].id != 0);

            cout << "[DEBUG] [Session Lecture] Lancement du traitement pour le Lecteur ID " << m_lecteurs[i].id
                 << " avec " << videosParLecteur[i].size() << " morceaux." << endl;

            instanceVideoComplexe.genererVideoComplexe(videosParLecteur[i].data(), videosParLecteur[i].size(), cheminSortie, masquerRef, m_lecteurs[i].id);
        }
    }

    cout << "[DEBUG] [Session Lecture] Fin de la generation des videos complexes." << endl;
}

void M_SessionLecture::uploaderVideoComplexe(const string& dossierSource) const {
    cout << "[DEBUG] [Session Lecture] Preparation de l'upload TFTP des videos complexes..." << endl;
    vector<pair<string, string>> listeTransferts;

    for (const auto& lecteur : m_lecteurs) {
        if (lecteur.id == 0) continue;
        if (lecteur.nbVideosCapacite > 0) {
            listeTransferts.push_back({lecteur.ip, "VideoComplexe_" + to_string(lecteur.id) + ".mp4"});
        }
    }

    if (listeTransferts.empty()) {
        cout << "[DEBUG] [Session Lecture] Aucun fichier a uploader (pas de clients distants configures)." << endl;
        return;
    }

#ifdef _WIN32
    try {
        M_TFTP_W tftp;
        for (const auto& [ip, fichier] : listeTransferts) {
            string cheminFichier = "videosComplexes/" + fichier;
            tftp.envoyer(ip, cheminFichier);
        }
    } catch (const exception &e) {
        cerr << "[DEBUG] [Session Lecture] [SESSION TFTP ERROR] Echec lors du transfert : " << e.what() << endl;
    }
#else
    cout << "[DEBUG] [Session Lecture] Transfert TFTP ignore (Disponible uniquement sous Windows)." << endl;
#endif
}

vector<map<string, string>> M_SessionLecture::rechercherLecteurs() {
    cout << "[DEBUG] [Session Lecture] Recherche des lecteurs physiques sur le reseau..." << endl;

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
            cout << "[DEBUG] [Session Lecture] Lecteur trouve -> ID: " << lecteur["id"] << " | IP: " << lecteur["ip"] << " | MAC: " << lecteur["mac"] << endl;
            lecteursDetectes.push_back(lecteur);
        }
    }

    cout << "[DEBUG] [Session Lecture] Fin de la recherche. " << lecteursDetectes.size() << " lecteur(s) stocke(s)." << endl;
    return lecteursDetectes;
}