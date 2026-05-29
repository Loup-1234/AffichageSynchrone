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

    // [Alain] Déplacé dans getConfigReseau pour simplifier le code
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
    // Tu peux ajuster cette valeur (ex: 3, 4 ou 5 secondes) selon la réactivité de tes lecteurs.
    std::this_thread::sleep_for(std::chrono::seconds(4));

    // 4. Traitement de TOUS les fichiers reçus dans le dossier
    bool auMoinsUnLecteurRecu = false;

    if (fs::exists(dossier) && fs::is_directory(dossier)) {
        // On parcourt le dossier à la recherche de fichiers comportant l'extension .json
        for (const auto& entry : fs::directory_iterator(dossier)) {
            if (entry.is_regular_file() && entry.path().extension() == ".json") {
                string cheminFichier = entry.path().string();
                cout << "[DEBUG] Nouveau lecteur detecte ! Traitement de : " << entry.path().filename().string() << endl;

                // On appelle ta fonction existante pour parser et insérer ce JSON spécifique en BDD
                enregistrerJson(cheminFichier);
                auMoinsUnLecteurRecu = true;

                // [Optionnel] Nettoyage : On supprime le fichier physique après traitement
                // pour que le dossier reste propre pour la prochaine recherche.
                fs::remove(entry.path());
            }
        }
    }

    // 5. Bilan de l'opération
    if (auMoinsUnLecteurRecu) {
        cout << "[DEBUG] Fin de la recherche. Tous les lecteurs detectes ont ete enregistres en Base de Donnees." << endl;
    } else {
        cerr << "[WARNING] Aucun lecteur physique n'a repondu a la commande multicast dans le temps imparti." << endl;
    }
}

vector<vector<string>> M_ConfigReseau::getConfigReseau() {
    configReseau = maM_BDD->recupereDonnees("*", "config_reseau", "");
    return configReseau;
}
