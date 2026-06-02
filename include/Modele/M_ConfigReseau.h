/**
* @file M_ConfigReseau.h
 * @brief Déclaration de la classe M_ConfigReseau pour la gestion de la topologie et des configurations réseau.
 * @author Robin Calendreau
 * @date 2026
 */

#pragma once

#include <fstream>
#include "M_BDD.h"
#include "M_UDP.h"
#include "../libs/JSON/json.hpp"

using json = nlohmann::json;
using namespace std;

/**
 * @class M_ConfigReseau
 * @brief Gestionnaire de la configuration réseau des lecteurs physiques.
 *
 * Cette classe permet de récupérer les informations réseau depuis la base de données interne,
 * de stocker localement la topologie réseau, et d'importer des configurations à partir de fichiers JSON.
 */
class M_ConfigReseau {
private:
    M_UDP expediteur; /**< Instance de l'expéditeur UDP pour l'envoi de commandes. */
    M_BDD maM_BDD; /**< Instance autonome et encapsulée d'accès à la base de données SQLite. */
    vector<vector<string> > configReseau; /**< Stockage local de la table de configuration réseau. */
    vector<string> ipLecteurs; /**< Liste interne des adresses IP des lecteurs détectés. */

public:
    /**
     * @brief Constructeur de la classe M_ConfigReseau.
     * @param ip Adresse IP de destination pour les commandes UDP.
     * @param port Port de destination pour les commandes UDP.
     */
    M_ConfigReseau(const string &ip, int port);

    /**
     * @brief Destructeur de la classe M_ConfigReseau. Libère les ressources sockets.
     */
    ~M_ConfigReseau();

    /**
     * @brief Récupère et met à jour la configuration brute locale depuis la table sqlite correspondante.
     */
    void visualiserLecteurPhysique();

    /**
     * @brief Lit un fichier JSON et insère ses données directement dans la base de données.
     * @param fichierJson Chemin complet du fichier JSON à analyser.
     */
    void enregistrerJson(string fichierJson);

    /**
     * @brief Parcourt un dossier et intègre tous les fichiers JSON qu'il contient.
     * @param dossierJson Chemin du dossier contenant les fichiers de configuration.
     */
    void enregistrerConfigurationReseau(string dossierJson);

    /**
     * @brief Lance la recherche multicast et l'importation dynamique des configurations JSON reçues.
     * @param dossier Chemin du dossier cible pour la capture des fichiers JSON.
     */
    void rechercherLecteurPhysique(string dossier);

    /**
     * @brief Accesseur (getter) pour obtenir la configuration réseau stockée localement.
     * @return Le tableau 2D contenant les lignes et colonnes de la configuration.
     */
    vector<vector<string> > getConfigReseau() { return configReseau; }

    /**
     * @brief Supprime l'ancienne configuration en BDD et persiste l'état de configReseau actuel.
     */
    void sauvegarderConfigActuelle();
};