#pragma once

#include "M_ConfigReseau.h"
#include "M_ExpediteurUDP.h"
#include <vector>
#include <string>

#ifdef _WIN32
    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif
    #include <winsock2.h>
    #include <ws2tcpip.h>
    using SocketType = SOCKET;
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/select.h>
using SocketType = int;
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#endif

class M_DecouverteReseau {
public:
    explicit M_DecouverteReseau(const M_ConfigReseau& config);
    ~M_DecouverteReseau();

    /**
     * @brief Lance une session de découverte sur le réseau via la structure PaquetControle.
     * @param timeoutMs Temps d'attente des réponses en millisecondes.
     */
    void lancerDecouverte(int timeoutMs = 2000);

    const std::vector<std::string>& getReponsesBrutes() const;
    void afficherResultats() const;

private:
    void attendreReponses(int timeoutMs);

    M_ConfigReseau m_config;
    M_ExpediteurUDP m_expediteur;
    std::vector<std::string> m_reponses;
    SocketType m_socketReception;
};