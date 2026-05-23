#pragma once

#ifdef _WIN32

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <cstdint>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

using namespace std;

class M_TFTP_W {
private:
    const int TFTP_PORT = 5000;
    const int BLOCK_SIZE = 512;
    const int MAX_RETRIES = 5;

public:
    M_TFTP_W();
    ~M_TFTP_W();

    void envoyer(string ipMaster, string cheminJson);
    bool recevoirFichierPousse(int port, const string& fichierLocal);
};

#endif // _WIN32