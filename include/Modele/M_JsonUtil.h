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
 * @class M_JsonUtil
 * @brief Utilitaire de sérialisation et de désérialisation JSON pour structures de données plates.
 * * Cette classe permet de convertir des dictionnaires clé-valeur (map<string, string>)
 * en chaînes JSON valides (RFC 8259) et inversement. Elle gère nativement l'échappement
 * des caractères spéciaux et le respect de la syntaxe JSON standard pour les objets simples.
 */
class M_JsonUtil {
public:
    /**
     * @brief Sérialise une map en une chaîne au format JSON.
     * @param champs Dictionnaire contenant les paires clé-valeur à convertir.
     * @return Une chaîne de caractères représentant l'objet JSON (ex: {"clé":"valeur"}).
     */
    static string construire(const map<string, string>& champs);

    /**
     * @brief Désérialise une chaîne JSON en un dictionnaire map.
     * @param json Chaîne de caractères au format JSON.
     * @return Un dictionnaire contenant les données extraites.
     * @throw std::invalid_argument Si la syntaxe JSON est malformée ou invalide.
     */
    static map<string, string> parser(const string& json);

private:
    /**
     * @brief Échappe les caractères spéciaux d'une chaîne pour garantir la validité JSON.
     * @param s Chaîne brute à traiter (gestion des guillemets, barres obliques, etc.).
     * @return La chaîne formatée selon le standard JSON.
     */
    static string echapperChaine(const string& s);

    /**
     * @brief Analyse une séquence de caractères délimitée par des guillemets.
     * @param json La chaîne JSON complète.
     * @param pos Position courante de lecture (mise à jour par référence).
     * @return La chaîne extraite sans ses délimiteurs JSON.
     */
    static string lireChaine(const string& json, size_t& pos);

    /**
     * @brief Lit la valeur associée à une clé, en gérant le typage JSON (valeurs brutes).
     * @param json La chaîne JSON complète.
     * @param pos Position courante de lecture.
     * @return La valeur sous forme de chaîne de caractères.
     */
    static string lireValeurBrute(const string& json, size_t& pos);

    /**
     * @brief Avance la position de lecture en ignorant les espaces blancs, tabulations et retours à la ligne.
     * @param json La chaîne JSON complète.
     * @param pos Position courante de lecture (incrémentée).
     */
    static void sauterEspaces(const string& json, size_t& pos);
};