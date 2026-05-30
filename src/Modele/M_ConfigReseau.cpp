#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>
#include <filesystem>

#include "Modele/M_ConfigReseau.h"

#ifdef _WIN32
#include "Modele/M_TFTP.h"
#endif

M_ConfigReseau::M_ConfigReseau(M_BDD* pMaBDD, const string &ip, int port) : maM_BDD(pMaBDD), configReseau({}) {
    expediteur.initialiserMulticast(ip, port);
}

M_ConfigReseau::~M_ConfigReseau() {
    expediteur.fermer();
}

void M_ConfigReseau::visualiserLecteurPhysique() {
    // Recuperation globale de la table cible stockee en base de donnees
    configReseau = maM_BDD->recupereDonnees("*", "config_reseau", "");
}

void M_ConfigReseau::enregistrerJson(string fichierJson) {
    ifstream fichier(fichierJson);

    if (!fichier.is_open()) {
        cerr << "[DEBUG] [Config Reseau] [ERROR] Impossible d'ouvrir le fichier : " << fichierJson << endl;
        return;
    }

    json listeValeurs;
    try {
        fichier >> listeValeurs;
    } catch (const json::parse_error& e) {
        cerr << "[DEBUG] [Config Reseau] [ERROR] Erreur de syntaxe JSON : " << e.what() << endl;
        return;
    }

    // Extraction des données en utilisant les clés réelles du JSON reçu
    string adress_mac = listeValeurs.value("mac", "00:00:00:00:00:00");
    string adress_ip  = listeValeurs.value("ip", "0.0.0.0");
    string os          = listeValeurs.value("os", "Inconnu");
    int ecran_largeur  = listeValeurs.value("largeurEcran", 0);
    int ecran_hauteur  = listeValeurs.value("hauteurEcran", 0);

    // Validation de sécurité optionnelle pour s'assurer que le JSON est complet
    if (!listeValeurs.contains("mac") || !listeValeurs.contains("ip")) {
        cerr << "[WARNING] Clés attendues ('mac' ou 'ip') manquantes dans le JSON reçu !" << endl;
        cerr << "Contenu réel du fichier : " << listeValeurs.dump(2) << endl;
    }

    // Préparation de la requête pour la base de données
    string colonnes = "adresse_mac, adresse_ip, os, ecran_largeur, ecran_hauteur";
    string valeurs = "'" + adress_mac + "', '" + adress_ip + "', '" + os + "', " + to_string(ecran_largeur) + ", " + to_string(ecran_hauteur);

    // Sauvegarde immédiate dans la table SQLite correspondante
    maM_BDD->enregistrerDonnees("config_reseau", colonnes, valeurs);
}

void M_ConfigReseau::enregistrerConfigurationReseau(string dossierJson) {
    namespace fs = std::filesystem;
    fs::path cheminConf = fs::path(dossierJson) / "conf.json";
    std::cerr << "Erreur " << dossierJson << std::endl;

    if (fs::exists(cheminConf) && fs::is_regular_file(cheminConf)) {
        enregistrerJson(cheminConf.string());
    } else {
        std::cerr << "Erreur : conf.json introuvable dans " << dossierJson << std::endl;
    }
}

