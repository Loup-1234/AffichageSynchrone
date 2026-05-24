#pragma once

#include <cstdint>
#include <string>

using namespace std;

/**
 * @enum Expediteur
 * @brief Identifie l'origine de la commande réseau.
 */
enum class Expediteur : uint8_t {
    MASTER = 0, ///< Instance principale contrôlant les autres.
    AUTRE = 1   ///< Autres types d'expéditeurs possibles.
};

/**
 * @enum TypeCommande
 * @brief Définit la nature du message UDP.
 */
enum class TypeCommande : uint8_t {
    ORDRE = 0,      ///< Commande d'action immédiate.
    CONNECTION = 1, ///< Message lié à l'établissement de la connexion.
};

/**
 * @enum Action
 * @brief Liste des commandes de contrôle de lecture disponibles.
 */
enum class Action : uint8_t {
    PLAY = 0,
    PAUSE = 1,
    STOP = 2,
    VOLUME = 3,
    PROGRESSION = 4,
    VITESSE = 5,
};

#pragma pack(push, 1)
/**
 * @struct PaquetControle
 * @brief Structure de données compacte envoyée sur le réseau.
 * L'alignement est forcé à 1 octet pour garantir que la structure
 * a la même taille sur toutes les plateformes (7 octets au total).
 */
struct PaquetControle {
    Expediteur exp;    ///< Qui envoie la commande.
    TypeCommande type; ///< Type de message.
    Action action;     ///< Action à réaliser.
    float valeur;      ///< Valeur associée (volume, position temporelle, etc.).
};
#pragma pack(pop)

/**
 * @struct LecteurConfig
 * @brief Configuration propre et typée pour un lecteur cible.
 */
struct LecteurConfig {
    int id;
    string ip;
    int nbVideosCapacite;
};