#include "../../include/Modele/M_ExpediteurUDP_W.h"

#include <ws2tcpip.h>

M_ExpediteurUDP_W::M_ExpediteurUDP_W(const string &ipGroupe, const int port) {
    WSADATA donneesWsa;
    if (WSAStartup(MAKEWORD(2, 2), &donneesWsa) != 0) return;

    descripteurSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (descripteurSocket != INVALID_SOCKET) {
        adresseDest.sin_family = AF_INET;
        adresseDest.sin_port = htons(port);
        inet_pton(AF_INET, ipGroupe.c_str(), &adresseDest.sin_addr);
    }
}

M_ExpediteurUDP_W::~M_ExpediteurUDP_W() {
    if (descripteurSocket != INVALID_SOCKET) closesocket(descripteurSocket);
    WSACleanup();
}

bool M_ExpediteurUDP_W::Envoyer(const void *donnees, const int taille) {
    if (descripteurSocket == INVALID_SOCKET) return false;

    return sendto(descripteurSocket, static_cast<const char *>(donnees), taille, 0,
                  reinterpret_cast<sockaddr *>(&adresseDest), sizeof(adresseDest)) != SOCKET_ERROR;
}