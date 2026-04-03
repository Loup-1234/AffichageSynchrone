#pragma once

#include "raylib.h"
#include <string>
#include <vector>

using namespace std;

/**
 * @class Master
 * @brief Gère uniquement l'interface utilisateur (IHM) et la structure visuelle.
 * Les fonctionnalités logiques sont destinées à être surchargées.
 */
class Master {
public:
    Master();
    ~Master();

    void executer();

private:
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