#include "Vue/V_Master.h"
#include <iostream>
#include <vector>
#include <string>

using namespace std;

/**
 * @file main.cpp
 * @brief Point d'entrée principal du logiciel Master.
 * * Ce fichier configure les paramètres réseau (IP Multicast et Port) ainsi que
 * la liste des lecteurs physiques cibles avant de lancer l'interface utilisateur.
 */

int main() {
    try {
        // Configuration de l'adresse de multidiffusion (Multicast)
        // Les adresses valides sont comprises entre 224.0.0.0 et 239.255.255.255
        const string IP_MULTICAST = "224.0.0.1";

        // Port utilisé pour l'envoi des commandes UDP synchronisées
        constexpr int PORT_RESEAU = 54321;

        /**
         * Configuration des lecteurs (LecteurSpec) :
         * Structure : { "ID", "Adresse IP", "Nombre de vidéos maximum" }
         * - ID "0" : Correspond généralement au Master local.
         * - IP : Adresse IP du client distant (utilisez 127.0.0.1 pour des tests locaux).
         * - Nb Vidéos : Définit combien de vidéos le moteur FFmpeg doit compiler pour ce client.
         */
        const vector<vector<string> > specLecteurs = {
            {"0", "", "2"},           // Lecteur local (Master)
            {"1", "127.0.0.1", "2"},  // Test sur la même machine
            {"2", "172.31.71.104", "2"} // Client distant réel
        };

        // Instanciation de la vue principale (qui gère l'initialisation de Raylib et du Contrôleur)
        V_Master master(IP_MULTICAST, PORT_RESEAU, specLecteurs);

        // Lancement de la boucle de rendu et de gestion des événements
        master.executer();

    } catch (const exception &e) {
        // Capture et affichage des erreurs fatales (échec init VLC, sockets, etc.)
        cerr << "[Erreur Critique] : " << e.what() << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}