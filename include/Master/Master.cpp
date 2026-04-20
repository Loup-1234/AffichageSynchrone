#define RAYGUI_IMPLEMENTATION

#include "Master.h"
#include "../include/raygui/raygui.h" // Vérifie ce chemin
#include "VideoComplexe.h" // <-- Remplacement ici
#include <filesystem>
#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <iostream>

using namespace std;
namespace fs = filesystem;

// ==========================================
// Callbacks VLC pour le rendu dans Raylib
// ==========================================

static void *verrouiller(void *donnees, void **p_pixels) {
    auto *app = static_cast<Master *>(donnees);
    app->mutexImage.lock();
    *p_pixels = app->pixelsVideo.data();
    return nullptr;
}

static void deverrouiller(void *donnees) {
    auto *app = static_cast<Master *>(donnees);
    app->mutexImage.unlock();
    app->framePrete = true;
}

static unsigned configurerVideo(void **donnees, char *chroma, const unsigned *largeur, const unsigned *hauteur, unsigned *pas, unsigned *lignes) {
    auto *app = static_cast<Master *>(*donnees);
    app->mutexImage.lock();
    app->largeurVideo = *largeur;
    app->hauteurVideo = *hauteur;
    app->pixelsVideo.resize(app->largeurVideo * app->hauteurVideo * 4);
    memcpy(chroma, "RGBA", 4);
    *pas = app->largeurVideo * 4;
    *lignes = app->hauteurVideo;
    app->textureDoitEtreRedimensionnee = true;
    app->mutexImage.unlock();
    return 1;
}

// ==========================================
// Implémentation de la classe Master
// ==========================================

Master::Master(const string &ipGroupe, const int port) : udp(ipGroupe, port) {
    // Initialisation Raylib
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 450, "Master UI - Connecté au Réseau");
    SetWindowMinSize(800, 450);
    InitAudioDevice();
    chargerListeVideos();
    SetTargetFPS(30);

    // Initialisation VLC
    instanceVLC = libvlc_new(0, nullptr);
    lecteurVLC = libvlc_media_player_new(instanceVLC);
    textureVideo = {0};

    miseAJourDisposition();

    if (fs::exists(cheminVideoComplexe)) {
        chargerVideo();
    }
}

Master::~Master() {
    if (threadGeneration.joinable()) threadGeneration.join();
    if (lecteurVLC) {
        libvlc_media_player_stop(lecteurVLC);
        libvlc_media_player_release(lecteurVLC);
    }
    if (instanceVLC) libvlc_release(instanceVLC);
    UnloadTexture(textureVideo);
    CloseAudioDevice();
    CloseWindow();
}

void Master::envoyerCommande(TypeCommande type, float valeur) {
    PaquetControle p;
    p.type = type;
    p.valeur = valeur;
    udp.Envoyer(&p, sizeof(p));
}

void Master::miseAJourDisposition() {
    const auto L = static_cast<float>(GetScreenWidth());
    const auto H = static_cast<float>(GetScreenHeight());

    zones[0] = {0, 48, 150, H - 96}; // Liste fichiers
    zones[1] = {0, H - 48, 150, 48}; // Bouton Générer
    zones[2] = {150, 0, L - 150, H - 72}; // Zone Vidéo
    zones[3] = {150, H - 72, L - 150, 2}; // Séparateur
    zones[4] = {158, H - 40, 32, 32}; // Play/Pause
    zones[5] = {198, H - 72, L - 382, 32}; // Label Temps
    zones[6] = {198, H - 40, L - 382, 32}; // Slider Progress
    zones[7] = {L - 176, H - 40, 32, 32}; // Bouton Muet
    zones[8] = {L - 136, H - 72, 128, 32}; // Label Volume
    zones[9] = {L - 136, H - 40, 128, 32}; // Slider Volume
    zones[10] = {0, 0, 150, 48}; // Bouton Dossier
}

