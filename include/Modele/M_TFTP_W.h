#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <cstdint>

#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

using namespace std;

/**
 * @class M_TFTP_W
 * @brief Implémente le protocole de transfert de fichiers simplifié TFTP pour l'environnement Windows.
 */
class M_TFTP_W {
private:
    const int BLOCK_SIZE = 512; /**< Taille fixe d'un bloc de données TFTP conforme au standard (512 octets). */
    const int MAX_RETRIES = 5; /**< Nombre de tentatives de réémission maximales en cas de perte de paquet. */

public:
    /**
     * @brief Constructeur initialisant l'utilisation de la bibliothèque Winsock.
     */
    M_TFTP_W();

    /**
     * @brief Destructeur assurant le nettoyage et la désinitialisation de Winsock.
     */
    ~M_TFTP_W();

    /**
     * @brief Émet un fichier local vers un serveur TFTP distant en utilisant une requête WRQ.
     * @param ipMaster Adresse IP de l'hôte récepteur distant.
     * @param cheminJson Chemin complet ou relatif du fichier physique à envoyer.
     */
    void envoyer(string ipMaster, string cheminJson);

    /**
     * @brief Initialise un serveur d'écoute TFTP passif pour stocker un fichier poussé par un client.
     * @param fichierLocal Chemin et nom du fichier de sortie à créer localement.
     * @return true si la totalité du fichier a été reçue et enregistrée, false sinon.
     */
    bool recevoirFichierPousse(const string& fichierLocal);

    /**
     * @brief Serveur d'écoute TFTP dédié au Master, doté d'une gestion intégrée des délais d'attente (timeouts).
     * @param fichierLocal Chemin et nom du fichier de sortie à créer localement.
     * @return true si le transfert s'est correctement exécuté, false en cas de timeout ou d'erreur système.
     */
    bool recevoirFichierPousseMaster(const string& fichierLocal);
};