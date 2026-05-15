#include "Vue/V_Master.h"
#include <iostream>
#include <vector>
#include <string>

using namespace std;

int main() {
    try {
        // Une seule IP pour tous les flux Multicast
        const string IP_MULTICAST = "239.0.0.1";

        // Les ports permettent de différencier les services
        constexpr int PORT_COMMANDES = 54321;
        constexpr int PORT_DECOUVERTE = 5000;
        constexpr int PORT_REPONSE = 5001;

        const vector<vector<string> > specLecteurs = {
            {"0", "", "2"}, // Lecteur local (Master)
            {"1", "127.0.0.1", "2"}, // Test sur la même machine
            {"2", "172.31.71.104", "2"} // Client distant réel
        };

        // Instanciation de la vue principale (qui gère l'initialisation de Raylib et du Contrôleur)
        V_Master master(IP_MULTICAST, PORT_COMMANDES, PORT_DECOUVERTE, PORT_REPONSE, specLecteurs);

        // Lancement de la boucle de rendu et de gestion des événements
        master.executer();
    } catch (const exception &e) {
        // Capture et affichage des erreurs fatales (échec init VLC, sockets, etc.)
        cerr << "[Erreur Critique] : " << e.what() << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
