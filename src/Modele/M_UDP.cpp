#include <string>
#include "Modele/M_UDP.h"

#ifndef _WIN32
#include <fcntl.h>
#endif

M_UDP::M_UDP() {
#ifdef _WIN32
    WSADATA donneesWsa;
    WSAStartup(MAKEWORD(2, 2), &donneesWsa);
#endif
}

M_UDP::~M_UDP() {
    fermer();
#ifdef _WIN32
    WSACleanup();
#endif
}

bool M_UDP::initialiser(const std::string& ipCible, int portCible, int portEcoute, const std::string& ipMulticast) {
    fermer();

    descripteurSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (descripteurSocket == INVALID_SOCKET) return false;

    // 1. Options universelles (le cast const char* fonctionne sur Windows et Linux)
    int opt = 1;
    setsockopt(descripteurSocket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&opt), sizeof(opt));
    setsockopt(descripteurSocket, SOL_SOCKET, SO_BROADCAST, reinterpret_cast<const char*>(&opt), sizeof(opt));

    int ttl = 1;
    setsockopt(descripteurSocket, IPPROTO_IP, IP_MULTICAST_TTL, reinterpret_cast<const char*>(&ttl), sizeof(ttl));

    // 2. Configuration de l'adresse de destination
    if (!ipCible.empty() && portCible > 0) {
        adresseDestination.sin_family = AF_INET;
        adresseDestination.sin_port = htons(portCible);
        inet_pton(AF_INET, ipCible.c_str(), &adresseDestination.sin_addr);
    }

    // 3. Liaison pour l'écoute locale (Bind)
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
        if (setsockopt(descripteurSocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, reinterpret_cast<const char*>(&groupeMulticast), sizeof(groupeMulticast)) >= 0) {
            estMulticast = true;
        }
    }

    // 5. Activation du mode NON-BLOQUANT (Windows vs Linux)
#ifdef _WIN32
    u_long mode = 1;
    ioctlsocket(descripteurSocket, FIONBIO, &mode);
#else
    int flags = fcntl(descripteurSocket, F_GETFL, 0);
    if (flags != -1) {
        fcntl(descripteurSocket, F_SETFL, flags | O_NONBLOCK);
    }
#endif

    return true;
}

void M_UDP::fermer() {
    if (descripteurSocket != INVALID_SOCKET) {
        if (estMulticast) {
            setsockopt(descripteurSocket, IPPROTO_IP, IP_DROP_MEMBERSHIP, reinterpret_cast<const char*>(&groupeMulticast), sizeof(groupeMulticast));
            estMulticast = false;
        }
#ifdef _WIN32
        closesocket(descripteurSocket);
#else
        close(descripteurSocket);
#endif
        descripteurSocket = INVALID_SOCKET;
    }
}

bool M_UDP::envoyer(const void* donnees, const int taille) {
    if (descripteurSocket == INVALID_SOCKET || donnees == nullptr || taille <= 0) return false;

    int resultat = sendto(descripteurSocket, static_cast<const char*>(donnees), taille, 0,
                          reinterpret_cast<const sockaddr*>(&adresseDestination), sizeof(adresseDestination));
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

    if (resultat == sizeof(PaquetControle)) {
        char ipTexte[INET_ADDRSTRLEN]{};

        if (inet_ntop(AF_INET, &(source.sin_addr), ipTexte, INET_ADDRSTRLEN) != nullptr) {
            derniereIpEmetteur = ipTexte;
        }
        return true;
    }

    return false;
}