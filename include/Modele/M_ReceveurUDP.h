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

/**
 * @class M_ReceveurUDP
 * @brief Gère la réception de paquets de contrôle et de messages UDP Multicast ou Unicast.
 */
class M_ReceveurUDP {
public:
    /**
     * @brief Constructeur configurant le socket de réception et rejoignant un groupe multicast.
     * @param port Port local d'écoute.
     * @param adresseMulticast Adresse IP du groupe multicast à rejoindre.
     */
    M_ReceveurUDP(int port, const string& adresseMulticast);

    /**
     * @brief Destructeur quittant le groupe multicast et fermant le socket d'écoute.
     */
    ~M_ReceveurUDP();

    /**
     * @brief Reçoit un paquet de contrôle structuré (mode non bloquant).
     * @param paquet Référence vers la structure à remplir avec les données reçues.
     * @return true si un paquet valide a été lu, false sinon.
     */
    bool recevoir(PaquetControle &paquet) const;

    /**
     * @brief Reçoit des données textuelles ou brutes avec une limite de temps d'attente (timeout).
     * @param buffer Pointeur vers le tableau de caractères d'accueil.
     * @param tailleMax Capacité maximale du buffer.
     * @param ipEmetteur [out] Chaîne recevant l'adresse IP de la source du paquet.
     * @param timeoutMs Durée d'attente maximale en millisecondes.
     * @return Le nombre d'octets reçus, 0 en cas de timeout, ou -1 en cas d'erreur.
     */
    int recevoirAvecTimeout(char* buffer, int tailleMax, string& ipEmetteur, int timeoutMs) const;

    /**
     * @brief Envoie une réponse UDP directe (Unicast) à un destinataire ciblé.
     * @param ipCible Adresse IP de l'hôte cible.
     * @param portCible Port applicatif cible.
     * @param message Chaîne de caractères à transmettre.
     */
    void envoyerReponse(const string& ipCible, int portCible, const string& message);

private:
    SocketType descripteurSocket = INVALID_SOCKET; /**< Descripteur système du socket réseau. */
    ip_mreq groupeMulticast{}; /**< Structure de gestion de l'abonnement au groupe Multicast. */
};