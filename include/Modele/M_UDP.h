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
 * @brief Version simplifiée avec stockage de la cible à l'initialisation.
 */
class M_UDP {
public:
    M_UDP();
    ~M_UDP();

    /**
     * @brief Initialise le socket UDP avec ses paramètres de communication.
     * @param ipCible Adresse IP de destination (Unicast, Broadcast ou Multicast).
     * @param portCible Port de destination pour l'envoi.
     * @param portEcoute Port local d'écoute (0 si émetteur seul).
     * @param ipMulticast Adresse du groupe multicast à rejoindre (optionnel).
     */
    bool initialiser(const std::string& ipCible = "", int portCible = 0, int portEcoute = 0, const std::string& ipMulticast = "");
    void fermer();

    // ENVOI
    bool envoyer(const void* donnees, int taille);
    void transmettreCommande(Expediteur exp, TypeCommande type, Action action, float valeur);

    // RÉCEPTION
    bool recevoir(PaquetControle &paquet) const;

    /**
     * @brief Récupère l'adresse IP du dernier émetteur de paquet.
     */
    string getIP() const { return derniereIpEmetteur; }

private:
    SocketType descripteurSocket = INVALID_SOCKET;
    sockaddr_in adresseDestination{}; // Stocke la cible configurée
    ip_mreq groupeMulticast{};
    bool estMulticast = false;
    mutable string derniereIpEmetteur = "";
};