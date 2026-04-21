#pragma once

#define NOGDI
#define NOUSER

#include <winsock2.h>
#include <string>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

class M_ExpediteurUDP_W {
public:
    M_ExpediteurUDP_W(const string& ipGroupe, const int port);

    ~M_ExpediteurUDP_W();

    bool envoyer(const void* donnees, const int taille);

private:
    SOCKET descripteurSocket = INVALID_SOCKET;
    sockaddr_in adresseDest{};
};