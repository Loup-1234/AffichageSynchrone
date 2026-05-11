#include "Vue/V_Master.h"
#include <iostream>
#include <vector>
#include <string>

using namespace std;

int main() {
    try {
        // IP_MULTICAST doit être entre 224.0.0.0 et 239.255.255.255
        const string IP_MULTICAST = "224.0.0.1";
        constexpr int PORT_RESEAU = 54321;

        /*
        const vector<vector<string> > specLecteurs = {
            {"0", "172.31.71.104", "2"},
            {"1", "0.0.0.0", "3"},
            {"2", "0.0.0.0", "3"}
        };

        const vector<vector<string> > specLecteurs = {
            {"0", "172.31.71.104", "3"},
        };
        */

        const vector<vector<string> > specLecteurs = {
            {"0", "", "2"},
            {"1", "127.0.0.1", "2"},
            {"2", "172.31.71.104", "2"}
        };

        V_Master master(IP_MULTICAST, PORT_RESEAU, specLecteurs);
        master.executer();
    } catch (const exception &e) {
        cerr << "[Erreur Critique] : " << e.what() << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