void Master::chargerListeVideos() {
    fichiersVideo.clear();
    videosSelectionnees.clear();
    ordreSelection.clear();

    string chemin = "videos";
    if (fs::exists(chemin) && fs::is_directory(chemin)) {
        for (const auto &entree: fs::directory_iterator(chemin)) {
            if (entree.is_regular_file()) {
                string ext = entree.path().extension().string();
                if (ext == ".mp4" || ext == ".mp3") {
                    fichiersVideo.push_back(entree.path().filename().string());
                    videosSelectionnees.push_back(false);
                }
            }
        }
    }
}

void Master::chargerVideo() {
    libvlc_media_t *media = libvlc_media_new_path(instanceVLC, cheminVideoComplexe.c_str());
    if (!media) { duree = 0.0f; return; }

    libvlc_media_player_set_media(lecteurVLC, media);
    libvlc_media_release(media);

    libvlc_video_set_callbacks(lecteurVLC, verrouiller, reinterpret_cast<libvlc_video_unlock_cb>(deverrouiller), nullptr, this);
    libvlc_video_set_format_callbacks(lecteurVLC, reinterpret_cast<libvlc_video_format_cb>(configurerVideo), nullptr);

    libvlc_media_player_play(lecteurVLC);
    this_thread::sleep_for(chrono::milliseconds(200));
    libvlc_media_player_set_pause(lecteurVLC, 1);
    playPause = false;

    const libvlc_time_t len = libvlc_media_player_get_length(lecteurVLC);
    duree = (len != -1) ? static_cast<float>(len) / 1000.0f : 0.0f;

    libvlc_audio_set_volume(lecteurVLC, static_cast<int>(valeurSliderSon));
}

void Master::generer() {
    if (generationEnCours) return;

    vector<string> videos;
    const fs::path cheminBase = "videos";
    for (const int index : ordreSelection) {
        if (index >= 0 && index < fichiersVideo.size()) {
            videos.push_back((cheminBase / fichiersVideo[index]).string());
        }
    }

    if (videos.empty()) return;

    string extension = fs::path(videos[0]).extension().string();
    bool estMp3 = (extension == ".mp3");

    // Vérification de sécurité
    if ((estMp3 && videos.size() < 3) || (!estMp3 && videos.size() < 2)) return;

    libvlc_media_player_stop(lecteurVLC);
    duree = 0.0f;
    valeurSliderProgression = 0.0f;
    generationEnCours = true;

    if (threadGeneration.joinable()) threadGeneration.join();

    threadGeneration = thread([this, videos]() {
        VideoComplexe videoComplexe;
        try {
            // Appel direct, la classe VideoComplexe s'occupe de la logique
            videoComplexe.genererVideoComplexe(videos, cheminVideoComplexe);
            videoGeneree = true;
        } catch (const exception& e) {
            cerr << "Erreur lors de la génération : " << e.what() << endl;
        }
        generationEnCours = false;
    });
}

void Master::ouvrirDossierVideos() {
    const string chemin = "videos";
    if (!fs::exists(chemin)) fs::create_directory(chemin);
#ifdef _WIN32
    string commande = "explorer " + chemin;
#elif __APPLE__
    string commande = "open " + chemin;
#else
    string commande = "xdg-open " + chemin;
#endif
    system(commande.c_str());
}

void Master::lecturePause() {
    if (libvlc_media_player_is_playing(lecteurVLC)) {
        libvlc_media_player_set_pause(lecteurVLC, 1);
        playPause = false;
    } else {
        libvlc_media_player_play(lecteurVLC);
        playPause = true;
    }
    envoyerCommande(TypeCommande::LECTURE_PAUSE, playPause ? 1.0f : 0.0f);
}

void Master::son() {
    estMuet = !estMuet;
    if (estMuet) {
        valeurSliderSonPrecedent = valeurSliderSon;
        setVolume(0.0f);
    } else {
        setVolume(valeurSliderSonPrecedent);
    }
    envoyerCommande(TypeCommande::VOLUME, estMuet ? 0.0f : valeurSliderSon);
}

void Master::setVolume(const float volume) {
    valeurSliderSon = volume;
    libvlc_audio_set_volume(lecteurVLC, static_cast<int>(valeurSliderSon));
}

