#pragma once

#include <string>
#include <map>
#include <stdexcept>

using namespace std;

/**
 * @class M_JsonUtil
 * @brief Utilitaire JSON robuste pour objets plats (cle->valeur string).
 * Sérialisation et désérialisation conformes à la spécification RFC 8259.
 */
class M_JsonUtil {
public:
    /**
     * @brief Construit une chaîne JSON à partir d'une map plate de clés/valeurs.
     * @param champs Map contenant les couples de clés et de valeurs à sérialiser.
     * @return Une chaîne de caractères formalisée en JSON.
     */
    static string construire(const map<string, string>& champs);

    /**
     * @brief Analyse une chaîne JSON pour en extraire une map plate de clés/valeurs.
     * @param json Chaîne JSON brute à analyser.
     * @return Une map contenant les paires de clés et valeurs extraites.
     */
    static map<string, string> parser(const string& json);

private:
    /**
     * @brief Ajoute les séquences d'échappement nécessaires aux caractères spéciaux d'une chaîne.
     * @param s Chaîne d'entrée brute.
     * @return Chaîne convertie avec caractères échappés.
     */
    static string echapperChaine(const string& s);

    /**
     * @brief Lit et extrait une chaîne de caractères entre guillemets depuis une position donnée.
     * @param json Chaîne JSON source.
     * @param pos Référence vers la position courante de lecture dans la chaîne.
     * @return La chaîne décodée.
     */
    static string lireChaine(const string& json, size_t& pos);

    /**
     * @brief Lit une valeur brute non entourée de guillemets (numérique, booléen, null).
     * @param json Chaîne JSON source.
     * @param pos Référence vers la position courante de lecture.
     * @return La valeur sous forme de chaîne brute.
     */
    static string lireValeurBrute(const string& json, size_t& pos);

    /**
     * @brief Avance l'index de lecture pour ignorer les espaces, tabulations et retours à la ligne.
     * @param json Chaîne JSON source.
     * @param pos Référence vers la position courante à mettre à jour.
     */
    static void sauterEspaces(const string& json, size_t& pos);
};