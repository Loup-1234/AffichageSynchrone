#include "Modele/M_ReceveurUDP.h"

M_ReceveurUDP::M_ReceveurUDP(int port, const std::string& adresseMulticast) {
#ifdef _WIN32
    WSADATA donneesWsa;
    if (WSAStartup(MAKEWORD(2, 2), &donneesWsa) != 0) return;
#endif

    descripteurSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (descripteurSocket != INVALID_SOCKET) {
        // Permettre à d'autres sockets de se lier à ce port
        int opt = 1;
        setsockopt(descripteurSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));

        sockaddr_in adresseLocale{};
        adresseLocale.sin_family = AF_INET;
        adresseLocale.sin_port = htons(port);
        adresseLocale.sin_addr.s_addr = INADDR_ANY;

        if (bind(descripteurSocket, reinterpret_cast<sockaddr*>(&adresseLocale), sizeof(adresseLocale)) == SOCKET_ERROR) {
#ifdef _WIN32
            closesocket(descripteurSocket);
#else
            close(descripteurSocket);
#endif
            descripteurSocket = INVALID_SOCKET;
            return;
        }

        // Joindre le groupe multicast
        inet_pton(AF_INET, adresseMulticast.c_str(), &groupeMulticast.imr_multiaddr);
        groupeMulticast.imr_interface.s_addr = INADDR_ANY;
        
        if (setsockopt(descripteurSocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (const char*)&groupeMulticast, sizeof(groupeMulticast)) < 0) {
#ifdef _WIN32
            closesocket(descripteurSocket);
#else
            close(descripteurSocket);
#endif
            descripteurSocket = INVALID_SOCKET;
            return;
        }

        // Mode non-bloquant
#ifdef _WIN32
        u_long mode = 1;
        ioctlsocket(descripteurSocket, FIONBIO, &mode);
#endif
    }
}

M_ReceveurUDP::~M_ReceveurUDP() {
    if (descripteurSocket != INVALID_SOCKET) {
        setsockopt(descripteurSocket, IPPROTO_IP, IP_DROP_MEMBERSHIP, (const char*)&groupeMulticast, sizeof(groupeMulticast));
#ifdef _WIN32
        closesocket(descripteurSocket);
        WSACleanup();
#else
        close(descripteurSocket);
#endif
    }
}

bool M_ReceveurUDP::recevoir(PaquetControle &paquet) const {
    if (descripteurSocket == INVALID_SOCKET) return false;

    sockaddr_in adresseSource{};
    SockLenType tailleSource = sizeof(adresseSource);

    int resultat = recvfrom(descripteurSocket, reinterpret_cast<char*>(&paquet), sizeof(PaquetControle), 0,
                            reinterpret_cast<sockaddr*>(&adresseSource), &tailleSource);

    return (resultat == sizeof(PaquetControle));
}

int M_ReceveurUDP::recevoirAvecTimeout(char* buffer, const int tailleMax, std::string& ipEmetteur, const int timeoutMs) const {
    if (descripteurSocket == INVALID_SOCKET) return -1;

    fd_set ensemble;
    FD_ZERO(&ensemble);
    FD_SET(descripteurSocket, &ensemble);

    timeval tv;
    tv.tv_sec = timeoutMs / 1000;
    tv.tv_usec = (timeoutMs % 1000) * 1000;

    int pret = select(static_cast<int>(descripteurSocket) + 1, &ensemble, nullptr, nullptr, &tv);
    if (pret <= 0) return pret; // 0 = timeout, -1 = erreur

    sockaddr_in adresseSource{};
    SockLenType tailleSource = sizeof(adresseSource);

    int nbOctets = recvfrom(descripteurSocket, buffer, tailleMax, 0,
                            reinterpret_cast<sockaddr*>(&adresseSource), &tailleSource);

    if (nbOctets > 0) {
        char ipStr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(adresseSource.sin_addr), ipStr, INET_ADDRSTRLEN);
        ipEmetteur = std::string(ipStr);
    }

    return nbOctets;
}

void M_ReceveurUDP::envoyerReponse(const std::string& ipCible, int portCible, const std::string& message) {
    SocketType sockReponse = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockReponse == INVALID_SOCKET) return;

    sockaddr_in addrCible{};
    addrCible.sin_family = AF_INET;
    addrCible.sin_port = htons(portCible);
    inet_pton(AF_INET, ipCible.c_str(), &addrCible.sin_addr);

    sendto(sockReponse, message.c_str(), static_cast<int>(message.size()), 0,
           reinterpret_cast<sockaddr*>(&addrCible), sizeof(addrCible));

#ifdef _WIN32
    closesocket(sockReponse);
#else
    close(sockReponse);
#endif
}