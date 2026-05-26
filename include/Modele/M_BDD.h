#pragma once

#include <string>
#include <vector>

#include "../libs/sqlite3/sqlite3.h"

using namespace std;

/**
 * Classe gérant les interactions avec la base de données SQLite.
 */
class M_BDD {
public:
    M_BDD();

    /**
     * Crée une table dans la base de données.
     * @param nomTable Le nom de la table à créer.
     */
    void createTable(string nomTable);

    /**
     * Enregistre des informations ligne par ligne.
     * @param nomTable Le nom de la table.
     * @param nomColonnes Les colonnes cibles.
     * @param values Les valeurs à insérer.
     * @return L'identifiant ou un code de statut.
     */
    int enregistrerDonnees(string nomTable, string nomColonnes, string values);

    /**
     * Actualise des informations dans un champ spécifique.
     * @param nomTable Le nom de la table.
     * @param nomColonne La colonne à modifier.
     * @param nomId Le nom de la colonne identifiante.
     * @param id L'identifiant de la ligne.
     * @param value La nouvelle valeur.
     */
    void actualiserDonnees(string nomTable, string nomColonne, string nomId, int id, string value);

    /**
     * Récupère des données selon des critères spécifiques.
     * @param select Les colonnes à sélectionner.
     * @param from La table source.
     * @param where La clause de condition.
     * @return Un vecteur de vecteurs contenant les résultats.
     */
    vector<vector<string>> recupereDonnees(string select, string from, string where) const;

    /**
     * Supprime des données selon une condition.
     * @param nomTable Le nom de la table.
     * @param condition La condition de suppression.
     */
    void supprimerDonnees(string nomTable, string condition);

private:
    sqlite3* m_db;
};
