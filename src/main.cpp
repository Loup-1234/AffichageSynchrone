#include "Vue/V_Master.h"
#include <iostream>

using namespace std;

int main() {
    try {
        const string IP_MULTICAST = "224.0.0.1";
        constexpr int PORT_COMMANDES = 54321;
        constexpr int PORT_DECOUVERTE = 5000;
        constexpr int PORT_REPONSE = 5001;

        const string DOSSIER_VIDEOS = "videos";
        const string CHEMIN_VIDEO_MASTER = "videosComplexes/VideoComplexe_0.mp4";

        V_Master master(IP_MULTICAST, PORT_COMMANDES, PORT_DECOUVERTE, PORT_REPONSE, DOSSIER_VIDEOS, CHEMIN_VIDEO_MASTER);

        master.executer();
    }
    catch (const exception &e) {
        cerr << "Erreur : " << e.what() << endl;
    }
    return 0;
}