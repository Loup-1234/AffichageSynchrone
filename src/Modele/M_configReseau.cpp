#include <iostream>

#include "Modele/M_configReseau.h"

#include "Modele/M_TFTP_W.h"

M_configReseau::M_configReseau(M_BDD* pMaBDD, const string &ip, int port) : Expediteur(ip, port), maM_BDD(pMaBDD), configReseau({}) {
}

void M_configReseau::visualiserLecteurPhysique() {
    configReseau = maM_BDD->recupereDonnees("*", "config_reseau", "");
}

void M_configReseau::enregistrerJson(string fichierJson) {

    ifstream fichier(fichierJson);

    if (!fichier.is_open()) {
        cout << "Erreur : Impossible d'ouvrir le fichier " << fichierJson << endl;
        cout << "Verifie que le dossier et le fichier existent au bon endroit !" << endl;
        return;
    }

    json listeValeurs;
    fichier >> listeValeurs;

    string adress_ip = listeValeurs["adresse_mac"].get<string>();
    string adress_mac = listeValeurs["adresse_ip"].get<string>();
    string os = listeValeurs["os"].get<string>();
    int ecran_largeur = listeValeurs["ecran_largeur"].get<int>();
    int ecran_hauteur = listeValeurs["ecran_hauteur"].get<int>();

    string colonnes = "adresse_mac, adresse_ip, os, ecran_largeur, ecran_hauteur";

    string valeurs = "'" + adress_mac + "', '" + adress_ip + "', '" + os + "', " + to_string(ecran_largeur) + ", " + to_string(ecran_hauteur);

    maM_BDD->enregistrerDonnees("config_reseau", colonnes, valeurs);
}

void M_configReseau::enregistrerConfigurationReseau(string dossierJson) {
    for (const auto& entree : filesystem::directory_iterator(dossierJson)) {

        string fichierTrouve = entree.path().string();

        enregistrerJson(fichierTrouve);
    }
}

void M_configReseau::rechercherLecteurPhysique(string fichierJson) {

    Expediteur.transmettreCommande(Expediteur::AUTRE, TypeCommande::CONNECTION, Action::PLAY, 0);

    M_TFTP_W tftp;

    tftp.recevoirFichierPousse( "JSON_recue");

    enregistrerConfigurationReseau(fichierJson);
}

vector<vector<string>> M_configReseau::getConfigReseau() {
    return configReseau;
}
