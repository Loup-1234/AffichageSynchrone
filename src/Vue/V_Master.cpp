#define RAYGUI_IMPLEMENTATION
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wenum-compare"
#include "../../libs/raygui/raygui.h"
#pragma GCC diagnostic pop

#include "Vue/V_Master.h"
#include <filesystem>
#include <algorithm>
#include <iostream>

using namespace std;
namespace fs = filesystem;

V_Master::V_Master(const string &ipMulticast, const int portCommandes, const int portDecouverte, const int portReponse,
                   const vector<vector<string>> &specLecteurs)
    : controleur(ipMulticast, portCommandes, portDecouverte, portReponse, specLecteurs) {

    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 450, "Master UI - Contrôleur MVC");
    SetWindowMinSize(800, 450);
    InitAudioDevice();
    SetTargetFPS(60);

    chargerListeVideos();
    controleur.lancerRechercheLecteurs();

    miseAJourDisposition();
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

    // Panneau gauche : Vidéos
    zones[0] = {0, 48, largeurPanneauGauche, H - 96};
    zones[1] = {0, H - 48, largeurPanneauGauche, 48};
    zones[10] = {0, 0, largeurPanneauGauche, 48};

    // Zone centrale : Vidéo
    float largeurCentre = L - largeurPanneauGauche - largeurPanneauDroit;
    zones[2] = {largeurPanneauGauche, 0, largeurCentre, H - 72};
    zones[3] = {largeurPanneauGauche, H - 72, largeurCentre, 2};

    // Barre de contrôle (Bas)
    zones[4] = {largeurPanneauGauche + 8, H - 40, 32, 32}; // Play/Pause
    zones[5] = {largeurPanneauGauche + 48, H - 72, 120, 32}; // Texte Temps

    // La barre de progression s'étire en fonction de la place restante
    float largeurTimeline = largeurCentre - 228.0f;
    if (largeurTimeline < 50.0f) largeurTimeline = 50.0f; // Largeur minimale de sécurité
    zones[6] = {largeurPanneauGauche + 48, H - 40, largeurTimeline, 32};

    // Contrôles audio et vitesse (Droite du bas)
    zones[7] = {L - largeurPanneauDroit - 176, H - 40, 32, 32}; // Icône Volume
    zones[8] = {L - largeurPanneauDroit - 136, H - 72, 128, 32}; // Texte Volume
    zones[9] = {L - largeurPanneauDroit - 136, H - 40, 128, 32}; // Slider Volume
    zones[11] = {L - largeurPanneauDroit - 136, 10.0f, 128.0f, 32.0f}; // Menu Vitesse

    // Panneau droit : Lecteurs réseau
    zones[12] = {L - largeurPanneauDroit, 0, largeurPanneauDroit, H - 48};
    zones[13] = {L - largeurPanneauDroit, H - 48, largeurPanneauDroit, 48};
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

void V_Master::chargerListeLecteurs() {
    lecteursIPs.clear();
    lecteursCoches.clear();
    ordreSelectionLecteurs.clear();

    auto lecteurs = controleur.getDerniersLecteursTrouves();
    for (const auto &infos: lecteurs) {
        string label = infos.at("ip");
        lecteursIPs.push_back(label);
        lecteursCoches.push_back(false);
    }
}

vector<string> V_Master::getVideosSelectionnees() const {
    vector<string> selection;
    for (int index: ordreSelection) {
        if (index >= 0 && index < (int) fichiersVideo.size()) {
            selection.push_back("videos/" + fichiersVideo[index]);
        }
    }
    return selection;
}

