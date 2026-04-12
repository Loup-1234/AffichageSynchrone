#pragma once

#include "raylib.h"
#include "../ExpediteurUDP_W/ExpediteurUDP_W.h"

#include <cstdint>

using namespace std;

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

/**
 * @class Master
 * @brief Gère l'interface utilisateur (IHM) et la communication réseau.
 */
class Master {
public:
    Master(const string &ipGroupe, const int port);

    ~Master();

    void executer();

private:
    ExpediteurUDP_W udp;

    float valeurSliderProgression = 0.0f;
    float dureeTotale = 100.0f;
    float valeurSliderSon = 100.0f;
    bool playPause = false;
    bool estMuet = false;
    Rectangle zones[11]{};

    void miseAJourDisposition();

    void afficherListeFichiers();

    void afficherZoneVideo();
};
