#pragma once

#define NOGDI
#define NOUSER

#include <winsock2.h>
#include <string>
#include <cstdint>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

enum class TypeCommande : uint8_t {
    LECTURE_PAUSE = 0,
    VOLUME = 1,
    PROGRESSION = 2
};

#pragma pack(push, 1)
struct PaquetControle {
    TypeCommande type;
    float valeur;
};
#pragma pack(pop)

class M_ExpediteurUDP_W {
public:
    M_ExpediteurUDP_W(const string &ipGroupe, int port);

    ~M_ExpediteurUDP_W();

    bool envoyer(const void *donnees, int taille);

    void transmettreCommande(TypeCommande type, float valeur);

private:
    SOCKET descripteurSocket = INVALID_SOCKET;
    sockaddr_in adresseDest{};
};
