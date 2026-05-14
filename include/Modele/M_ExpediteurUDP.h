#pragma once

#include <string>
#include <cstdint>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
using SocketType = SOCKET;
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
using SocketType = int;
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#endif

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
    ORDRE = 0,     ///< Commande d'action immédiate.
    CONNECTION = 1 ///< Message lié à l'établissement de la connexion.
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
    VITESSE = 5
};

#pragma pack(push, 1)
/**
 * @struct PaquetControle
 * @brief Structure de données compacte envoyée sur le réseau.
 * * L'alignement est forcé à 1 octet pour garantir que la structure
 * a la même taille sur toutes les plateformes.
 */
struct PaquetControle {
    Expediteur exp;    ///< Qui envoie la commande.
    TypeCommande type; ///< Type de message.
    Action action;     ///< Action à réaliser.
    float valeur;      ///< Valeur associée (volume, position temporelle, etc.).
};
#pragma pack(pop)

/**
 * @class M_ExpediteurUDP
 * @brief Gère l'envoi de paquets UDP pour la synchronisation multi-écrans.
 */
class M_ExpediteurUDP {
public:
    /**
     * @brief Initialise le socket et prépare l'adresse de destination.
     * @param ip Adresse IP de destination (souvent une adresse de Multicast ou Broadcast).
     * @param port Port de destination.
     */
    M_ExpediteurUDP(const std::string &ip, int port);

    /**
     * @brief Ferme le socket et nettoie les ressources réseau.
     */
    ~M_ExpediteurUDP();

    /**
     * @brief Envoie des données brutes via le socket UDP.
     * @param donnees Pointeur vers les données à envoyer.
     * @param taille Taille des données en octets.
     * @return true si l'envoi a réussi, false sinon.
     */
    bool envoyer(const void *donnees, int taille);

    /**
     * @brief Encapsule et transmet une commande de contrôle.
     * @param exp Origine de la commande.
     * @param type Nature du message.
     * @param action Commande à exécuter.
     * @param valeur Donnée numérique associée à l'action.
     */
    void transmettreCommande(Expediteur exp, TypeCommande type, Action action, float valeur);

private:
    SocketType descripteurSocket = INVALID_SOCKET; ///< Identifiant système du socket.
    sockaddr_in adresseDest{};                     ///< Structure d'adresse réseau de destination.
};