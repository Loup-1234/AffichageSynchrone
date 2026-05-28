#include "Modele/M_BDD.h"
#include <iostream>
#include <iomanip>

using namespace std;

M_BDD::M_BDD() {
    // Ouverture ou creation du fichier de base de donnees local SQLite
    sqlite3_open("../AffichageSynchroneBDD.db", &m_db);
}

void M_BDD::createTable(string nomTable) {
    // Preparation de la requete de creation par defaut
    string query = "CREATE TABLE IF NOT EXISTS " + nomTable + " (Texte1 TEXT)";

    int exit = sqlite3_exec(m_db, query.c_str(), nullptr, 0, nullptr);

    // Verification du resultat d execution du moteur SQLite
    if (exit != SQLITE_OK) {
        cerr << "[DEBUG] [Base de Donnees] [ERROR] La table '" << nomTable << "' n'a pas pu etre cree." << endl;
    } else {
        cout << "[DEBUG] [Base de Donnees] La table '" << nomTable << "' a ete cree avec succes." << endl;
    }
}

int M_BDD::enregistrerDonnees(string nomTable, string nomColonnes, string values) {
    // Insertion SQL classique
    string query = "INSERT INTO " + nomTable + " (" + nomColonnes + ") VALUES (" + values + ")";

    int exit = sqlite3_exec(m_db, query.c_str(), nullptr, 0, nullptr);

    if (exit != SQLITE_OK) {
        cerr << "[DEBUG] [Base de Donnees] [SQL ERROR] Code " << exit << " : " << sqlite3_errmsg(m_db) << endl;
    } else {
        cout << "[DEBUG] [Base de Donnees] Ligne inseree avec succes dans la table '" << nomTable << "'." << endl;
    }

    return exit;
}

vector<vector<string>> M_BDD::recupereDonnees(string select, string from, string where) const {
    vector<vector<string>> tableComplete;
    sqlite3_stmt* stmt;

    string query = "SELECT " + select + " FROM " + from;

    if (!where.empty()) {
        query += " WHERE " + where;
    }

    // Preparation de la requete SQL et lecture du curseur de resultat
    if (sqlite3_prepare_v2(m_db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        int nbColonnes = sqlite3_column_count(stmt);

        // Parcours de l ensemble des lignes retournées
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            vector<string> ligne;
            for (int i = 0; i < nbColonnes; i++) {
                const unsigned char* value = sqlite3_column_text(stmt, i);
                if (value != nullptr) {
                    ligne.push_back((const char*) value);
                } else {
                    ligne.push_back("NULL");
                }
            }
            tableComplete.push_back(ligne);
        }
    }
    sqlite3_finalize(stmt);

    cout << endl;

    for (const auto& ligne : tableComplete) {
        cout << "[DEBUG] [Base de Donnees] ";
        for (const string& cellule : ligne) {
            cout << left << setw(30) << cellule;
        }
        cout << endl;
    }

    cout << endl;

    return tableComplete;
}

void M_BDD::actualiserDonnees(string nomTable, string nomColonne, string nomId, int id, string value) {
    // Requete de mise a jour
    string query = "UPDATE " + nomTable + " SET " + nomColonne + " = '" + value + "' " + " WHERE " + nomId + " = " + to_string(id);

    int exit = sqlite3_exec(m_db, query.c_str(), nullptr, 0, nullptr);

    if (exit != SQLITE_OK) {
        cerr << "[DEBUG] [Base de Donnees] [SQL ERROR] Impossible de modifier la ligne : " << sqlite3_errmsg(m_db) << endl;
    } else {
        cout << "[DEBUG] [Base de Donnees] Ligne modifiee avec succes dans '" << nomTable << "'." << endl;
    }
}

void M_BDD::supprimerDonnees(string nomTable, string condition) {
    // Requete de suppression de donnees avec condition clause WHERE
    string query = "DELETE FROM " + nomTable + " WHERE " + condition;

    int exit = sqlite3_exec(m_db, query.c_str(), nullptr, 0, nullptr);

    if (exit != SQLITE_OK) {
        cerr << "[DEBUG] [Base de Donnees] [SQL ERROR] Erreur lors de la suppression : " << sqlite3_errmsg(m_db) << endl;
    } else {
        cout << "[DEBUG] [Base de Donnees] Nettoyage reussi de la table '" << nomTable << "'." << endl;
    }
}