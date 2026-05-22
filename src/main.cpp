#include "Vue/V_Master.h"
#include "Modele/M_ProtocoleReseau.h"
#include <iostream>
#include <vector>

using namespace std;

int main() {
    try {
        const string IP_MULTICAST = "224.0.0.1";
        constexpr int PORT_COMMANDES = 54321;
        constexpr int PORT_DECOUVERTE = 5000;
        constexpr int PORT_REPONSE = 5001;

        const string DOSSIER_VIDEOS = "videos";
        const string CHEMIN_VIDEO_MASTER = "videosComplexes/VideoComplexe_0.mp4";

        // Déclaration explicite et fortement typée de notre infrastructure réseau
        const vector<LecteurConfig> configurationReseau = {
            {0, "127.0.0.1", 2}, // Master local (ID 0)
            {1, "127.0.0.1", 2}, // Instance test locale (ID 1)
            {2, "172.31.71.104", 2} // Client physique distant realisé (ID 2)
        };

        V_Master master(IP_MULTICAST, PORT_COMMANDES, PORT_DECOUVERTE, PORT_REPONSE,
                         configurationReseau, DOSSIER_VIDEOS, CHEMIN_VIDEO_MASTER);

        master.executer();
    }
    catch (const exception &e) {
        cerr << "[Erreur Critique] : " << e.what() << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}