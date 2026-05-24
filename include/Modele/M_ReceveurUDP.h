#pragma once

#include <string>
#include "Modele/M_ProtocoleReseau.h"

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

using namespace std;

class M_ReceveurUDP {
public:
    M_ReceveurUDP(int port, const string& adresseMulticast);
    ~M_ReceveurUDP();

    bool recevoir(PaquetControle &paquet) const;
    int recevoirAvecTimeout(char* buffer, int tailleMax, string& ipEmetteur, int timeoutMs) const;
    void envoyerReponse(const string& ipCible, int portCible, const string& message);

private:
    SocketType descripteurSocket = INVALID_SOCKET;
    ip_mreq groupeMulticast{};
};