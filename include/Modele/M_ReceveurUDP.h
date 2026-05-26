#pragma once

#include "Modele/M_ProtocoleReseau.h"
#include <string>
#include <cstdint>

#ifdef _WIN32
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>
    #include <winsock2.h>
    #include <ws2tcpip.h>
    using SocketType = SOCKET;   ///< Alias système pour les sockets sous Windows.
    using SockLenType = int;     ///< Type standard pour la gestion des longueurs de socket (Windows).
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <sys/select.h>
    using SocketType = int;      ///< Alias système pour les sockets sous systèmes POSIX.
    using SockLenType = socklen_t; ///< Type standard pour la gestion des longueurs de socket (POSIX).
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
#endif

using namespace std;

/**
 * @class M_ReceveurUDP
 * @brief Gestionnaire d'écoute réseau pour la réception de commandes et la découverte de nœuds.
 * * Cette classe gère l'ouverture d'une socket en mode Multicast, permettant à un écran
 * client de recevoir des ordres synchrones ou de répondre à des requêtes de découverte
 * provenant du Master.
 */
class M_ReceveurUDP {
public:
    /**
     * @brief Constructeur initialisant l'écoute sur un port et un groupe Multicast donnés.
     * @param port Port réseau sur lequel écouter les paquets entrants.
     * @param adresseMulticast Adresse IP de groupe Multicast (ex: 239.0.0.1) pour la diffusion.
     */
    M_ReceveurUDP(int port, const string& adresseMulticast);

    /**
     * @brief Destructeur garantissant la fermeture de la socket et le départ du groupe Multicast.
     */
    ~M_ReceveurUDP();

    /**
     * @brief Attend la réception d'un paquet de contrôle structuré.
     * @param paquet Référence vers la structure PaquetControle à remplir avec les données reçues.
     * @return True si la réception a réussi, False en cas d'erreur.
     */
    bool recevoir(PaquetControle &paquet) const;

    /**
     * @brief Reçoit des données brutes avec un délai d'attente (timeout) paramétrable.
     * @param buffer Pointeur vers la zone mémoire où stocker les données.
     * @param tailleMax Taille maximale du buffer alloué.
     * @param ipEmetteur Chaîne de caractères où sera stockée l'adresse IP de l'émetteur.
     * @param timeoutMs Temps d'attente maximum en millisecondes.
     * @return Le nombre d'octets reçus, ou une valeur négative en cas de timeout ou erreur.
     */
    int recevoirAvecTimeout(char* buffer, int tailleMax, string& ipEmetteur, int timeoutMs) const;

    /**
     * @brief Envoie une réponse réseau spécifique vers une cible identifiée.
     * @param ipCible Adresse IP du destinataire.
     * @param portCible Port du destinataire.
     * @param message Contenu textuel ou binaire de la réponse.
     */
    void envoyerReponse(const string& ipCible, int portCible, const string& message);

private:
    SocketType descripteurSocket = INVALID_SOCKET; ///< Descripteur de la socket active.
    ip_mreq groupeMulticast{};                     ///< Structure de configuration pour l'abonnement au groupe Multicast.
};