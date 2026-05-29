#pragma once

#include "Modele/M_ProtocoleReseau.h"
#include <string>

#ifdef _WIN32
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    using SocketType = SOCKET;
using SockLenType = int;
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/select.h>
#include <cstring>
using SocketType = int;
using SockLenType = socklen_t;
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#endif

/**
 * @class M_UDP
 * @brief Gestionnaire de socket UDP spécialisé par cas d'usage.
 */
class M_UDP {
public:
    M_UDP();
    ~M_UDP();

    /**
     * @brief Configure le socket uniquement pour l'envoi (Unicast / Broadcast).
     */
    bool initialiserEmetteur(const std::string& ipCible, int portCible);

    /**
     * @brief Configure le socket uniquement pour l'écoute locale (Unicast).
     */
    bool initialiserRecepteur(int portEcoute);

    /**
     * @brief Configure le socket pour écouter et émettre au sein d'un groupe Multicast.
     */
    bool initialiserMulticast(const std::string& ipMulticast, int portPort);

    void fermer();

    // ENVOI
    bool envoyer(const void* donnees, int taille);
    void transmettreCommande(Expediteur exp, TypeCommande type, Action action, float valeur);

    // RÉCEPTION
    bool recevoir(PaquetControle &paquet) const;

    /**
     * @brief Récupère l'adresse IP du dernier émetteur de paquet.
     */
    std::string getIP() const { return derniereIpEmetteur; }

private:
    /**
     * @brief Factorise la création du socket et l'application des options universelles.
     */
    bool preparerSocket();

    SocketType descripteurSocket = INVALID_SOCKET;
    sockaddr_in adresseDestination{};
    ip_mreq groupeMulticast{};
    bool estMulticast = false;
    mutable std::string derniereIpEmetteur = "";
};