#define RAYGUI_IMPLEMENTATION
#include "Vue/V_Master.h"
#include "../../libs/raygui/raygui.h"
#include <filesystem>
#include <algorithm>

using namespace std;
namespace fs = filesystem;

V_Master::V_Master(const string &ipMulticast, const int port, const vector<vector<string> > &specLecteurs)
    : controleur(ipMulticast, port, specLecteurs) {
    // Configuration des paramètres de fenêtre et du moteur de rendu
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 450, "Master UI - Contrôleur MVC");
    SetWindowMinSize(800, 450);
    InitAudioDevice();
    SetTargetFPS(30);

    chargerListeVideos();
    miseAJourDisposition();

    // Synchronisation initiale du volume
    controleur.modifierVolume(valeurVolume, estMuet);
}

V_Master::~V_Master() {
    controleur.stopper();

    if (textureVideo.id > 0) UnloadTexture(textureVideo);
    CloseAudioDevice();
    CloseWindow();
}

void V_Master::miseAJourDisposition() {
    const auto L = static_cast<float>(GetScreenWidth());
    const auto H = static_cast<float>(GetScreenHeight());

    // Définition statique des zones de l'interface (Ancrages)
    zones[0] = {0, 48, 150, H - 96};        // Liste fichiers
    zones[1] = {0, H - 48, 150, 48};        // Bouton Générer
    zones[2] = {150, 0, L - 150, H - 72};   // Zone Vidéo
    zones[3] = {150, H - 72, L - 150, 2};   // Séparateur horizontal
    zones[4] = {158, H - 40, 32, 32};       // Play/Pause
    zones[5] = {198, H - 72, L - 382, 32};  // Label Temps
    zones[6] = {198, H - 40, L - 382, 32};  // Slider Progress
    zones[7] = {L - 176, H - 40, 32, 32};   // Bouton Muet
    zones[8] = {L - 136, H - 72, 128, 32};  // Label Volume
    zones[9] = {L - 136, H - 40, 128, 32};  // Slider Volume
    zones[10] = {0, 0, 150, 48};            // Bouton Dossier
    zones[11] = {L - 136, 10.0f, 128.0f, 32.0f}; // Menu Vitesse
}

void V_Master::chargerListeVideos() {
    fichiersVideo.clear();
    videosCochees.clear();
    ordreSelection.clear();

    if (fs::exists("videos") && fs::is_directory("videos")) {
        for (const auto &entree: fs::directory_iterator("videos")) {
            if (entree.is_regular_file()) {
                string ext = entree.path().extension().string();
                if (ext == ".mp4" || ext == ".mp3" || ext == ".avi" || ext == ".wav") {
                    fichiersVideo.push_back(entree.path().filename().string());
                    videosCochees.push_back(false);
                }
            }
        }
    }
}

vector<string> V_Master::getVideosSelectionnees() const {
    vector<string> selection;
    for (int index: ordreSelection) {
        if (index >= 0 && index < fichiersVideo.size()) {
            selection.push_back("videos/" + fichiersVideo[index]);
        }
    }
    return selection;
}

void V_Master::ouvrirDossierVideos() {
    if (!fs::exists("videos")) fs::create_directory("videos");
#ifdef _WIN32
    system("explorer videos");
#elif __APPLE__
    system("open videos");
#else
    system("xdg-open videos");
#endif
}

void V_Master::executer() {
    while (!WindowShouldClose()) {
        gererLogique();
        dessinerInterface();
    }
}

void V_Master::gererLogique() {
    if (IsWindowResized()) miseAJourDisposition();

    // Demande au contrôleur de mettre à jour son état interne (VLC, etc.)
    controleur.mettreAJour();

    void *pixels = nullptr;
    unsigned int largeur = 0;
    unsigned int hauteur = 0;
    bool redimensionnement = false;

    // Récupération de la frame décodée par VLC via le contrôleur
    if (controleur.recupererFrameVideo(pixels, largeur, hauteur, redimensionnement)) {
        largeurVideoCache = largeur;
        hauteurVideoCache = hauteur;

        // Recréation de la texture si la résolution de la vidéo a changé
        if (redimensionnement) {
            if (textureVideo.id > 0) UnloadTexture(textureVideo);
            if (largeur > 0 && hauteur > 0) {
                const Image img = GenImageColor(largeur, hauteur, BLACK);
                textureVideo = LoadTextureFromImage(img);
                UnloadImage(img);
            }
        }

        // Mise à jour des pixels en mémoire GPU
        if (largeur > 0 && hauteur > 0 && pixels != nullptr) {
            UpdateTexture(textureVideo, pixels);
        }
    }

    // Mise à jour de la barre de progression (si on n'est pas en train de chercher)
    if (delaiRecherche > 0) delaiRecherche -= GetFrameTime();
    if (!enGlissement && delaiRecherche <= 0 && controleur.getDureeTotale() > 0) {
        valeurProgression = controleur.estTermine() ?
            controleur.getDureeTotale() : controleur.getProgressionActuelle();
    }
}

