#pragma once

// ==============================================================
// JsonUtil V1 — Serialiseur/parseur JSON RFC 8259 pour objets plats.
// Gere : toutes les sequences d'echappement, les valeurs numeriques
// et booleennes (converties en string), les espaces ignores.
// API identique a V0 : aucun changement cote appelant.
// Pour une utilisation avancee (imbrication, tableaux), integrer
// nlohmann/json (single_include/nlohmann/json.hpp) et rediriger ici.
// ==============================================================

#include <string>
#include <map>
#include <stdexcept>

using namespace std;

/**
 * Utilitaire JSON robuste pour objets plats (cle->valeur string).
 * Serialisation et deserialisation conformes RFC 8259.
 */
class M_JsonUtil {
public:
    static string construire(const map<string, string>& champs);
    static map<string, string> parser(const string& json);

private:
    static string echapperChaine(const string& s);
    static string lireChaine(const string& json, size_t& pos);
    static string lireValeurBrute(const string& json, size_t& pos);
    static void sauterEspaces(const string& json, size_t& pos);
};