void M_ConfigReseau::rechercherLecteurPhysique(string dossier) {
    namespace fs = std::filesystem;

    static bool serveurTftpLance = false;

    // 1. Initialisation et lancement du serveur TFTP Multi-Thread
#ifdef _WIN32
    if (!serveurTftpLance) {
        std::thread tftpThread([dossier]() {
            M_TFTP tftp;
            tftp.demarrerServeurMultiThread(dossier);
        });
        tftpThread.detach();

        serveurTftpLance = true; // Bloque les futures tentatives de bind
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
#endif

    // 2. Envoi de la commande spécifique d'initialisation réseau en Multicast
    expediteur.transmettreCommande(Expediteur::AUTRE, TypeCommande::CONNECTION, Action::PLAY, 0);
    cout << "[DEBUG] Trame multicast envoyee. Collecte des fichiers JSON en cours..." << endl;

    // 3. Fenêtre de capture : On attend quelques secondes que tous les lecteurs envoient leur JSON
    std::this_thread::sleep_for(std::chrono::seconds(4));

    // --- NOUVEAUTÉ : Réinitialisation de notre liste locale avant de stocker les résultats ---
    configReseau.clear();

    // 4. Traitement de TOUS les fichiers reçus dans le dossier
    bool auMoinsUnLecteurRecu = false;

    if (fs::exists(dossier) && fs::is_directory(dossier)) {
        // On parcourt le dossier à la recherche de fichiers comportant l'extension .json
        for (const auto& entry : fs::directory_iterator(dossier)) {
            if (entry.is_regular_file() && entry.path().extension() == ".json") {
                string cheminFichier = entry.path().string();
                cout << "[DEBUG] Nouveau lecteur detecte ! Traitement de : " << entry.path().filename().string() << endl;

                // --- LECTURE DIRECTE DU FICHIER JSON ---
                ifstream fichier(cheminFichier);
                if (fichier.is_open()) {
                    json listeValeurs;
                    try {
                        fichier >> listeValeurs;

                        // Extraction des données (identique à ton ancienne méthode)
                        string adress_mac = listeValeurs.value("mac", "00:00:00:00:00:00");
                        string adress_ip  = listeValeurs.value("ip", "0.0.0.0");
                        string os          = listeValeurs.value("os", "Inconnu");
                        int ecran_largeur  = listeValeurs.value("largeurEcran", 0);
                        int ecran_hauteur  = listeValeurs.value("hauteurEcran", 0);

                        // Création de la ligne représentant le lecteur sous forme de chaînes de caractères
                        vector<string> ligneLecteur = {
                            adress_mac,
                            adress_ip,
                            os,
                            to_string(ecran_largeur),
                            to_string(ecran_hauteur)
                        };

                        // Ajout de cette ligne directement dans notre vecteur 2D de configuration
                        configReseau.push_back(ligneLecteur);
                        auMoinsUnLecteurRecu = true;

                    } catch (const json::parse_error& e) {
                        cerr << "[DEBUG] [Config Reseau] [ERROR] Erreur de syntaxe JSON dans "
                             << entry.path().filename().string() << " : " << e.what() << endl;
                    }
                    fichier.close(); // Pense à bien refermer le flux de fichier
                }

                // Nettoyage : On supprime le fichier physique après traitement pour libérer l'espace
                fs::remove(entry.path());
            }
        }
    }

    // 5. Bilan de l'opération
    if (auMoinsUnLecteurRecu) {
        cout << "[DEBUG] Fin de la recherche. Tous les lecteurs detectes ont ete enregistres localement dans configReseau." << endl;
    } else {
        cerr << "[WARNING] Aucun lecteur physique n'a repondu a la commande multicast dans le temps imparti." << endl;
    }
}

void M_ConfigReseau::sauvegarderConfigActuelle() {
    if (configReseau.empty()) {
        cout << "[DEBUG] [Config Reseau] Rien à enregistrer, la liste est vide." << endl;
        return;
    }

    // 1. SUPPRIME LES DONNÉES EXISTANTES
    maM_BDD->supprimerDonnees("config_reseau", "");

    string colonnes = "adresse_mac, adresse_ip, os, ecran_largeur, ecran_hauteur";

    // 2. ENREGISTRE LES NOUVELLES DONNÉES
    for (const auto& lecteur : configReseau) {
        string adress_mac     = lecteur[0];
        string adress_ip      = lecteur[1];
        string os             = lecteur[2];
        string ecran_largeur  = lecteur[3];
        string ecran_hauteur  = lecteur[4];

        const string valeurs = "'" + adress_mac + "', '" + adress_ip + "', '" + os + "', " + ecran_largeur + ", " + ecran_hauteur;

        maM_BDD->enregistrerDonnees("config_reseau", colonnes, valeurs);
    }

    cout << "[DEBUG] [Config Reseau] Configuration sauvegardée avec succès en BDD !" << endl;
}
