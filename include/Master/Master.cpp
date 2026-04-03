#define RAYGUI_IMPLEMENTATION
#include "Master.h"
#include "../include/raygui/raygui.h"

Master::Master() {
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 450, "Master UI - Interface Seule");
    SetWindowMinSize(800, 450);

    miseAJourDisposition();
}

Master::~Master() {
    CloseWindow();
}

void Master::executer() {
    while (!WindowShouldClose()) {
        if (IsWindowResized()) miseAJourDisposition();

        BeginDrawing();
        ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

        afficherZoneVideo();
        afficherListeFichiers();

        // --- Boutons Latéraux ---
        GuiButton(zones[1], "GÉNÉRER");
        GuiButton(zones[10], "DOSSIER");

        // --- Barre de contrôle ---
        if (GuiButton(zones[4], playPause ? "#132#" : "#131#")) {
            playPause = !playPause;
        }

        GuiSliderBar(zones[6], "", nullptr, &valeurSliderProgression, 0.0f, dureeTotale);

        if (GuiButton(zones[7], estMuet ? "#128#" : "#122#")) {
            estMuet = !estMuet;
        }

        GuiSliderBar(zones[9], "", nullptr, &valeurSliderSon, 0.0f, 100.0f);

        EndDrawing();
    }
}

void Master::miseAJourDisposition() {
    const auto L = static_cast<float>(GetScreenWidth());
    const auto H = static_cast<float>(GetScreenHeight());

    zones[0] = {0, 48, 150, H - 96};           // Liste fichiers
    zones[1] = {0, H - 48, 150, 48};           // Bouton Générer
    zones[2] = {150, 0, L - 150, H - 72};      // Zone Vidéo
    zones[3] = {150, H - 72, L - 150, 2};      // Séparateur
    zones[4] = {158, H - 40, 32, 32};          // Play/Pause
    zones[5] = {198, H - 72, L - 382, 32};     // Label Temps
    zones[6] = {198, H - 40, L - 382, 32};     // Slider Progress
    zones[7] = {L - 176, H - 40, 32, 32};      // Bouton Muet
    zones[8] = {L - 136, H - 72, 128, 32};     // Label Volume
    zones[9] = {L - 136, H - 40, 128, 32};     // Slider Volume
    zones[10] = {0, 0, 150, 48};               // Bouton Dossier
}

void Master::afficherListeFichiers() {
    DrawRectangleRec(zones[0], RAYWHITE);
}

void Master::afficherZoneVideo() {
    DrawRectangleRec(zones[2], BLACK);
}
