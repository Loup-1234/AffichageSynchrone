#pragma once

#include "Modele/M_ProtocoleReseau.h"
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

using namespace std;

/**
 * @class M_ExpediteurUDP
 * @brief Gère l'envoi de paquets UDP pour la synchronisation multi-écrans.
 */
class M_ExpediteurUDP {
public:
    /**
     * @brief Constructeur initialisant le socket UDP et configurant les options réseau.
     * @param ip Adresse IP de destination (Unicast, Broadcast ou Multicast).
     * @param port Port de destination des paquets.
     */
    M_ExpediteurUDP(const string &ip, int port);

    /**
     * @brief Destructeur fermant proprement le socket et libérant la pile réseau.
     */
    ~M_ExpediteurUDP();

    /**
     * @brief Envoie des données brutes via le socket UDP configuré.
     * @param donnees Pointeur vers le bloc de données à transmettre.
     * @param taille Taille en octets des données à transmettre.
     * @return true si l'envoi a réussi, false sinon.
     */
    bool envoyer(const void *donnees, int taille);

    /**
     * @brief Transmet une commande structurée de contrôle à distance.
     * @param exp Identifiant du type d'expéditeur.
     * @param type Nature du message UDP.
     * @param action Commande d'action à réaliser.
     * @param valeur Valeur numérique optionnelle associée à l'action.
     */
    void transmettreCommande(Expediteur exp, TypeCommande type, Action action, float valeur);

private:
    SocketType descripteurSocket = INVALID_SOCKET; /**< Identifiant ou descripteur système du socket. */
    sockaddr_in groupeMulticast{}; /**< Structure d'adresse de destination réseau. */
};