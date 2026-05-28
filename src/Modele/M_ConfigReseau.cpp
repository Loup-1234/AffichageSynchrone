#include <iostream>
#include <iomanip>

#include "Modele/M_ConfigReseau.h"

#ifdef _WIN32
#include "Modele/M_TFTP_W.h"
#endif

M_ConfigReseau::M_ConfigReseau(M_BDD* pMaBDD, const string &ip, int port) : Expediteur(ip, port), maM_BDD(pMaBDD), configReseau({}) {
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

    string adress_mac = listeValeurs.value("adresse_mac", "00:00:00:00:00:00");
    string adress_ip  = listeValeurs.value("adresse_ip", "0.0.0.0");
    string os          = listeValeurs.value("os", "Inconnu");
    int ecran_largeur  = listeValeurs.value("ecran_largeur", 0);
    int ecran_hauteur  = listeValeurs.value("ecran_hauteur", 0);

    if (!listeValeurs.contains("adresse_mac") || !listeValeurs.contains("adresse_ip")) {
        cerr << "[WARNING] Cles manquantes dans le JSON recu !" << endl;
        cerr << "Contenu reel du fichier : " << listeValeurs.dump(2) << endl;
        cerr << "Verifie si le lecteur n'envoie pas plutot 'mac' ou 'ip' au lieu de 'adresse_mac'..." << endl;
    }

    string colonnes = "adresse_mac, adresse_ip, os, ecran_largeur, ecran_hauteur";
    string valeurs = "'" + adress_mac + "', '" + adress_ip + "', '" + os + "', " + to_string(ecran_largeur) + ", " + to_string(ecran_hauteur);

    // Sauvegarde immediate dans la table SQLite correspondante
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
    string cheminConf = dossier + "/conf.json";
    error_code ec;
    filesystem::remove(cheminConf, ec);

    // Envoi d une commande specifique d initialisation reseau
    Expediteur.transmettreCommande(Expediteur::AUTRE, TypeCommande::CONNECTION, Action::PLAY, 0);

    bool transfertReussi = false;

#ifdef _WIN32
    M_TFTP_W tftp;
    transfertReussi = tftp.recevoirFichierPousseMaster(cheminConf);
#else

#endif

    if (transfertReussi) {
        enregistrerConfigurationReseau(dossier);
    } else {
        cerr << "Abandon de la configuration : impossible de joindre le lecteur physique." << endl;
    }
}

vector<vector<string>> M_ConfigReseau::getConfigReseau() {
    configReseau = maM_BDD->recupereDonnees("*", "config_reseau", "");
    return configReseau;
}