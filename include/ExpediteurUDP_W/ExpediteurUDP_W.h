#pragma once

#define NOGDI
#define NOUSER

#include <winsock2.h>
#include <string>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

class ExpediteurUDP_W {
public:
    ExpediteurUDP_W(const string& ipGroupe, const int port);

    ~ExpediteurUDP_W();

    bool Envoyer(const void* donnees, const int taille);

private:
    SOCKET descripteurSocket = INVALID_SOCKET;
    sockaddr_in adresseDest{};
};