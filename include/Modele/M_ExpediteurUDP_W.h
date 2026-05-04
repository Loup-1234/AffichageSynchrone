#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <winsock2.h>
#include <string>
#include <cstdint>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

enum class Expediteur : uint8_t { MASTER = 0, AUTRE = 1 };
enum class TypeCommande : uint8_t { ORDRE = 0, CONNECTION = 1 };
enum class Action : uint8_t { PLAY = 0, PAUSE = 1, STOP = 2, VOLUME = 3, PROGRESSION = 4, VITESSE = 5 };

#pragma pack(push, 1)
struct PaquetControle {
    Expediteur exp;
    TypeCommande type;
    Action action;
    float valeur;
};
#pragma pack(pop)

class M_ExpediteurUDP_W {
public:
    // ipMulticast doit être entre 224.0.0.0 et 239.255.255.255
    M_ExpediteurUDP_W(const string &ipMulticast, int port);
    ~M_ExpediteurUDP_W();

    bool envoyer(const void *donnees, int taille);
    void transmettreCommande(const Expediteur exp, const TypeCommande type, const Action action, const float valeur);

private:
    SOCKET descripteurSocket = INVALID_SOCKET;
    sockaddr_in adresseDest{};
};