void V_Master::dessinerInterface() {
    BeginDrawing();
    ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

    dessinerZoneVideo();
    DrawRectangleRec(zones[3], GRAY);
    dessinerListeFichiers();
    dessinerPanneauControle();
    gererVitesse();
    dessinerOverlayChargement(); // Toujours dessiné en dernier (au-dessus)

    EndDrawing();
}

void V_Master::dessinerZoneVideo() const {
    DrawRectangleRec(zones[2], BLACK);

    if (largeurVideoCache > 0 && hauteurVideoCache > 0 && textureVideo.id > 0) {
        // Calcul du ratio pour l'affichage "Letterbox"
        float echelleX = zones[2].width / static_cast<float>(largeurVideoCache);
        float echelleY = zones[2].height / static_cast<float>(hauteurVideoCache);
        const float echelle = min(echelleX, echelleY);

        const float destLargeur = static_cast<float>(largeurVideoCache) * echelle;
        const float destHauteur = static_cast<float>(hauteurVideoCache) * echelle;
        const float destX = zones[2].x + (zones[2].width - destLargeur) / 2.0f;
        const float destY = zones[2].y + (zones[2].height - destHauteur) / 2.0f;

        DrawTexturePro(textureVideo,
                       {0.0f, 0.0f, static_cast<float>(largeurVideoCache), static_cast<float>(hauteurVideoCache)},
                       {destX, destY, destLargeur, destHauteur},
                       {0, 0}, 0.0f, WHITE);
    }
}

void V_Master::dessinerListeFichiers() {
    Rectangle vue = {0};
    const float contentHeight = static_cast<float>(fichiersVideo.size()) * 30;

    // Panneau scrollable pour la liste des fichiers
    GuiScrollPanel(zones[0], nullptr, (Rectangle){0, 0, zones[0].width - 16, contentHeight}, &positionDefilement, &vue);

    BeginScissorMode(static_cast<int>(vue.x), static_cast<int>(vue.y), static_cast<int>(vue.width),
                     static_cast<int>(vue.height));
    for (size_t i = 0; i < fichiersVideo.size(); ++i) {
        const Rectangle itemRect = {
            zones[0].x + 10 + positionDefilement.x,
            zones[0].y + 10 + static_cast<float>(i) * 30 + positionDefilement.y, 20, 20
        };

        if (itemRect.y + itemRect.height < vue.y || itemRect.y > vue.y + vue.height) continue;

        // Gestion de l'ordre de sélection (pour FFmpeg)
        int ordre = 0;
        if (videosCochees[i]) {
            auto it = ranges::find(ordreSelection, static_cast<int>(i));
            if (it != ordreSelection.end()) ordre = static_cast<int>(distance(ordreSelection.begin(), it)) + 1;
        }

        string etiquette = fichiersVideo[i] + (ordre > 0 ? " (" + to_string(ordre) + ")" : "");
        bool coche = videosCochees[i];
        GuiCheckBox(itemRect, etiquette.c_str(), &coche);

        if (coche != videosCochees[i]) {
            videosCochees[i] = coche;
            if (coche) ordreSelection.push_back(static_cast<int>(i));
            else ordreSelection.erase(ranges::find(ordreSelection, static_cast<int>(i)));
        }
    }
    EndScissorMode();
}

void V_Master::dessinerPanneauControle() {
    // Désactivation des boutons pendant les traitements lourds (Génération/TFTP)
    GuiSetState(controleur.estGenerationEnCours() ? STATE_DISABLED : STATE_NORMAL);
    if (GuiButton(zones[1], "GÉNÉRER ET\nTRANSFÉRER")) {
        controleur.initialiserSession(getVideosSelectionnees());
    }
    if (GuiButton(zones[10], "DOSSIER")) ouvrirDossierVideos();
    GuiSetState(STATE_NORMAL);

    if (GuiButton(zones[4], controleur.estEnLecture() ? "#132#" : "#131#")) {
        controleur.basculerPlayPause();
    }

    gererBarreProgression();
    gererControlesVolume();
}

