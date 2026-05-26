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

class M_TFTP_W {
private:
    const int BLOCK_SIZE = 512;
    const int MAX_RETRIES = 5;

public:
    M_TFTP_W();
    ~M_TFTP_W();

    void envoyer(string ipMaster, string cheminJson);
    bool recevoirFichierPousse(const string& fichierLocal);
};