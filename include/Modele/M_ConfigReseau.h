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
 * Cette classe permet de récupérer les informations réseau depuis une base de données,
 * de les visualiser, et d'importer des configurations à partir de fichiers JSON.
 */
class M_ConfigReseau {
private:
    M_UDP expediteur; /**< Instance de l'expéditeur UDP pour l'envoi de commandes. */

    M_BDD* maM_BDD; /**< Pointeur vers le gestionnaire de la base de données. */

    vector<vector<string>> configReseau; /**< Stockage local de la table de configuration réseau. */

    //vector<string> ipLecteurs;

public:
    /**
     * @brief Constructeur de la classe M_ConfigReseau.
     * @param pMaBDD Pointeur vers l'instance de la base de données à utiliser.
     * @param ip Adresse IP de destination pour les commandes UDP.
     * @param port Port de destination pour les commandes UDP.
     */
    M_ConfigReseau(M_BDD* pMaBDD, const string &ip, int port);

    ~M_ConfigReseau();

    /**
     * @brief Récupère et affiche les données des lecteurs physiques à l'écran.
     */
    void visualiserLecteurPhysique();

    /**
     * @brief Lit un fichier JSON et insère ses données dans la base de données.
     * @param fichierJson Chemin complet du fichier JSON à analyser.
     */
    void enregistrerJson(string fichierJson);

    /**
     * @brief Parcourt un dossier et intègre tous les fichiers JSON qu'il contient.
     * @param dossierJson Chemin du dossier contenant les fichiers de configuration.
     */
    void enregistrerConfigurationReseau(string dossierJson);

    /**
     * @brief Lance la recherche et l'importation des configurations (alias de enregistrerConfigurationReseau).
     * @param dossier Chemin du dossier/fichier cible pour la recherche.
     */
    void rechercherLecteurPhysique(string dossier);

    /**
     * @brief Accesseur (getter) pour obtenir la configuration réseau stockée localement.
     * @return Le tableau 2D contenant les lignes et colonnes de la configuration.
     */
    vector<vector<string>> getConfigReseau();
};