/**
 * @file M_TFTP.h
 * @brief Déclaration de la classe M_TFTP pour le transfert de fichiers via le protocole TFTP sous Windows.
 * @author Robin Manceau
 * @date 2026
 */

#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <cstdint>
#include <thread>

#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

using namespace std;

/**
 * @class M_TFTP
 * @brief Implémente le protocole de transfert de fichiers simplifié TFTP pour l'environnement Windows.
 * * Cette classe prend en charge l'émission (requêtes WRQ), la réception séquentielle bloquante
 * et la réception simultanée multi-threadée pour la centralisation de fichiers.
 */
class M_TFTP {
public:
    /**
     * @brief Constructeur par défaut.
     * * Initialise la bibliothèque Winsock (WSAStartup) indispensable pour ouvrir des sockets sous Windows.
     */
    M_TFTP();

    /**
     * @brief Destructeur.
     * * Assure la fermeture des connexions résiduelles et la désinitialisation propre de Winsock (WSACleanup).
     */
    ~M_TFTP();

    /**
     * @brief Émet un fichier local vers un serveur TFTP distant en utilisant une requête d'écriture WRQ.
     * @param ipMaster Adresse IP de l'hôte récepteur distant (le serveur TFTP).
     * @param cheminJson Chemin complet ou relatif du fichier physique à envoyer (ex: configuration JSON).
     */
    void envoyer(string ipMaster, string cheminJson);

    /**
     * @brief Initialise un serveur d'écoute TFTP passif pour stocker un fichier poussé par un client (Monoclient/Bloquant).
     * @param fichierLocal Chemin et nom du fichier de sortie à créer localement pour enregistrer les données reçues.
     * @return true si la totalité du fichier a été reçue, acquittée et enregistrée avec succès, false sinon.
     */
    bool recevoirFichierPousse(const string &fichierLocal);

    /**
     * @brief Serveur d'écoute TFTP dédié au Master, doté d'une gestion intégrée des délais d'attente (timeouts) (Monoclient/Bloquant).
     * @param fichierLocal Chemin et nom du fichier de sortie à créer localement.
     * @return true si le transfert s'est correctement exécuté, false en cas de dépassement de délai (timeout) ou d'erreur réseau.
     */
    bool recevoirFichierPousseMaster(const string &fichierLocal);

    /**
     * @brief Lance un serveur d'écoute TFTP permanent sur le port standard (69) capable de recevoir plusieurs fichiers simultanément.
     * * Chaque nouvelle requête d'écriture (WRQ) valide détectée déclenche l'exécution asynchrone d'un thread autonome.
     * @param dossierCible Chemin du dossier local destiné à accueillir l'ensemble des fichiers collectés.
     */
    void demarrerServeurMultiThread(const string &dossierCible);

private:
    const int BLOCK_SIZE = 512; ///< Taille fixe d'un bloc de données TFTP conforme à la RFC 1350 (512 octets).
    const int MAX_RETRIES = 5;  ///< Nombre maximal de tentatives de réémission en cas de perte de paquet ou d'absence d'ACK.

    /**
     * @brief Tâche d'arrière-plan (Thread) dédiée à la réception des blocs de données pour un client spécifique.
     * * Cette fonction négocie et utilise un port éphémère distinct pour le transfert effectif des données,
     * libérant immédiatement le port principal 69 pour intercepter d'autres requêtes entrantes.
     * @param clientAddr Structure sockaddr_in contenant l'adresse réseau (IP et port) du client distant.
     * @param nomFichier Nom d'origine du fichier extrait de la requête WRQ.
     * @param dossierCible Chemin du dossier local où le fichier reçu sera écrit et stocké.
     */
    void recevoirFichierThread(sockaddr_in clientAddr, string nomFichier, string dossierCible);
};