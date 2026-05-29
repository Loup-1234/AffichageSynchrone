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
 * Prend en charge l'émission, la réception séquentielle et la réception simultanée multi-threadée.
 */
class M_TFTP {
private:
    const int BLOCK_SIZE = 512; /**< Taille fixe d'un bloc de données TFTP conforme au standard (512 octets). */
    const int MAX_RETRIES = 5;  /**< Nombre de tentatives de réémission maximales en cas de perte de paquet. */

    /**
     * @brief Tâche d'arrière-plan (Thread) dédiée à la réception des blocs de données pour un client spécifique.
     * Cette fonction utilise un port éphémère pour libérer le port principal d'écoute.
     * @param clientAddr Structure contenant l'adresse réseau (IP et port) du client distant.
     * @param nomFichier Nom d'origine du fichier extrait de la requête WRQ.
     * @param dossierCible Chemin du dossier local où le fichier reçu sera enregistré.
     */
    void recevoirFichierThread(sockaddr_in clientAddr, string nomFichier, string dossierCible);

public:
    /**
     * @brief Constructeur initialisant l'utilisation de la bibliothèque Winsock.
     */
    M_TFTP();

    /**
     * @brief Destructeur assurant le nettoyage et la désinitialisation de Winsock.
     */
    ~M_TFTP();

    /**
     * @brief Émet un fichier local vers un serveur TFTP distant en utilisant une requête WRQ.
     * @param ipMaster Adresse IP de l'hôte récepteur distant.
     * @param cheminJson Chemin complet ou relatif du fichier physique à envoyer.
     */
    void envoyer(string ipMaster, string cheminJson);

    /**
     * @brief Initialise un serveur d'écoute TFTP passif pour stocker un fichier poussé par un client (Monoclient/Bloquant).
     * @param fichierLocal Chemin et nom du fichier de sortie à créer localement.
     * @return true si la totalité du fichier a été reçue et enregistrée, false sinon.
     */
    bool recevoirFichierPousse(const string& fichierLocal);

    /**
     * @brief Serveur d'écoute TFTP dédié au Master, doté d'une gestion intégrée des délais d'attente (timeouts) (Monoclient/Bloquant).
     * @param fichierLocal Chemin et nom du fichier de sortie à créer localement.
     * @return true si le transfert s'est correctement exécuté, false en cas de timeout ou d'erreur système.
     */
    bool recevoirFichierPousseMaster(const string& fichierLocal);

    /**
     * @brief Lance un serveur d'écoute TFTP permanent sur le port 69 capable de recevoir plusieurs fichiers simultanément.
     * Chaque nouvelle requête d'écriture (WRQ) valide déclenche l'exécution d'un thread autonome.
     * @param dossierCible Chemin du dossier local destiné à accueillir l'ensemble des fichiers collectés.
     */
    void demarrerServeurMultiThread(const string& dossierCible);
};