void Master::barreProgression(bool &enGlissement, bool &etaitEnLecture, float &delaiRecherche) {
    const bool estEnLecture = libvlc_media_player_is_playing(lecteurVLC);

    if (CheckCollisionPointRec(GetMousePosition(), zones[6]) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        enGlissement = true;
        etaitEnLecture = estEnLecture;
        libvlc_media_player_set_pause(lecteurVLC, 1);
        libvlc_audio_set_volume(lecteurVLC, 0);
    }

    const float ancienneValeur = valeurSliderProgression;
    GuiSliderBar(zones[6], "", nullptr, &valeurSliderProgression, 0.0f, duree);

    if (enGlissement) {
        if (valeurSliderProgression != ancienneValeur) {
            libvlc_media_player_set_time(lecteurVLC, static_cast<libvlc_time_t>(valeurSliderProgression * 1000));
            envoyerCommande(TypeCommande::PROGRESSION, valeurSliderProgression);
        }

        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) || !IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            enGlissement = false;
            if (etaitEnLecture) libvlc_media_player_play(lecteurVLC);
            libvlc_audio_set_volume(lecteurVLC, static_cast<int>(valeurSliderSon));
            delaiRecherche = 0.2f;

            envoyerCommande(TypeCommande::PROGRESSION, valeurSliderProgression);
        }
    }
}

void Master::barreVolume() {
    float nouveauVolume = valeurSliderSon;
    GuiSliderBar(zones[9], "", nullptr, &nouveauVolume, 0.0f, 100.0f);
    if (nouveauVolume != valeurSliderSon) {
        setVolume(nouveauVolume);
        estMuet = (valeurSliderSon <= 0.0f);
        envoyerCommande(TypeCommande::VOLUME, valeurSliderSon);
    }
}

void Master::afficherVideo() {
    if (framePrete) {
        mutexImage.lock();
        if (largeurVideo > 0 && hauteurVideo > 0) {
            UpdateTexture(textureVideo, pixelsVideo.data());
        }
        mutexImage.unlock();
        framePrete = false;
    }

    if (largeurVideo == 0 || hauteurVideo == 0) return;

    const float echelleX = zones[2].width / static_cast<float>(largeurVideo);
    const float echelleY = zones[2].height / static_cast<float>(hauteurVideo);
    const float echelle = min(echelleX, echelleY);

    const float destLargeur = static_cast<float>(largeurVideo) * echelle;
    const float destHauteur = static_cast<float>(hauteurVideo) * echelle;
    const float destX = zones[2].x + (zones[2].width - destLargeur) / 2.0f;
    const float destY = zones[2].y + (zones[2].height - destHauteur) / 2.0f;

    const Rectangle source = {0.0f, 0.0f, static_cast<float>(largeurVideo), static_cast<float>(hauteurVideo)};
    const Rectangle dest = {destX, destY, destLargeur, destHauteur};

    DrawTexturePro(textureVideo, source, dest, {0,0}, 0.0f, WHITE);
}

void Master::afficherListeFichiers() {
    Rectangle vue = {0};
    float contentHeight = static_cast<float>(fichiersVideo.size()) * 30;

    GuiScrollPanel(zones[0], nullptr, (Rectangle){0, 0, zones[0].width - 16, contentHeight}, &positionDefilement, &vue);

    BeginScissorMode(static_cast<int>(vue.x), static_cast<int>(vue.y), static_cast<int>(vue.width), static_cast<int>(vue.height));

    for (size_t i = 0; i < fichiersVideo.size(); ++i) {
        Rectangle itemRect = {
            zones[0].x + 10 + positionDefilement.x,
            zones[0].y + 10 + static_cast<float>(i) * 30 + positionDefilement.y,
            20, 20
        };

        if (itemRect.y + itemRect.height < vue.y || itemRect.y > vue.y + vue.height) continue;

        int ordre = 0;
        if (videosSelectionnees[i]) {
            auto it = ranges::find(ordreSelection, static_cast<int>(i));
            if (it != ordreSelection.end()) ordre = static_cast<int>(distance(ordreSelection.begin(), it)) + 1;
        }

        string etiquette = fichiersVideo[i];
        if (ordre > 0) etiquette += " (" + to_string(ordre) + ")";

        bool coche = videosSelectionnees[i];
        const bool etaitCoche = coche;
        GuiCheckBox(itemRect, etiquette.c_str(), &coche);

        if (coche != etaitCoche) {
            videosSelectionnees[i] = coche;
            if (coche) ordreSelection.push_back(static_cast<int>(i));
            else {
                auto it = ranges::find(ordreSelection, static_cast<int>(i));
                if (it != ordreSelection.end()) ordreSelection.erase(it);
            }
        }
    }
    EndScissorMode();
}

