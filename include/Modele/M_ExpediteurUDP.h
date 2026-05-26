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
using SocketType = SOCKET; ///< Définit le type de socket selon l'OS (Windows).
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
using SocketType = int;    ///< Définit le type de socket selon l'OS (Linux/POSIX).
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#endif

using namespace std;

/**
 * @class M_ExpediteurUDP
 * @brief Gestionnaire de communication réseau pour l'envoi de trames UDP de synchronisation.
 * * Cette classe encapsule la gestion des sockets bas niveau pour permettre la diffusion
 * de commandes de contrôle vers les nœuds distants de la grappe d'écrans.
 */
class M_ExpediteurUDP {
public:
    /**
     * @brief Constructeur initialisant la socket de communication.
     * @param ip Adresse IP de destination ou multicast.
     * @param port Port réseau cible pour l'écoute des commandes.
     */
    M_ExpediteurUDP(const string &ip, int port);

    /**
     * @brief Destructeur garantissant la fermeture propre de la socket.
     */
    ~M_ExpediteurUDP();

    /**
     * @brief Envoie un bloc de données brut via le protocole UDP.
     * @param donnees Pointeur vers le buffer de données à transmettre.
     * @param taille Taille en octets du bloc de données.
     * @return True si l'envoi a réussi, False sinon.
     */
    bool envoyer(const void *donnees, int taille);

    /**
     * @brief Formate et transmet une commande de contrôle structurée.
     * @param exp Identifiant de l'expéditeur (Master ou autre).
     * @param type Catégorie de la commande (Ordre ou connexion).
     * @param action Type d'action multimédia à exécuter (PLAY, PAUSE, etc.).
     * @param valeur Donnée paramétrique associée à l'action.
     */
    void transmettreCommande(Expediteur exp, TypeCommande type, Action action, float valeur);

private:
    SocketType descripteurSocket = INVALID_SOCKET; ///< Identifiant de la socket active.
    sockaddr_in adresseDest{};                     ///< Structure contenant les paramètres de destination (IP/Port).
};