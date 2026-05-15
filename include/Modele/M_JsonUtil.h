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

/**
 * Utilitaire JSON robuste pour objets plats (cle->valeur string).
 * Serialisation et deserialisation conformes RFC 8259.
 */
class M_JsonUtil {
public:
    static std::string construire(const std::map<std::string, std::string>& champs);
    static std::map<std::string, std::string> parser(const std::string& json);

private:
    static std::string echapperChaine(const std::string& s);
    static std::string lireChaine(const std::string& json, size_t& pos);
    static std::string lireValeurBrute(const std::string& json, size_t& pos);
    static void sauterEspaces(const std::string& json, size_t& pos);
};
