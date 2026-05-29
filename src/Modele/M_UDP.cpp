#include "Modele/M_UDP.h"

M_UDP::M_UDP() {
#ifdef _WIN32
    WSADATA donneesWsa;
    WSAStartup(MAKEWORD(2, 2), &donneesWsa);
#endif
}

M_UDP::~M_UDP() {
    fermer();
}

bool M_UDP::initialiser(const std::string& ipCible, int portCible, int portEcoute, const std::string& ipMulticast) {
    fermer();

    descripteurSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (descripteurSocket == INVALID_SOCKET) return false;

    // 1. Options universelles
    int opt = 1;
    setsockopt(descripteurSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
    setsockopt(descripteurSocket, SOL_SOCKET, SO_BROADCAST, (const char*)&opt, sizeof(opt));

    int ttl = 1;
    setsockopt(descripteurSocket, IPPROTO_IP, IP_MULTICAST_TTL, (const char*)&ttl, sizeof(ttl));

    // 2. Configuration de l'adresse de destination (si fournie)
    if (!ipCible.empty() && portCible > 0) {
        adresseDestination.sin_family = AF_INET;
        adresseDestination.sin_port = htons(portCible);
        inet_pton(AF_INET, ipCible.c_str(), &adresseDestination.sin_addr);
    }

    // 3. Liaison pour l'écoute locale
    if (portEcoute > 0) {
        sockaddr_in adresseLocale{};
        adresseLocale.sin_family = AF_INET;
        adresseLocale.sin_port = htons(portEcoute);
        adresseLocale.sin_addr.s_addr = INADDR_ANY;

        if (bind(descripteurSocket, reinterpret_cast<sockaddr*>(&adresseLocale), sizeof(adresseLocale)) == SOCKET_ERROR) {
            fermer();
            return false;
        }
    }

    // 4. Abonnement Multicast
    if (!ipMulticast.empty()) {
        inet_pton(AF_INET, ipMulticast.c_str(), &groupeMulticast.imr_multiaddr);
        groupeMulticast.imr_interface.s_addr = INADDR_ANY;
        if (setsockopt(descripteurSocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (const char*)&groupeMulticast, sizeof(groupeMulticast)) >= 0) {
            estMulticast = true;
        }
    }

#ifdef _WIN32
    u_long mode = 1;
    ioctlsocket(descripteurSocket, FIONBIO, &mode);
#endif

    return true;
}

void M_UDP::fermer() {
    if (descripteurSocket != INVALID_SOCKET) {
        if (estMulticast) {
            setsockopt(descripteurSocket, IPPROTO_IP, IP_DROP_MEMBERSHIP, (const char*)&groupeMulticast, sizeof(groupeMulticast));
            estMulticast = false;
        }
#ifdef _WIN32
        closesocket(descripteurSocket);
        WSACleanup();
#else
        close(descripteurSocket);
#endif
        descripteurSocket = INVALID_SOCKET;
    }
}

bool M_UDP::envoyer(const void *donnees, const int taille) {
    if (descripteurSocket == INVALID_SOCKET) return false;

    int resultat = sendto(descripteurSocket, static_cast<const char *>(donnees), taille, 0,
                          reinterpret_cast<sockaddr *>(&adresseDestination), sizeof(adresseDestination));
    return resultat != SOCKET_ERROR;
}

void M_UDP::transmettreCommande(Expediteur exp, TypeCommande type, Action action, float valeur) {
    const PaquetControle p{exp, type, action, valeur};
    envoyer(&p, sizeof(p));
}

bool M_UDP::recevoir(PaquetControle &paquet) const {
    if (descripteurSocket == INVALID_SOCKET) return false;

    sockaddr_in source{};
    SockLenType tailleSource = sizeof(source);
    int resultat = recvfrom(descripteurSocket, reinterpret_cast<char*>(&paquet), sizeof(PaquetControle), 0,
                            reinterpret_cast<sockaddr*>(&source), &tailleSource);

    return (resultat == sizeof(PaquetControle));
}

int M_UDP::recevoirAvecTimeout(char* buffer, int tailleMax, std::string& ipEmetteur, int timeoutMs) const {
    if (descripteurSocket == INVALID_SOCKET) return -1;

    fd_set ensemble;
    FD_ZERO(&ensemble);
    FD_SET(descripteurSocket, &ensemble);

    timeval tv{ timeoutMs / 1000, (timeoutMs % 1000) * 1000 };
    if (select(static_cast<int>(descripteurSocket) + 1, &ensemble, nullptr, nullptr, &tv) <= 0) return 0;

    sockaddr_in source{};
    SockLenType tailleSource = sizeof(source);
    int nbOctets = recvfrom(descripteurSocket, buffer, tailleMax, 0, reinterpret_cast<sockaddr*>(&source), &tailleSource);

    if (nbOctets > 0) {
        char ipStr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(source.sin_addr), ipStr, INET_ADDRSTRLEN);
        ipEmetteur = std::string(ipStr);
    }
    return nbOctets;
}