void V_Master::gererBarreProgression() {
    const float dureeTotale = controleur.getDureeTotale();
    const bool desactiverSlider = (dureeTotale <= 0.0f) || controleur.estTermine();

    // Formatage du temps HH:MM
    const int minutes = static_cast<int>(valeurProgression) / 60;
    const int secondes = static_cast<int>(valeurProgression) % 60;
    const int dureeMinutes = static_cast<int>(dureeTotale) / 60;
    const int dureeSecondes = static_cast<int>(dureeTotale) % 60;

    GuiLabel(zones[5], TextFormat("%02d:%02d / %02d:%02d", minutes, secondes, dureeMinutes, dureeSecondes));

    if (desactiverSlider) GuiSetState(STATE_DISABLED);

    // Détection du début de glissement (clic sur le slider)
    if (!desactiverSlider && CheckCollisionPointRec(GetMousePosition(), zones[6]) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        enGlissement = true;
        etaitEnLectureAvantGlissement = controleur.estEnLecture();
    }

    const float ancienneProg = valeurProgression;
    GuiSliderBar(zones[6], "", nullptr, &valeurProgression, 0.0f, (dureeTotale <= 0.0f) ? 1.0f : dureeTotale);

    if (desactiverSlider) GuiSetState(STATE_NORMAL);

    // Envoi des ordres de "Seek" au contrôleur pendant et après le glissement
    if (enGlissement && valeurProgression != ancienneProg) {
        controleur.modifierProgression(valeurProgression, true);
    }

    if (enGlissement && (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) || !IsMouseButtonDown(MOUSE_LEFT_BUTTON))) {
        enGlissement = false;
        delaiRecherche = 0.2f;
        controleur.modifierProgression(valeurProgression, false, etaitEnLectureAvantGlissement);
    }
}

void V_Master::gererControlesVolume() {
    if (GuiButton(zones[7], estMuet ? "#128#" : "#122#")) {
        estMuet = !estMuet;
        controleur.modifierVolume(valeurVolume, estMuet);
    }

    GuiLabel(zones[8], TextFormat("%d%%", static_cast<int>(valeurVolume)));

    const float ancienVol = valeurVolume;
    GuiSliderBar(zones[9], "", nullptr, &valeurVolume, 0.0f, 100.0f);

    if (valeurVolume != ancienVol) {
        estMuet = (valeurVolume <= 0.0f);
        controleur.modifierVolume(valeurVolume, estMuet);
    }
}

void V_Master::gererVitesse() {
    const float valeursVitesse[] = {0.5f, 1.0f, 1.5f, 2.0f};
    const int ancienIndex = indexVitesse;

    if (controleur.getDureeTotale() <= 0.0f) GuiSetState(STATE_DISABLED);

    if (GuiDropdownBox(zones[11], "Vitesse: 0.5x;Vitesse: 1.0x;Vitesse: 1.5x;Vitesse: 2.0x", &indexVitesse, menuVitesseActif)) {
        menuVitesseActif = !menuVitesseActif;
    }

    GuiSetState(STATE_NORMAL);

    if (indexVitesse != ancienIndex && !menuVitesseActif) {
        controleur.modifierVitesse(valeursVitesse[indexVitesse]);
    }
}

void V_Master::dessinerOverlayChargement() {
    // Si une génération ou un transfert est en cours dans le thread de fond
    if (controleur.estGenerationEnCours()) {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.7f));

        const char *texteAffichage = controleur.estTransfertEnCours() ? "Transfert en cours..." : "Génération vidéo en cours...";

        int largeurTexte = MeasureText(texteAffichage, 20);
        ::DrawText(texteAffichage, (GetScreenWidth() - largeurTexte) / 2, GetScreenHeight() / 2 - 30, 20, WHITE);

        // Petit carré blanc qui tourne pour l'animation
        rotationChargement += 4.0f;
        const Rectangle rectChargement = { static_cast<float>(GetScreenWidth()) / 2, static_cast<float>(GetScreenHeight()) / 2 + 20, 20, 20 };
        DrawRectanglePro(rectChargement, {10, 10}, rotationChargement, WHITE);
    }
}