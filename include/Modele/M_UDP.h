#pragma once

#include <cstdint>
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
using SocketType = int;
using SockLenType = socklen_t;
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#endif

using namespace std;

/**
 * @enum Expediteur
 * @brief Identifie l'origine de la commande réseau.
 */
enum class Expediteur : uint8_t {
    MASTER = 0, ///< Instance principale contrôlant les autres.
    AUTRE = 1   ///< Autres types d'expéditeurs possibles.
};

/**
 * @enum TypeCommande
 * @brief Définit la nature du message UDP.
 */
enum class TypeCommande : uint8_t {
    ORDRE = 0,      ///< Commande d'action immédiate.
    CONNECTION = 1, ///< Message lié à l'établissement de la connexion.
};

/**
 * @enum Action
 * @brief Liste des commandes de contrôle de lecture disponibles.
 */
enum class Action : uint8_t {
    PLAY = 0,
    PAUSE = 1,
    STOP = 2,
    VOLUME = 3,
    PROGRESSION = 4,
    VITESSE = 5,
};

#pragma pack(push, 1)
/**
 * @struct PaquetControle
 * @brief Structure de données compacte envoyée sur le réseau.
 * L'alignement est forcé à 1 octet pour garantir que la structure
 * a la même taille sur toutes les plateformes (7 octets au total).
 */
struct PaquetControle {
    Expediteur exp;    ///< Qui envoie la commande.
    TypeCommande type; ///< Type de message.
    Action action;     ///< Action à réaliser.
    float valeur;      ///< Valeur associée (volume, position temporelle, etc.).
};
#pragma pack(pop)

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