vector<string> V_Master::getLecteursSelectionnes() const {
    vector<string> selection;
    for (int index: ordreSelectionLecteurs) {
        if (index >= 0 && index < (int) lecteursIPs.size()) {
            selection.push_back(lecteursIPs[index]);
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
    // GESTION DU REDIMENSIONNEMENT FENÊTRE
    if (IsWindowResized()) {
        // Sécurité : Empêcher les panneaux d'être plus larges que la moitié de l'écran si on réduit la fenêtre
        float limiteMax = static_cast<float>(GetScreenWidth()) / 2.5f;
        if (largeurPanneauGauche > limiteMax) largeurPanneauGauche = limiteMax;
        if (largeurPanneauDroit > limiteMax) largeurPanneauDroit = limiteMax;
        miseAJourDisposition();
    }

    // LOGIQUE DE REDIMENSIONNEMENT DES PANNEAUX
    float mouseX = GetMouseX();
    float mouseY = GetMouseY();
    float L = static_cast<float>(GetScreenWidth());

    // Définition des "zones de capture" (8 pixels de large au bord de chaque panneau)
    Rectangle zoneCaptureGauche = {largeurPanneauGauche - 4.0f, 0, 8.0f, static_cast<float>(GetScreenHeight())};
    Rectangle zoneCaptureDroit = {L - largeurPanneauDroit - 4.0f, 0, 8.0f, static_cast<float>(GetScreenHeight())};

    bool surGauche = CheckCollisionPointRec({mouseX, mouseY}, zoneCaptureGauche);
    bool surDroit = CheckCollisionPointRec({mouseX, mouseY}, zoneCaptureDroit);

    // Changer le curseur de la souris pour indiquer qu'on peut redimensionner
    if (surGauche || enRedimensionnementGauche || surDroit || enRedimensionnementDroit) {
        SetMouseCursor(MOUSE_CURSOR_RESIZE_EW);
    } else {
        SetMouseCursor(MOUSE_CURSOR_DEFAULT);
    }

    // Détection du clic pour démarrer le glissement
    if (surGauche && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) enRedimensionnementGauche = true;
    if (surDroit && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) enRedimensionnementDroit = true;

    // Arrêt du glissement au relâchement
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        enRedimensionnementGauche = false;
        enRedimensionnementDroit = false;
    }

    // Mise à jour de la largeur en temps réel
    if (enRedimensionnementGauche) {
        largeurPanneauGauche = mouseX;
        if (largeurPanneauGauche < 100) largeurPanneauGauche = 100; // Largeur minimale
        if (largeurPanneauGauche > L / 2.5f) largeurPanneauGauche = L / 2.5f; // Largeur maximale
        miseAJourDisposition();
    }

    if (enRedimensionnementDroit) {
        largeurPanneauDroit = L - mouseX;
        if (largeurPanneauDroit < 100) largeurPanneauDroit = 100;
        if (largeurPanneauDroit > L / 2.5f) largeurPanneauDroit = L / 2.5f;
        miseAJourDisposition();
    }

    controleur.mettreAJour();

    if (controleur.resultatsRechercheDisponibles()) {
        chargerListeLecteurs();
    }

    void *pixels = nullptr;
    unsigned int largeur = 0, hauteur = 0;
    bool redimensionnement = false;

    if (controleur.recupererFrameVideo(pixels, largeur, hauteur, redimensionnement)) {
        largeurVideoCache = largeur;
        hauteurVideoCache = hauteur;

        if (redimensionnement) {
            if (textureVideo.id > 0) UnloadTexture(textureVideo);
            if (largeur > 0 && hauteur > 0) {
                const Image img = GenImageColor(largeur, hauteur, BLACK);
                textureVideo = LoadTextureFromImage(img);
                UnloadImage(img);
            }
        }
        if (largeur > 0 && hauteur > 0 && pixels != nullptr) {
            UpdateTexture(textureVideo, pixels);
        }
    }

    if (delaiRecherche > 0) delaiRecherche -= GetFrameTime();
    if (!enGlissement && delaiRecherche <= 0 && controleur.getDureeTotale() > 0) {
        valeurProgression = controleur.estTermine() ? controleur.getDureeTotale() : controleur.getProgressionActuelle();
    }
}

void V_Master::dessinerInterface() {
    BeginDrawing();
    ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

    dessinerZoneVideo();
    DrawRectangleRec(zones[3], GRAY);

    dessinerListeFichiers();
    dessinerListeLecteurs();
    dessinerPanneauControle();

    gererVitesse();
    dessinerOverlayChargement();

    EndDrawing();
}

void V_Master::dessinerZoneVideo() const {
    DrawRectangleRec(zones[2], BLACK);

    if (largeurVideoCache > 0 && hauteurVideoCache > 0 && textureVideo.id > 0) {
        const float echelleX = zones[2].width / static_cast<float>(largeurVideoCache);
        const float echelleY = zones[2].height / static_cast<float>(hauteurVideoCache);
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
    bool desactiver = controleur.estGenerationEnCours() || controleur.estRechercheEnCours();
    GuiSetState(desactiver ? STATE_DISABLED : STATE_NORMAL);

    if (GuiButton(zones[1], "GÉNÉRER ET\nTRANSFÉRER")) {
        controleur.initialiserSession(getVideosSelectionnees());
    }

    constexpr float HAUTEUR_LIGNE = 30.0f;
    constexpr float TAILLE_CHECKBOX = 20.0f;
    constexpr float MARGE = 10.0f;
    constexpr float LARGEUR_SCROLLBAR = 16.0f;

    Rectangle vue = {0};
    const float hauteurContenu = static_cast<float>(fichiersVideo.size()) * HAUTEUR_LIGNE;
    const Rectangle zoneScroll = {0, 0, zones[0].width - LARGEUR_SCROLLBAR, hauteurContenu};

    GuiScrollPanel(zones[0], nullptr, zoneScroll, &positionDefilement, &vue);

    BeginScissorMode(static_cast<int>(vue.x), static_cast<int>(vue.y), static_cast<int>(vue.width),
                     static_cast<int>(vue.height));

    for (size_t i = 0; i < fichiersVideo.size(); ++i) {
        const float posY = zones[0].y + MARGE + static_cast<float>(i) * HAUTEUR_LIGNE + positionDefilement.y;
        const Rectangle rectItem = {zones[0].x + MARGE + positionDefilement.x, posY, TAILLE_CHECKBOX, TAILLE_CHECKBOX};

        if (rectItem.y + rectItem.height < vue.y || rectItem.y > vue.y + vue.height) continue;

        int ordre = 0;
        if (videosCochees[i]) {
            auto it = ranges::find(ordreSelection, static_cast<int>(i));
            if (it != ordreSelection.end()) {
                ordre = static_cast<int>(distance(ordreSelection.begin(), it)) + 1;
            }
        }

        string etiquette = fichiersVideo[i] + (ordre > 0 ? " (" + to_string(ordre) + ")" : "");
        bool coche = videosCochees[i];

        GuiCheckBox(rectItem, etiquette.c_str(), &coche);

        if (coche != videosCochees[i]) {
            videosCochees[i] = coche;
            if (coche) ordreSelection.push_back(static_cast<int>(i));
            else ordreSelection.erase(ranges::find(ordreSelection, static_cast<int>(i)));
        }
    }
    EndScissorMode();

    if (GuiButton(zones[10], "DOSSIER")) ouvrirDossierVideos();
    GuiSetState(STATE_NORMAL);
}

void V_Master::dessinerListeLecteurs() {
    bool desactiver = controleur.estGenerationEnCours() || controleur.estRechercheEnCours();
    GuiSetState(desactiver ? STATE_DISABLED : STATE_NORMAL);

    if (GuiButton(zones[13], "CHERCHER IP")) {
        controleur.lancerRechercheLecteurs();
    }

    constexpr float HAUTEUR_LIGNE = 30.0f;
    constexpr float TAILLE_CHECKBOX = 20.0f;
    constexpr float MARGE = 10.0f;
    constexpr float LARGEUR_SCROLLBAR = 16.0f;

    Rectangle vue = {0};
    const float hauteurContenu = static_cast<float>(lecteursIPs.size()) * HAUTEUR_LIGNE;
    const Rectangle zoneScroll = {0, 0, zones[12].width - LARGEUR_SCROLLBAR, hauteurContenu};

    GuiScrollPanel(zones[12], nullptr, zoneScroll, &positionDefilementLecteurs, &vue);

    BeginScissorMode(static_cast<int>(vue.x), static_cast<int>(vue.y), static_cast<int>(vue.width), static_cast<int>(vue.height));

    for (size_t i = 0; i < lecteursIPs.size(); ++i) {
        const float posY = zones[12].y + MARGE + static_cast<float>(i) * HAUTEUR_LIGNE + positionDefilementLecteurs.y;
        const Rectangle rectItem = {zones[12].x + MARGE + positionDefilementLecteurs.x, posY, TAILLE_CHECKBOX, TAILLE_CHECKBOX};

        if (rectItem.y + rectItem.height < vue.y || rectItem.y > vue.y + vue.height) continue;

        int ordre = 0;
        if (lecteursCoches[i]) {
            auto it = ranges::find(ordreSelectionLecteurs, static_cast<int>(i));
            if (it != ordreSelectionLecteurs.end()) {
                ordre = static_cast<int>(distance(ordreSelectionLecteurs.begin(), it)) + 1;
            }
        }

        string etiquette = lecteursIPs[i] + (ordre > 0 ? " (" + to_string(ordre) + ")" : "");
        bool coche = lecteursCoches[i];

        GuiCheckBox(rectItem, etiquette.c_str(), &coche);

        if (coche != lecteursCoches[i]) {
            lecteursCoches[i] = coche;
            if (coche) ordreSelectionLecteurs.push_back(static_cast<int>(i));
            else ordreSelectionLecteurs.erase(ranges::find(ordreSelectionLecteurs, static_cast<int>(i)));
        }
    }

    EndScissorMode();
    GuiSetState(STATE_NORMAL);
}

void V_Master::dessinerPanneauControle() {
    bool desactiver = controleur.estGenerationEnCours() || controleur.estRechercheEnCours();
    GuiSetState(desactiver ? STATE_DISABLED : STATE_NORMAL);

    if (GuiButton(zones[4], controleur.estEnLecture() ? "#132#" : "#131#")) {
        controleur.basculerPlayPause();
    }

    gererBarreProgression();
    gererControlesVolume();
    GuiSetState(STATE_NORMAL);
}

void V_Master::gererBarreProgression() {
    const float dureeTotale = controleur.getDureeTotale();
    const bool desactiverSlider = (dureeTotale <= 0.0f) || controleur.estTermine();

    const int minutes = static_cast<int>(valeurProgression) / 60;
    const int secondes = static_cast<int>(valeurProgression) % 60;
    const int dureeMinutes = static_cast<int>(dureeTotale) / 60;
    const int dureeSecondes = static_cast<int>(dureeTotale) % 60;

    GuiLabel(zones[5], TextFormat("%02d:%02d / %02d:%02d", minutes, secondes, dureeMinutes, dureeSecondes));

    if (desactiverSlider) GuiSetState(STATE_DISABLED);

    if (!desactiverSlider && CheckCollisionPointRec(GetMousePosition(), zones[6]) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        enGlissement = true;
        etaitEnLectureAvantGlissement = controleur.estEnLecture();
    }

    const float ancienneProg = valeurProgression;
    GuiSliderBar(zones[6], "", nullptr, &valeurProgression, 0.0f, (dureeTotale <= 0.0f) ? 1.0f : dureeTotale);

    if (desactiverSlider) GuiSetState(STATE_NORMAL);

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

    if (controleur.getDureeTotale() <= 0.0f || controleur.estGenerationEnCours() || controleur.estRechercheEnCours()) {
        GuiSetState(STATE_DISABLED);
    }

    if (GuiDropdownBox(zones[11], "Vitesse: 0.5x;Vitesse: 1.0x;Vitesse: 1.5x;Vitesse: 2.0x", &indexVitesse,
                       menuVitesseActif)) {
        menuVitesseActif = !menuVitesseActif;
    }

    GuiSetState(STATE_NORMAL);

    if (indexVitesse != ancienIndex && !menuVitesseActif) {
        controleur.modifierVitesse(valeursVitesse[indexVitesse]);
    }
}

void V_Master::dessinerOverlayChargement() {
    if (controleur.estGenerationEnCours() || controleur.estRechercheEnCours()) {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.7f));

        const char *texteAffichage = "Chargement...";
        if (controleur.estTransfertEnCours()) {
            texteAffichage = "Transfert en cours...";
        } else if (controleur.estGenerationEnCours()) {
            texteAffichage = "Génération vidéo en cours...";
        } else if (controleur.estRechercheEnCours()) {
            texteAffichage = "Recherche des lecteurs sur le réseau...";
        }

        int largeurTexte = MeasureText(texteAffichage, 20);
        ::DrawText(texteAffichage, (GetScreenWidth() - largeurTexte) / 2, GetScreenHeight() / 2 - 30, 20, WHITE);

        rotationChargement += 4.0f;
        const Rectangle rectChargement = {
            static_cast<float>(GetScreenWidth()) / 2, static_cast<float>(GetScreenHeight()) / 2 + 20, 20, 20
        };
        DrawRectanglePro(rectChargement, {10, 10}, rotationChargement, WHITE);
    }
}