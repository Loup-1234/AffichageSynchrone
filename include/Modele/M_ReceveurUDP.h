#pragma once

#include <string>
#include "Modele/M_ProtocoleReseau.h"

// Gestion multiplateforme et sécurité pour la macro Windows
#ifdef _WIN32
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>
    #include <winsock2.h>
    #include <ws2tcpip.h>
    using SocketType = SOCKET;
using SockLenType = int;
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/select.h>
using SocketType = int;
using SockLenType = socklen_t;
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#endif

class M_ReceveurUDP {
public:
    M_ReceveurUDP(int port, const std::string& adresseMulticast);
    ~M_ReceveurUDP();

    bool recevoir(PaquetControle &paquet) const;
    int recevoirAvecTimeout(char* buffer, int tailleMax, std::string& ipEmetteur, int timeoutMs) const;
    void envoyerReponse(const std::string& ipCible, int portCible, const std::string& message);

private:
    SocketType descripteurSocket = INVALID_SOCKET;
    struct ip_mreq groupeMulticast{};
};