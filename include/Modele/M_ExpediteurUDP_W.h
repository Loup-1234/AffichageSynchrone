#pragma once

#define NOGDI
#define NOUSER

#include <winsock2.h>
#include <string>
#include <cstdint> // Ajouté pour uint8_t et uint32_t

#pragma comment(lib, "ws2_32.lib")

using namespace std;

// --- DÉPLACEMENT DEPUIS LE LECTEUR ---

enum class TypeCommande : uint8_t {
    LECTURE_PAUSE = 0,
    VOLUME = 1,
    PROGRESSION = 2
};

#pragma pack(push, 1)
struct PaquetControle {
    uint32_t signature = 0x5453454D;
    TypeCommande type;
    float valeur;
};
#pragma pack(pop)

// -------------------------------------

class M_ExpediteurUDP_W {
public:
    M_ExpediteurUDP_W(const string& ipGroupe, const int port);
    ~M_ExpediteurUDP_W();

    bool envoyer(const void* donnees, const int taille);

    // Nouvelle méthode déplacée
    void transmettreCommande(TypeCommande type, float valeur);

private:
    SOCKET descripteurSocket = INVALID_SOCKET;
    sockaddr_in adresseDest{};
};