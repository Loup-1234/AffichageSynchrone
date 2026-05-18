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

/**
 * @class M_ExpediteurUDP
 * @brief Gère l'envoi de paquets UDP pour la synchronisation multi-écrans.
 */
class M_ExpediteurUDP {
public:
    M_ExpediteurUDP(const std::string &ip, int port);
    ~M_ExpediteurUDP();

    bool envoyer(const void *donnees, int taille);
    void transmettreCommande(Expediteur exp, TypeCommande type, Action action, float valeur);

private:
    SocketType descripteurSocket = INVALID_SOCKET;
    sockaddr_in adresseDest{};
};