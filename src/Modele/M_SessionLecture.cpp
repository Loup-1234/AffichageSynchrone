#include "Modele/M_SessionLecture.h"
#include "Modele/M_TFTP_W.h"
#include <raylib.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <cmath>
#include <map>

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

    // Allocation initiale : 1 slot reserve d'office pour la video de reference (index 0)
    m_lecteurs[0].nbVideosCapacite = 1;

    int videosAPartager = nombreTotalVideos - 1;
    int nombreTotalLecteurs = static_cast<int>(m_lecteurs.size());

    // Etape 1 : Garantie anti-ecran noir (on attribue au moins 1 video secondaire par machine si possible)
    int minSecParLecteur = (videosAPartager >= nombreTotalLecteurs) ? 1 : 0;
    for (size_t i = 0; i < m_lecteurs.size(); ++i) {
        m_lecteurs[i].nbVideosCapacite = minSecParLecteur;
    }

    int totalAttribue = minSecParLecteur * nombreTotalLecteurs;
    int videosRestantes = videosAPartager - totalAttribue;

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

    // Capture dynamique de l'ecran du Master via l'affichage physique de Raylib (sans BDD)
    long long surfaceMaster = 1920LL * 1080LL;
    if (IsWindowReady()) {
        int largeurMaster = GetMonitorWidth(0);
        int hauteurMaster = GetMonitorHeight(0);

        if (largeurMaster > 0 && hauteurMaster > 0) {
            surfaceMaster = static_cast<long long>(largeurMaster) * static_cast<long long>(hauteurMaster);
        }
    } else {
        cout << "[DEBUG] [Session Lecture] Fenetre Raylib non prete. Taille Master par defaut (1920x1080)." << endl;
    }
    surfaceParIP[""] = surfaceMaster;

    long long surfaceTotaleGlobale = 0;
    for (size_t i = 0; i < m_lecteurs.size(); ++i) {
        if (surfaceParIP.find(m_lecteurs[i].ip) == surfaceParIP.end()) {
            surfaceParIP[m_lecteurs[i].ip] = 1920LL * 1080LL;
            cout << "[DEBUG] [Session Lecture] Dimensions non trouvees pour le client " << m_lecteurs[i].ip
                 << ". Application de la resolution par defaut (1920x1080)." << endl;
        }
        surfaceTotaleGlobale += surfaceParIP[m_lecteurs[i].ip];
    }

    // Etape 2 : Distribution mathematique au prorata exact des surfaces d'affichage
    if (videosRestantes > 0 && surfaceTotaleGlobale > 0) {
        for (size_t i = 0; i < m_lecteurs.size(); ++i) {
            double ratio = static_cast<double>(surfaceParIP[m_lecteurs[i].ip]) / surfaceTotaleGlobale;
            int supplement = static_cast<int>(ratio * videosRestantes);
            m_lecteurs[i].nbVideosCapacite += supplement;
            totalAttribue += supplement;
        }
    }

    // Etape 3 : Lissage algorithmique (Round-Robin) pour epuiser le reste des videos lie aux arrondis
    size_t idxLissage = 0;
    while (totalAttribue < videosAPartager) {
        m_lecteurs[idxLissage].nbVideosCapacite++;
        totalAttribue++;
        idxLissage++;
        if (idxLissage >= m_lecteurs.size()) idxLissage = 0;
    }

    // Etape 4 : Integration de la video de reference obligatoire dans la capacite finale globale
    for (size_t i = 0; i < m_lecteurs.size(); ++i) {
        m_lecteurs[i].nbVideosCapacite += 1;
    }

    for (const auto& lecteur : m_lecteurs) {
        cout << "[DEBUG] [Session Lecture] Capacite finale fixee -> Lecteur ID " << lecteur.id
             << " (" << (lecteur.id == 0 ? "Master" : lecteur.ip) << ") : " << lecteur.nbVideosCapacite << " video(s)." << endl;
    }
    cout << "[DEBUG] [Session Lecture] Fin du calcul des capacites." << endl;
}

void M_SessionLecture::genererVideoComplexe(const vector<string>& listeFichiersEntree, const string& dossierSortie, const vector<string>& ipsSelectionnees) {
    cout << "[DEBUG] [Session Lecture] Debut de la generation des videos complexes..." << endl;

    if (listeFichiersEntree.empty()) {
        cerr << "[DEBUG] [Session Lecture] [ERROR] La liste des fichiers d'entree est vide. Abandon." << endl;
        return;
    }

    // --- LE NETTOYAGE DU DOSSIER CIBLE "videosComplexes" ---
    try {
        if (std::filesystem::exists("videosComplexes")) {
            cout << "[DEBUG] [Session Lecture] Nettoyage complet du dossier : videosComplexes" << endl;
            for (const auto& element : std::filesystem::directory_iterator("videosComplexes")) {
                std::filesystem::remove_all(element.path());
            }
        } else {
            std::filesystem::create_directories("videosComplexes");
        }
    } catch (const std::exception& e) {
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

    // Distribution entrelacee des fichiers secondaires pour equilibrer les contenus sur la grille
    size_t indexVideoSource = 1;
    size_t lecteurActuel = 0;

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

    // Traitement sequentiel (un par un) pour laisser 100% de la puissance CPU a chaque processus FFmpeg
    for (size_t i = 0; i < m_lecteurs.size(); ++i) {
        if (!videosParLecteur[i].empty()) {
            string cheminSortie = "videosComplexes/VideoComplexe_" + to_string(m_lecteurs[i].id) + ".mp4";

            // Regle de secret visuel : On cache le flux de reference uniquement sur les clients distants
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

    // Isolation des fichiers reseau (le Master genere localement, pas de transfert requis)
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

    // Transformation du tableau brut BDD en paires Clef/Valeur exploitables par l'IHM
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