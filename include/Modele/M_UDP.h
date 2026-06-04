/**
 * @file M_UDP.h
 * @brief Déclaration de la classe M_UDP et des structures associées pour la communication réseau.
 * @author Robin Manceau et Alain Humeau
 * @date 2026
 */

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
/** @brief Alias pour le type de socket sous Windows. */
using SocketType = SOCKET;
/** @brief Alias pour la taille des structures de socket sous Windows. */
using SockLenType = int;
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/select.h>
/** @brief Alias pour le type de socket sous systèmes POSIX. */
using SocketType = int;
/** @brief Alias pour la taille des structures de socket sous systèmes POSIX. */
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
    AUTRE = 1 ///< Autres types d'expéditeurs possibles.
};

/**
 * @enum TypeCommande
 * @brief Définit la nature du message UDP.
 */
enum class TypeCommande : uint8_t {
    ORDRE = 0, ///< Commande d'action immédiate.
    CONNECTION = 1, ///< Message lié à l'établissement de la connexion.
};

/**
 * @enum Action
 * @brief Liste des commandes de contrôle de lecture disponibles.
 */
enum class Action : uint8_t {
    PLAY = 0, ///< Lancer ou reprendre la lecture.
    PAUSE = 1, ///< Mettre la lecture en pause.
    STOP = 2, ///< Arrêter la lecture et réinitialiser la position.
    VOLUME = 3, ///< Modifier le niveau sonore.
    PROGRESSION = 4, ///< Repositionner le curseur temporel.
    VITESSE = 5, ///< Ajuster la vitesse d'exécution.
};

#pragma pack(push, 1)
/**
 * @struct PaquetControle
 * @brief Structure de données compacte envoyée sur le réseau.
 * * L'alignement est forcé à 1 octet pour garantir que la structure
 * a la même taille sur toutes les plateformes (7 octets au total).
 */
struct PaquetControle {
    Expediteur exp; ///< Qui envoie la commande.
    TypeCommande type; ///< Type de message.
    Action action; ///< Action à réaliser.
    float valeur; ///< Valeur associée (volume, position temporelle, etc.).
};
#pragma pack(pop)

/**
 * @class M_UDP
 * @brief Gestionnaire de socket UDP spécialisé par cas d'usage (Émission, Réception, Multicast).
 * * Cette classe encapsule la complexité de l'API des sockets réseau (Winsock/POSIX)
 * pour offrir une interface unifiée de transport de paquets de contrôle.
 */
class M_UDP {
public:
    /**
     * @brief Constructeur par défaut. Initialise les variables internes.
     */
    M_UDP();

    /**
     * @brief Destructeur assurant la fermeture du socket et la libération des ressources.
     */
    ~M_UDP();

    /**
     * @brief Configure le socket uniquement pour l'envoi (Unicast / Broadcast).
     * @param ipCible Adresse IP de destination (ex: "192.168.1.255" pour du broadcast).
     * @param portCible Port distant sur lequel envoyer les paquets.
     * @return true si la configuration et la résolution d'adresse ont réussi, false sinon.
     */
    bool initialiserEmetteur(const std::string &ipCible, int portCible);

    /**
     * @brief Configure le socket uniquement pour l'écoute locale (Unicast).
     * @param portEcoute Port local sur lequel le socket doit se lier (bind).
     * @return true si le socket a été correctement lié, false sinon.
     */
    bool initialiserRecepteur(int portEcoute);

    /**
     * @brief Configure le socket pour écouter et émettre au sein d'un groupe Multicast.
     * @param ipMulticast Adresse IP du groupe multicast (ex: "239.0.0.1").
     * @param portPort Port local et distant utilisé pour l'échange de données.
     * @return true si l'abonnement au groupe multicast a réussi, false sinon.
     */
    bool initialiserMulticast(const std::string &ipMulticast, int portPort);

    /**
     * @brief Ferme proprement le socket ouvert et réinitialise le descripteur.
     */
    void fermer();

    /**
     * @brief Envoie un bloc de données brutes via le socket.
     * @param donnees Pointeur vers la zone mémoire à transmettre.
     * @param taille Nombre d'octets à envoyer.
     * @return true si la totalité des données a été envoyée, false en cas d'erreur.
     */
    bool envoyer(const void *donnees, int taille);

    /**
     * @brief Construit et transmet instantanément un paquet de contrôle structuré.
     * @param exp Identifiant de l'émetteur (Master/Autre).
     * @param type Nature du message (Ordre/Connexion).
     * @param action Commande de lecture ciblée.
     * @param valeur Donnée numérique d'accompagnement (ex: 75.0f pour le volume).
     */
    void transmettreCommande(Expediteur exp, TypeCommande type, Action action, float valeur);

    /**
     * @brief Tente de réceptionner un paquet de contrôle (bloquant ou non selon la configuration).
     * @param[out] paquet Référence vers la structure à remplir avec les données reçues.
     * @return true si un paquet valide de la bonne taille a été intercepté, false sinon.
     */
    bool recevoir(PaquetControle &paquet) const;

    /**
     * @brief Récupère l'adresse IP du dernier émetteur de paquet.
     * @return Chaîne de caractères contenant l'adresse IP (ex: "192.168.1.15").
     */
    std::string getIP() const { return derniereIpEmetteur; }

private:
    /**
     * @brief Factorise la création du socket et l'application des options universelles.
     * * Crée le descripteur et configure les options de réutilisation de port et de diffusion.
     * @return true si le socket est prêt, false en cas d'échec d'allocation système.
     */
    bool preparerSocket();

    SocketType descripteurSocket = INVALID_SOCKET; ///< Descripteur système du socket réseau.
    sockaddr_in adresseDestination{}; ///< Structure d'adresse internet pour la cible des envois.
    ip_mreq groupeMulticast{}; ///< Structure de configuration pour l'abonnement multicast.
    bool estMulticast = false; ///< Indicateur d'activation du mode multicast.
    mutable std::string derniereIpEmetteur = ""; ///< Stockage de l'adresse IP de la dernière source reçue.
};
