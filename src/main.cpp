#include "Vue/V_Master.h"
#include "Modele/M_BDD.h"
#include <iostream>

using namespace std;

int main() {

    // M_BDD maBDD;
    // maBDD.supprimerDonnees("video", "true");
    // maBDD.supprimerDonnees("config_reseau", "true");
    // maBDD.supprimerDonnees("video_complexe", "true");
    // maBDD.supprimerDonnees("musique", "true");

    try {
        // Configuration des parametres de demarrage reseau et d affichage
        const string IP_MULTICAST = "224.0.0.1";
        constexpr int PORT_COMMANDES = 54321;
        constexpr int PORT_DECOUVERTE = 5000;
        constexpr int PORT_REPONSE = 5001;

        const string DOSSIER_VIDEOS = "videos";
        const string CHEMIN_VIDEO_MASTER = "videosComplexes/VideoComplexe_0.mp4";

        // Instanciation unique de l interface graphique Master
        V_Master master(IP_MULTICAST, PORT_COMMANDES, PORT_DECOUVERTE, PORT_REPONSE, DOSSIER_VIDEOS, CHEMIN_VIDEO_MASTER);

        // Lancement effectif du programme de diffusion
        master.executer();
    }
    catch (const exception &e) {
        cerr << "Erreur : " << e.what() << endl;
    }
    return 0;
}