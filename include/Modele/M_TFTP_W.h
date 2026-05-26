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
 * @brief Gestionnaire de transfert de fichiers via le protocole TFTP (Trivial File Transfer Protocol).
 * * Cette classe implémente les fonctions de base pour envoyer ou recevoir des fichiers
 * entre le Master et les clients distants. Elle gère la segmentation des données
 * en blocs (taille fixe) et la logique de retransmission en cas d'échec réseau.
 */
class M_TFTP_W {
public:
    /**
     * @brief Constructeur initialisant l'environnement réseau (WSAStartup pour Windows).
     */
    M_TFTP_W();

    /**
     * @brief Destructeur garantissant la fermeture propre des ressources réseau (WSACleanup).
     */
    ~M_TFTP_W();

    /**
     * @brief Transmet un fichier vers un client distant via TFTP.
     * @param ipMaster Adresse IP de la cible recevant le fichier.
     * @param cheminJson Chemin local du fichier source à envoyer.
     */
    void envoyer(string ipMaster, string cheminJson);

    /**
     * @brief Reçoit un fichier envoyé par un client distant.
     * @param fichierLocal Chemin où enregistrer le fichier reçu localement.
     * @return True si le transfert est complet et intègre, False en cas d'erreur.
     */
    bool recevoirFichierPousse(const string& fichierLocal);

    /**
     * @brief Spécifique au Master : reçoit un fichier poussé par un client distant.
     * @param fichierLocal Chemin de sauvegarde sur le système de fichiers du Master.
     * @return True si la réception réussit.
     */
    bool recevoirFichierPousseMaster(const string& fichierLocal);

private:
    const int BLOCK_SIZE = 512;   ///< Taille standard d'un bloc TFTP (RFC 1350).
    const int MAX_RETRIES = 5;    ///< Nombre maximal de tentatives de retransmission en cas de perte de paquet.
};