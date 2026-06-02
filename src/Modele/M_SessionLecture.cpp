#include "Modele/M_SessionLecture.h"
#ifdef _WIN32
#include "Modele/M_TFTP.h"
#endif
#include <raylib.h>
#include <filesystem>
#include <iostream>
#include <map>
#include <iomanip>
#include <algorithm>

using namespace std;

M_SessionLecture::M_SessionLecture(const string &ip, int port) : config(ip, port) {}

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

    for (auto& lecteur : m_lecteurs) {
        lecteur.nbVideosCapacite = 0;
    }

    int videosRestantes = nombreTotalVideos - 1;

    // --- PALIER 1 : Garantie anti-ecran noir pour les clients distants (1 a N-1) ---
    for (size_t i = 1; i < m_lecteurs.size() && videosRestantes > 0; ++i) {
        m_lecteurs[i].nbVideosCapacite++;
        videosRestantes--;
    }

    // --- PALIER 2 : Garantie d'au moins une video secondaire pour le Master (0) ---
    if (videosRestantes > 0) {
        m_lecteurs[0].nbVideosCapacite++;
        videosRestantes--;
    }

    // --- TRAITEMENT VIA LE CACHE LOCALISÉ ---
    map<string, long long> surfaceParIP;

    for (const auto& ligne : m_cacheConfigReseau) {
        if (ligne.size() >= 5) {
            // Index basés sur le SELECT * (0: mac, 1: ip, 2: os, 3: largeur, 4: hauteur)
            long long surface = stoll(ligne[3]) * stoll(ligne[4]);
            if (surface > 0) surfaceParIP[ligne[1]] = surface;
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
    surfaceParIP[""] = surfaceMaster;

    // Calcul de la surface globale et application des résolutions par défaut si manquantes
    long long surfaceTotaleGlobale = 0;
    for (const auto& lecteur : m_lecteurs) {
        if (!surfaceParIP.contains(lecteur.ip)) {
            surfaceParIP[lecteur.ip] = 1920LL * 1080LL;
            cout << "[DEBUG] [Session Lecture] Dimensions non trouvees pour le lecteur " << lecteur.ip
                 << ". Application de la resolution par defaut (1920x1080)." << endl;
        }
        surfaceTotaleGlobale += surfaceParIP[lecteur.ip];
    }

    // --- PALIER 3 : Distribution du reste au prorata exact ---
    if (videosRestantes > 0 && surfaceTotaleGlobale > 0) {
        int aDistribuerAuProrata = videosRestantes;
        for (auto& lecteur : m_lecteurs) {
            double ratio = static_cast<double>(surfaceParIP[lecteur.ip]) / surfaceTotaleGlobale;
            int supplement = static_cast<int>(ratio * aDistribuerAuProrata);
            lecteur.nbVideosCapacite += supplement;
            videosRestantes -= supplement;
        }
    }

    // --- PALIER 4 : Lissage des résidus (Round-Robin) avec priorité aux clients ---
    if (videosRestantes > 0) {
        vector<size_t> ordrePriorite;
        ordrePriorite.reserve(m_lecteurs.size());

        for (size_t i = 1; i < m_lecteurs.size(); ++i) ordrePriorite.push_back(i);
        ordrePriorite.push_back(0);

        size_t pointeurInterne = 0;
        while (videosRestantes > 0) {
            m_lecteurs[ordrePriorite[pointeurInterne]].nbVideosCapacite++;
            videosRestantes--;
            pointeurInterne = (pointeurInterne + 1) % ordrePriorite.size();
        }
    }

    // --- ETAPE FINALE : Injection de la vidéo de référence (+1 obligatoire) ---
    for (auto& lecteur : m_lecteurs) {
        lecteur.nbVideosCapacite += 1;
    }

    // Affichage du rapport de débug
    for (const auto& lecteur : m_lecteurs) {
        string nomLecteur = (lecteur.id == 0) ? "Master (0)" : "Client (" + to_string(lecteur.id) + ")";
        string adresseIP  = (lecteur.id == 0) ? " " : lecteur.ip;
        string surfacePx  = to_string(surfaceParIP[lecteur.ip]) + " px";

        cout << "[DEBUG] [Session Lecture] "
             << left << setw(20) << nomLecteur
             << setw(20) << adresseIP
             << setw(20) << surfacePx
             << setw(20) << lecteur.nbVideosCapacite << endl;
    }
    cout << endl << "[DEBUG] [Session Lecture] Fin du calcul des capacites." << endl;
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
    m_lecteurs.push_back({.id = 0, .ip = "", .nbVideosCapacite = 0});

    int prochainId = 1;
    for (const string& ip : ipsSelectionnees) {
        if (ip.empty()) continue;

        bool existeDeja = std::any_of(m_lecteurs.begin(), m_lecteurs.end(),
                                      [&ip](const auto& l) { return l.ip == ip; });

        if (!existeDeja) {
            m_lecteurs.push_back({.id = prochainId++, .ip = ip, .nbVideosCapacite = 0});
            cout << "[DEBUG] [Session Lecture] Ajout du Lecteur Client - ID: " << prochainId - 1 << ", IP: " << ip << endl;
        }
    }

    calculerCapacitesVideo(static_cast<int>(listeFichiersEntree.size()));

    vector<vector<string>> videosParLecteur(m_lecteurs.size());

    for (size_t i = 0; i < m_lecteurs.size(); ++i) {
        if (m_lecteurs[i].nbVideosCapacite > 0) {
            videosParLecteur[i].push_back(listeFichiersEntree[0]);
        }
    }

    size_t indexVideoSource = 1;
    size_t lecteurActuel = (m_lecteurs.size() > 1) ? 1 : 0;

    while (indexVideoSource < listeFichiersEntree.size()) {
        if (static_cast<int>(videosParLecteur[lecteurActuel].size()) < m_lecteurs[lecteurActuel].nbVideosCapacite) {
            videosParLecteur[lecteurActuel].push_back(listeFichiersEntree[indexVideoSource++]);
        }
        lecteurActuel = (lecteurActuel + 1) % m_lecteurs.size();
    }

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

    struct Transfert {
        string ip;
        string fichier;
    };

    vector<Transfert> listeTransferts;
    listeTransferts.reserve(m_lecteurs.size());

    for (const auto& lecteur : m_lecteurs) {
        if (lecteur.id != 0 && lecteur.nbVideosCapacite > 0) {
            listeTransferts.push_back({lecteur.ip, dossierSource + "/VideoComplexe_" + to_string(lecteur.id) + ".mp4"});
        }
    }

    if (listeTransferts.empty()) {
        cout << "[DEBUG] [Session Lecture] Aucun fichier a uploader (pas de clients distants configures)." << endl;
        return;
    }

#ifdef _WIN32
    try {
        std::vector<std::thread> listeThreads;
        listeThreads.reserve(listeTransferts.size());

        for (const auto& [ip, fichier] : listeTransferts) {
            listeThreads.push_back(std::thread([ip, fichier]() {
                try {
                    M_TFTP tftp;
                    tftp.envoyer(ip, fichier);
                } catch (const std::exception &e) {
                    std::cerr << "[DEBUG] [Session Lecture] [THREAD ERROR] Echec pour "
                              << ip << " (" << fichier << ") : " << e.what() << std::endl;
                }
            }));
        }

        cout << "[DEBUG] [Session Lecture] " << listeThreads.size()
             << " transferts TFTP lances en simultane. Attente de la fin des envois..." << endl;

        for (auto& unThread : listeThreads) {
            if (unThread.joinable()) {
                unThread.join();
            }
        }

        cout << "[DEBUG] [Session Lecture] Tous les transferts multithreades sont termines avec succes." << endl;

    } catch (const std::exception &e) {
        cerr << "[DEBUG] [Session Lecture] [SESSION TFTP ERROR] Erreur generale dans le gestionnaire : " << e.what() << endl;
    }
#else
    cout << "[DEBUG] [Session Lecture] Transfert TFTP ignore (Disponible uniquement sous Windows)." << endl;
#endif
}

vector<map<string, string>> M_SessionLecture::rechercherLecteurs() {
    cout << "[DEBUG] [Session Lecture] Recherche des lecteurs physiques sur le reseau..." << endl;

    config.rechercherLecteurPhysique("JSON_recue");

    // MISE À JOUR DU CACHE AVEC LA DECOUVERTE RESEAU DYNAMIQUE
    m_cacheConfigReseau = config.getConfigReseau();

    const auto rawConfig = config.getConfigReseau();
    vector<map<string, string>> lecteursDetectes;
    lecteursDetectes.reserve(rawConfig.size());

    const vector<string> colonnes = {"mac", "ip", "os", "ecran_largeur", "ecran_hauteur"};

    for (const auto& ligne : rawConfig) {
        if (ligne.empty()) continue;

        map<string, string> lecteur;
        for (size_t i = 0; i < ligne.size() && i < colonnes.size(); ++i) {
            lecteur[colonnes[i]] = ligne[i];
        }

        lecteursDetectes.push_back(std::move(lecteur));
    }

    cout << "[DEBUG] [Session Lecture] Fin de la recherche. " << lecteursDetectes.size() << " lecteur(s) stocke(s)." << endl;
    return lecteursDetectes;
}