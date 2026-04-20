#include "Vue/V_Master.h"
#include <iostream>
#include <vector>
#include <string>

using namespace std;

int main() {
    try {
        const string IP_RESEAU = "127.0.0.1";
        constexpr int PORT_RESEAU = 5004;

        const vector<vector<string> > specLecteurs = {
            {"0", "172.31.71.102", "2"},
            {"1", "0.0.0.0", "3"},
            {"2", "0.0.0.0", "3"}
        };

        V_Master master(IP_RESEAU, PORT_RESEAU, specLecteurs);
        master.executer();
    } catch (const exception &e) {
        cerr << "[Erreur Critique] : " << e.what() << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
