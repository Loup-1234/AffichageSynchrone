/**
* @file M_BDD.h
 * @brief Déclaration de la classe M_BDD pour la gestion et l'accès à une base de données SQLite3.
 * @author Robin Calendreau
 * @date 2026
 */

#pragma once

#include <string>
#include <vector>

#include "../libs/sqlite3/sqlite3.h"

using namespace std;

/**
 * @class M_BDD
 * @brief Classe gérant les interactions et les requêtes avec la base de données SQLite.
 */
class M_BDD {
public:
    /**
     * @brief Constructeur par défaut qui initialise la connexion à la base de données.
     */
    M_BDD();

    /**
     * @brief Crée une table dans la base de données.
     * @param nomTable Le nom de la table à créer.
     */
    void createTable(string nomTable);

    /**
     * @brief Enregistre des informations ligne par ligne.
     * @param nomTable Le nom de la table.
     * @param nomColonnes Les colonnes cibles.
     * @param values Les valeurs à insérer.
     * @return L'identifiant ou un code de statut.
     */
    int enregistrerDonnees(string nomTable, string nomColonnes, string values);

    /**
     * @brief Actualise des informations dans un champ spécifique.
     * @param nomTable Le nom de la table.
     * @param nomColonne La colonne à modifier.
     * @param nomId Le nom de la colonne identifiante.
     * @param id L'identifiant de la ligne.
     * @param value La nouvelle valeur.
     */
    void actualiserDonnees(string nomTable, string nomColonne, string nomId, int id, string value);

    /**
     * @brief Récupère des données selon des critères spécifiques.
     * @param select Les colonnes à sélectionner.
     * @param from La table source.
     * @param where La clause de condition.
     * @return Un vecteur de vecteurs contenant les résultats sous forme de chaînes.
     */
    vector<vector<string> > recupereDonnees(string select, string from, string where) const;

    /**
     * @brief Supprime des données selon une condition.
     * @param nomTable Le nom de la table.
     * @param condition La condition de suppression.
     */
    void supprimerDonnees(string nomTable, string condition);

private:
    sqlite3 *m_db; /**< Pointeur vers l'objet de connexion SQLite3. */
};