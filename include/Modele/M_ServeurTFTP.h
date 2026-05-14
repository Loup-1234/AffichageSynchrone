#pragma once

#include <string>
#include <vector>

#ifdef _WIN32
    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    using SocketType = SOCKET;
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
using SocketType = int;
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#endif

#define BLOCK_SIZE 512 ///< Taille standard d'un bloc de données TFTP.

/**
 * @struct TransferInfo
 * @brief Regroupe les informations nécessaires pour un transfert individuel.
 */
struct TransferInfo {
    std::string ip;   ///< Adresse IP du destinataire.
    std::string path; ///< Chemin relatif du fichier à envoyer.
};

/**
 * @enum OPCODE
 * @brief Codes d'opération du protocole TFTP (RFC 1350).
 */
enum OPCODE {
    WRQ = 2,  ///< Write Request (Requête d'écriture).
    DATA = 3, ///< Data packet (Paquet de données).
    ACK = 4,  ///< Acknowledgment (Accusé de réception).
    ERR = 5   ///< Error (Message d'erreur).
};

/**
 * @enum TransferStatus
 * @brief États de sortie possibles d'une opération de transfert.
 */
enum class TransferStatus {
    SUCCESS,               ///< Le transfert s'est terminé avec succès.
    LOCAL_FILE_NOT_FOUND,  ///< Le fichier source est introuvable.
    TRANSFER_SOCKET_ERROR, ///< Erreur lors de la création ou config du socket.
    TIMEOUT_ERROR,         ///< Le destinataire n'a pas répondu (ACK manquant).
    NOT_IMPLEMENTED_ON_OS  ///< Fonctionnalité non disponible sur cet OS.
};

/**
 * @class M_ServeurTFTP
 * @brief Gère l'envoi de fichiers vers plusieurs clients via le protocole TFTP.
 */
class M_ServeurTFTP {
public:
    /**
     * @brief Constructeur du serveur.
     * @param transfers Liste des couples IP/Fichier à traiter.
     * @param docRoot Dossier racine contenant les fichiers vidéos.
     */
    M_ServeurTFTP(const std::vector<TransferInfo>& transfers, const std::string& docRoot);

    /**
     * @brief Nettoie les ressources réseau.
     */
    ~M_ServeurTFTP();

    /**
     * @brief Lance tous les transferts en parallèle de manière asynchrone.
     */
    void runAllTransfers();

private:
    std::string documentRoot;           ///< Chemin racine pour la lecture des fichiers.
    std::vector<TransferInfo> transferData; ///< Liste des tâches de transfert.

    /**
     * @brief Initialise la pile réseau (spécifique à Windows).
     */
    void initNetwork();

    /**
     * @brief Effectue un transfert TFTP complet vers une destination.
     * @param ip Adresse IP cible.
     * @param filePath Chemin complet du fichier source.
     * @return Statut final du transfert.
     */
    TransferStatus sendTftpTransfer(const std::string& ip, const std::string& filePath);
};