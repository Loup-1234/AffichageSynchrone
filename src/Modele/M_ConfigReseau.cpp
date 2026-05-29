#include <iostream>
#include <iomanip>

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
    string cheminConf = dossier + "/conf.json";
    error_code ec;
    filesystem::remove(cheminConf, ec);

    // Envoi d'une commande specifique d initialisation reseau
    expediteur.transmettreCommande(Expediteur::AUTRE, TypeCommande::CONNECTION, Action::PLAY, 0);

    bool transfertReussi = false;

    //expediteur.recevoir();

#ifdef _WIN32
    M_TFTP tftp;
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
