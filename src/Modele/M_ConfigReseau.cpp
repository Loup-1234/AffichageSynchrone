#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>
#include <filesystem>

#include "Modele/M_ConfigReseau.h"

#ifdef _WIN32
#include "Modele/M_TFTP.h"
#endif

M_ConfigReseau::M_ConfigReseau(const string &ip, int port) : configReseau({}) {
    expediteur.initialiserMulticast(ip, port);
}

M_ConfigReseau::~M_ConfigReseau() {
    expediteur.fermer();
}

void M_ConfigReseau::visualiserLecteurPhysique() {
    configReseau = maM_BDD.recupereDonnees("*", "config_reseau", "");
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

    string adress_mac = listeValeurs.value("mac", "00:00:00:00:00:00");
    string adress_ip  = listeValeurs.value("ip", "0.0.0.0");
    string os          = listeValeurs.value("os", "Inconnu");
    int ecran_largeur  = listeValeurs.value("largeurEcran", 0);
    int ecran_hauteur  = listeValeurs.value("hauteurEcran", 0);

    if (!listeValeurs.contains("mac") || !listeValeurs.contains("ip")) {
        cerr << "[WARNING] Cles attendues ('mac' ou 'ip') manquantes dans le JSON recu !" << endl;
    }

    string colonnes = "adresse_mac, adresse_ip, os, ecran_largeur, ecran_hauteur";
    string valeurs = "'" + adress_mac + "', '" + adress_ip + "', '" + os + "', " + to_string(ecran_largeur) + ", " + to_string(ecran_hauteur);

    maM_BDD.enregistrerDonnees("config_reseau", colonnes, valeurs);
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

#ifdef _WIN32
    if (!serveurTftpLance) {
        std::thread tftpThread([dossier]() {
            M_TFTP tftp;
            tftp.demarrerServeurMultiThread(dossier);
        });
        tftpThread.detach();

        serveurTftpLance = true;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
#endif

    expediteur.transmettreCommande(Expediteur::AUTRE, TypeCommande::CONNECTION, Action::PLAY, 0);
    cout << "[DEBUG] Trame multicast envoyee. Collecte des fichiers JSON en cours..." << endl;

    std::this_thread::sleep_for(std::chrono::seconds(4));

    configReseau.clear();

    bool auMoinsUnLecteurRecu = false;

    if (fs::exists(dossier) && fs::is_directory(dossier)) {
        for (const auto& entry : fs::directory_iterator(dossier)) {
            if (entry.is_regular_file() && entry.path().extension() == ".json") {
                string cheminFichier = entry.path().string();
                cout << "[DEBUG] Nouveau lecteur detecte ! Traitement de : " << entry.path().filename().string() << endl;

                ifstream fichier(cheminFichier);
                if (fichier.is_open()) {
                    json listeValeurs;
                    try {
                        fichier >> listeValeurs;

                        string adress_mac = listeValeurs.value("mac", "00:00:00:00:00:00");
                        string adress_ip  = listeValeurs.value("ip", "0.0.0.0");
                        string os          = listeValeurs.value("os", "Inconnu");
                        int ecran_largeur  = listeValeurs.value("largeurEcran", 0);
                        int ecran_hauteur  = listeValeurs.value("hauteurEcran", 0);

                        vector<string> ligneLecteur = {
                            adress_mac,
                            adress_ip,
                            os,
                            to_string(ecran_largeur),
                            to_string(ecran_hauteur)
                        };

                        configReseau.push_back(ligneLecteur);
                        auMoinsUnLecteurRecu = true;

                    } catch (const json::parse_error& e) {
                        cerr << "[DEBUG] [Config Reseau] [ERROR] Erreur de syntaxe JSON dans "
                             << entry.path().filename().string() << " : " << e.what() << endl;
                    }
                    fichier.close();
                }
                fs::remove(entry.path());
            }
        }
    }

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

    maM_BDD.supprimerDonnees("config_reseau", "true");

    string colonnes = "adresse_mac, adresse_ip, os, ecran_largeur, ecran_hauteur";

    for (const auto& lecteur : configReseau) {
        string adress_mac     = lecteur[0];
        string adress_ip      = lecteur[1];
        string os             = lecteur[2];
        string ecran_largeur  = lecteur[3];
        string ecran_hauteur  = lecteur[4];

        const string valeurs = "'" + adress_mac + "', '" + adress_ip + "', '" + os + "', " + ecran_largeur + ", " + ecran_hauteur;

        maM_BDD.enregistrerDonnees("config_reseau", colonnes, valeurs);
    }

    cout << "[DEBUG] [Config Reseau] Configuration sauvegardee avec succes en BDD !" << endl;
}