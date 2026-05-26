#pragma once

#include <cstdint>
#include <string>

using namespace std;

/**
 * @enum Expediteur
 * @brief Identifie l'origine et le rôle de la commande réseau.
 */
enum class Expediteur : uint8_t {
    MASTER = 0, ///< Instance principale contrôlant la synchronisation des autres.
    AUTRE = 1   ///< Autres types d'expéditeurs ou noeuds secondaires.
};

/**
 * @enum TypeCommande
 * @brief Définit la nature et le canal du message UDP.
 */
enum class TypeCommande : uint8_t {
    ORDRE = 0,      ///< Commande d'action ou de contrôle immédiat de flux.
    CONNECTION = 1, ///< Message protocolaire lié à l'établissement de la connexion ou découverte.
};

/**
 * @enum Action
 * @brief Liste des commandes de contrôle de lecture multimédia disponibles.
 */
enum class Action : uint8_t {
    PLAY = 0,        ///< Lance ou reprend la lecture des fichiers vidéo.
    PAUSE = 1,       ///< Suspend temporairement la lecture multimédia.
    STOP = 2,        ///< Arrête la lecture et réinitialise la tête de lecture au début.
    VOLUME = 3,      ///< Ordre de modification du niveau sonore des lecteurs.
    PROGRESSION = 4, ///< Ajustement de la position temporelle (Seek) au sein du média.
    VITESSE = 5,     ///< Modification du facteur de vitesse d'exécution de la vidéo.
};

#pragma pack(push, 1)
/**
 * @struct PaquetControle
 * @brief Structure de données réseau compacte envoyée via protocole UDP.
 * L'alignement mémoire est forcé à 1 octet pour garantir une taille stricte
 * et identique de 7 octets sur toutes les architectures matérielles.
 */
struct PaquetControle {
    Expediteur exp;    ///< Entité émettrice à l'origine du message réseau.
    TypeCommande type; ///< Catégorie fonctionnelle du message protocolaire.
    Action action;     ///< Commande ou action spécifique à exécuter sur le lecteur.
    float valeur;      ///< Donnée numérique associée à l'action (position, niveau de volume, etc.).
};
#pragma pack(pop)

/**
 * @struct LecteurConfig
 * @brief Structure décrivant la topologie logicielle et matérielle d'un écran cible.
 */
struct LecteurConfig {
    int id;               ///< Identifiant numérique séquentiel unique (0 pour le Master local).
    string ip;            ///< Adresse IP de la machine cible pour les transmissions réseau.
    int nbVideosCapacite; ///< Quota maximal de flux vidéo affecté à cet écran dans la mosaïque globale.
};