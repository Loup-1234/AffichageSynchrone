#include "Modele/M_BDD.h"
#include <iostream>

M_BDD::M_BDD() {
    sqlite3_open("../AffichageSynchroneBDD.db", &m_db);
}

void M_BDD::createTable(string nomTable) {
    string query = "CREATE TABLE IF NOT EXISTS " + nomTable + " (Texte1 TEXT)";

    int exit = sqlite3_exec(m_db, query.c_str(), nullptr, 0, nullptr);

    if (exit != SQLITE_OK) {
        cout << "la table a mal ete cree" << endl;
    } else {
        cout << "la table a bien ete cree" << endl;
    }
}

int M_BDD::enregistrerDonnees(string nomTable, string nomColonnes, string values) {
    string query = "INSERT INTO " + nomTable + " (" + nomColonnes + ") VALUES (" + values + ")";

    int exit = sqlite3_exec(m_db, query.c_str(), nullptr, 0, nullptr);

    if (exit != SQLITE_OK) {
        cout << "Erreur SQLite (" << exit << ") : " << sqlite3_errmsg(m_db) << endl;
    } else {
        cout << "Ligne insérée avec succes" << endl;
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

    if (sqlite3_prepare_v2(m_db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        int nbColonnes = sqlite3_column_count(stmt);

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

    for (const auto& ligne : tableComplete) {
        for (const string& cellule : ligne) {
            cout << cellule << "   ";
        }
        cout << endl;
    }

    return tableComplete;
}

void M_BDD::actualiserDonnees(string nomTable, string nomColonne, string nomId, int id, string value) {
    string query = "UPDATE " + nomTable + " SET " + nomColonne + " = '" + value + "' " + " WHERE " + nomId + " = " + to_string(id);

    int exit = sqlite3_exec(m_db, query.c_str(), nullptr, 0, nullptr);

    if (exit != SQLITE_OK) {
        cout << "Erreur : " << sqlite3_errmsg(m_db) << endl;
    } else {
        cout << "Ligne modifie avec succes" << endl;
    }
}

void M_BDD::supprimerDonnees(string nomTable, string condition) {
    string query = "DELETE FROM " + nomTable + " WHERE " + condition;

    int exit = sqlite3_exec(m_db, query.c_str(), nullptr, 0, nullptr);

    if (exit != SQLITE_OK) {
        cerr << "Erreur lors de la suppression : " << sqlite3_errmsg(m_db) << endl;
    } else {
        cout << "Nettoyage reussi" << endl;
    }
}
