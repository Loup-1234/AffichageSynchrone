#include "../../include/Modele/M_ExpediteurUDP_W.h"
#include <ws2tcpip.h>

M_ExpediteurUDP_W::M_ExpediteurUDP_W(const string &ipMulticast, const int port) {
    WSADATA donneesWsa;
    if (WSAStartup(MAKEWORD(2, 2), &donneesWsa) != 0) return;

    descripteurSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (descripteurSocket != INVALID_SOCKET) {
        int ttl = 1; // Time To Live
        if (setsockopt(descripteurSocket, IPPROTO_IP, IP_MULTICAST_TTL, (char *)&ttl, sizeof(ttl)) == SOCKET_ERROR) {
            closesocket(descripteurSocket);
            descripteurSocket = INVALID_SOCKET;
            return;
        }

        adresseDest.sin_family = AF_INET;
        adresseDest.sin_port = htons(port);
        inet_pton(AF_INET, ipMulticast.c_str(), &adresseDest.sin_addr);
    }
}

M_ExpediteurUDP_W::~M_ExpediteurUDP_W() {
    if (descripteurSocket != INVALID_SOCKET) closesocket(descripteurSocket);
    WSACleanup();
}

bool M_ExpediteurUDP_W::envoyer(const void *donnees, const int taille) {
    if (descripteurSocket == INVALID_SOCKET) return false;

    return sendto(descripteurSocket, static_cast<const char *>(donnees), taille, 0,
                  reinterpret_cast<sockaddr *>(&adresseDest), sizeof(adresseDest)) != SOCKET_ERROR;
}

void M_ExpediteurUDP_W::transmettreCommande(const Expediteur exp, const TypeCommande type, const Action action, const float valeur) {
    const PaquetControle p{ exp, type, action, valeur };
    envoyer(&p, sizeof(p));
}