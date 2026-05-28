#include "../../include/Modele/M_ExpediteurUDP.h"

M_ExpediteurUDP::M_ExpediteurUDP(const string &ip, const int port) {
#ifdef _WIN32
    // Initialisation de la pile réseau Windows (WSA)
    WSADATA donneesWsa;
    if (WSAStartup(MAKEWORD(2, 2), &donneesWsa) != 0) return;
#endif

    // Création d'un socket UDP (SOCK_DGRAM)
    descripteurSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (descripteurSocket == INVALID_SOCKET) return;

#ifdef _WIN32
    // Configuration pour le Multicast sur Windows
    int ttl = 1;
    if (setsockopt(descripteurSocket, IPPROTO_IP, IP_MULTICAST_TTL, (char *) &ttl, sizeof(ttl)) == SOCKET_ERROR) {
        closesocket(descripteurSocket);
        descripteurSocket = INVALID_SOCKET;
        return;
    }
#else
    // Configuration pour le Broadcast sur Linux/macOS
    int broadcastOpt = 1;
    if (setsockopt(descripteurSocket, SOL_SOCKET, SO_BROADCAST, &broadcastOpt, sizeof(broadcastOpt)) == SOCKET_ERROR) {
        close(descripteurSocket);
        descripteurSocket = INVALID_SOCKET;
        return;
    }
#endif

    // Préparation de la structure d'adresse de destination
    groupeMulticast.sin_family = AF_INET;
    groupeMulticast.sin_port = htons(port); // Conversion en format réseau (Big Endian)
    inet_pton(AF_INET, ip.c_str(), &groupeMulticast.sin_addr);
}

M_ExpediteurUDP::~M_ExpediteurUDP() {
    // Nettoyage de la socket et de la pile reseau selon le systeme d exploitation hote
    if (descripteurSocket != INVALID_SOCKET) {
#ifdef _WIN32
        closesocket(descripteurSocket);
        WSACleanup(); // Libère les ressources Winsock
#else
        close(descripteurSocket);
#endif
    }
}

bool M_ExpediteurUDP::envoyer(const void *donnees, const int taille) {
    if (descripteurSocket == INVALID_SOCKET) return false;

    // Envoi effectif du paquet vers l'adresse stockée dans adresseDest
    const int resultat = sendto(descripteurSocket, static_cast<const char *>(donnees), taille, 0,
                                reinterpret_cast<sockaddr *>(&groupeMulticast), sizeof(groupeMulticast));

    return resultat != SOCKET_ERROR;
}

void M_ExpediteurUDP::transmettreCommande(Expediteur exp, TypeCommande type, Action action, float valeur) {
    // Création du paquet sur la pile et envoi immédiat
    const PaquetControle p{exp, type, action, valeur};
    envoyer(&p, sizeof(p));
}