void Master::executer() {
    bool enGlissement = false;
    bool etaitEnLecture = false;
    float delaiRecherche = 0.0f;
    float rotationChargement = 0.0f;

    while (!WindowShouldClose()) {
        if (IsWindowResized()) miseAJourDisposition();

        if (videoGeneree) {
            chargerVideo();
            videoGeneree = false;
        }

        if (textureDoitEtreRedimensionnee) {
            mutexImage.lock();
            if (textureVideo.id > 0) UnloadTexture(textureVideo);
            if (largeurVideo > 0 && hauteurVideo > 0) {
                Image img = GenImageColor(static_cast<int>(largeurVideo), static_cast<int>(hauteurVideo), BLACK);
                textureVideo = LoadTextureFromImage(img);
                UnloadImage(img);
            } else {
                textureVideo = {0};
            }
            textureDoitEtreRedimensionnee = false;
            mutexImage.unlock();
        }

        if (delaiRecherche > 0) delaiRecherche -= GetFrameTime();

        if (!enGlissement && delaiRecherche <= 0 && duree > 0) {
            libvlc_time_t tempsActuel = libvlc_media_player_get_time(lecteurVLC);
            if (tempsActuel != -1) {
                valeurSliderProgression = static_cast<float>(tempsActuel) / 1000.0f;
            }
        }

        BeginDrawing();
        ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

        if (duree > 0 && textureVideo.id > 0) afficherVideo();

        DrawRectangleRec(zones[3], GRAY);
        afficherListeFichiers();

        GuiSetState(generationEnCours ? STATE_DISABLED : STATE_NORMAL);
        if (GuiButton(zones[1], TEXTE_BOUTON_GENERER)) generer();
        if (GuiButton(zones[10], TEXTE_BOUTON_OUVRIR_DOSSIER)) ouvrirDossierVideos();
        GuiSetState(STATE_NORMAL);

        if (GuiButton(zones[4], playPause ? "#132#" : "#131#")) lecturePause();

        const int minutes = static_cast<int>(valeurSliderProgression) / 60;
        const int secondes = static_cast<int>(valeurSliderProgression) % 60;
        const int dureeMinutes = static_cast<int>(duree) / 60;
        const int dureeSecondes = static_cast<int>(duree) % 60;
        GuiLabel(zones[5], TextFormat("%02d:%02d / %02d:%02d", minutes, secondes, dureeMinutes, dureeSecondes));

        barreProgression(enGlissement, etaitEnLecture, delaiRecherche);

        if (GuiButton(zones[7], estMuet ? "#128#" : "#122#")) son();

        GuiLabel(zones[8], TextFormat("%d%%", static_cast<int>(valeurSliderSon)));
        barreVolume();

        if (generationEnCours) {
            DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.5f));
            const auto texteChargement = "Génération en cours...";
            const int tailleTexte = MeasureText(texteChargement, 20);
            DrawText(texteChargement, GetScreenWidth() / 2 - tailleTexte / 2, GetScreenHeight() / 2, 20, WHITE);
            rotationChargement += 4.0f;
            const Rectangle rectChargement = {static_cast<float>(GetScreenWidth()) / 2, static_cast<float>(GetScreenHeight()) / 2 - 40, 20, 20};
            DrawRectanglePro(rectChargement, {10, 10}, rotationChargement, WHITE);
        }

        EndDrawing();
    }
}