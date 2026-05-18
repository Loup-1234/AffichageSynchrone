#define RAYGUI_IMPLEMENTATION

// Désactivation temporaire des avertissements de comparaison d'énumérations dans Raygui
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wenum-compare"
#include "../../libs/raygui/raygui.h"
#pragma GCC diagnostic pop

#include "Vue/V_Master.h"
#include <filesystem>
#include <algorithm>

using namespace std;
namespace fs = filesystem;

// --- CONSTRUCTEUR ET DESTRUCTEUR ---

V_Master::V_Master(const string &ipMulticast, const int portCommandes, const int portDecouverte, const int portReponse,
                   const vector<vector<string> > &specLecteurs)
    : controleur(ipMulticast, portCommandes, portDecouverte, portReponse, specLecteurs) {
    // Configuration de la fenêtre Raylib
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 450, "Master UI - Contrôleur MVC");
    SetWindowMinSize(800, 450);
    InitAudioDevice(); // Initialisation du moteur sonore
    SetTargetFPS(30); // Limitation à 30 images par seconde pour économiser le CPU

    // Chargement initial des données
    chargerListeVideos();

    // Lancement asynchrone de la découverte réseau (UC4)
    // Cela évite de figer l'interface au démarrage
    controleur.lancerRechercheLecteurs();

    miseAJourDisposition(); // Calculer les positions initiales des boutons
    controleur.modifierVolume(valeurVolume, estMuet);
}

V_Master::~V_Master() {
    controleur.stopper(); // Arrêter les flux réseaux
    if (textureVideo.id > 0) UnloadTexture(textureVideo); // Libérer la mémoire GPU
    CloseAudioDevice(); // Fermer le moteur sonore
    CloseWindow(); // Fermer la fenêtre
}

// --- GESTION DE LA DISPOSITION (LAYOUT) ---

void V_Master::miseAJourDisposition() {
    const auto L = static_cast<float>(GetScreenWidth());
    const auto H = static_cast<float>(GetScreenHeight());

    // Panneau gauche : Vidéos
    zones[0] = {0, 48, 150, H - 96}; // Zone de défilement (Scroll)
    zones[1] = {0, H - 48, 150, 48}; // Bouton Générer
    zones[10] = {0, 0, 150, 48}; // Bouton Dossier

    // Zone centrale : Vidéo
    zones[2] = {150, 0, L - 300, H - 72}; // Affichage vidéo
    zones[3] = {150, H - 72, L - 300, 2}; // Séparateur visuel

    // Barre de contrôle (Bas)
    zones[4] = {158, H - 40, 32, 32}; // Play/Pause
    zones[5] = {198, H - 72, L - 532, 32}; // Texte Temps (ex: 00:01 / 02:40)
    zones[6] = {198, H - 40, L - 532, 32}; // Slider de progression (Timeline)

    // Contrôles audio et vitesse (Droite du bas)
    zones[7] = {L - 326, H - 40, 32, 32}; // Icône Volume/Mute
    zones[8] = {L - 286, H - 72, 128, 32}; // Texte % Volume
    zones[9] = {L - 286, H - 40, 128, 32}; // Slider Volume
    zones[11] = {L - 286, 10.0f, 128.0f, 32.0f}; // Menu déroulant vitesse

    // Panneau droit : Lecteurs réseau
    zones[12] = {L - 150, 0, 150, H - 48}; // Zone de défilement lecteurs
    zones[13] = {L - 150, H - 48, 150, 48}; // Bouton Chercher IP
}

// --- CHARGEMENT DES DONNÉES ---

void V_Master::chargerListeVideos() {
    fichiersVideo.clear();
    videosCochees.clear();
    ordreSelection.clear();

    // Scan du dossier "videos" pour trouver les fichiers compatibles
    if (fs::exists("videos") && fs::is_directory("videos")) {
        for (const auto &entree: fs::directory_iterator("videos")) {
            if (entree.is_regular_file()) {
                string ext = entree.path().extension().string();
                // Filtrage par extension
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

    // Récupération des résultats de la découverte réseau via le contrôleur
    auto lecteurs = controleur.getDerniersLecteursTrouves();

    for (const auto &infos: lecteurs) {
        // Formatage pour l'affichage : IP - Nom (ex: 192.168.1.10 - Raspberry)
        string label = infos.at("ip") + " - " + infos.at("nom");
        lecteursIPs.push_back(label);
        lecteursCoches.push_back(false);
    }
}

// --- GETTERS DE SÉLECTION (POUR FFmpeg/UC2) ---

vector<string> V_Master::getVideosSelectionnees() const {
    vector<string> selection;
    // On parcourt la pile d'ordre pour respecter l'ordre des clics utilisateur
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
    // Crée le dossier s'il n'existe pas et l'ouvre dans l'explorateur système
    if (!fs::exists("videos")) fs::create_directory("videos");
#ifdef _WIN32
    system("explorer videos");
#elif __APPLE__
    system("open videos");
#else
    system("xdg-open videos");
#endif
}

// --- BOUCLE PRINCIPALE ---

void V_Master::executer() {
    while (!WindowShouldClose()) {
        gererLogique(); // Mettre à jour les données (Frame VLC, réseau)
        dessinerInterface(); // Afficher les éléments graphiques
    }
}

void V_Master::gererLogique() {
    // Gestion du redimensionnement dynamique de la fenêtre
    if (IsWindowResized()) miseAJourDisposition();

    // Mise à jour de l'état du contrôleur (notamment pour VLC)
    controleur.mettreAJour();

    // Vérification du thread de découverte réseau
    if (controleur.resultatsRechercheDisponibles()) {
        chargerListeLecteurs(); // Rafraîchir la liste si des lecteurs ont répondu
    }

    // --- RÉCUPÉRATION DE L'IMAGE VIDÉO ---
    void *pixels = nullptr;
    unsigned int largeur = 0;
    unsigned int hauteur = 0;
    bool redimensionnement = false;

    // Si le moteur VLC a une nouvelle frame à fournir
    if (controleur.recupererFrameVideo(pixels, largeur, hauteur, redimensionnement)) {
        largeurVideoCache = largeur;
        hauteurVideoCache = hauteur;

        // Si la résolution de la vidéo a changé (ex: changement de fichier)
        if (redimensionnement) {
            if (textureVideo.id > 0) UnloadTexture(textureVideo);
            if (largeur > 0 && hauteur > 0) {
                const Image img = GenImageColor(largeur, hauteur, BLACK);
                textureVideo = LoadTextureFromImage(img);
                UnloadImage(img);
            }
        }

        // Mise à jour des pixels dans la texture GPU
        if (largeur > 0 && hauteur > 0 && pixels != nullptr) {
            UpdateTexture(textureVideo, pixels);
        }
    }

    // --- MISE À JOUR DE LA BARRE DE PROGRESSION ---
    if (delaiRecherche > 0) delaiRecherche -= GetFrameTime();
    // Si on ne touche pas au slider, on suit la progression réelle de la vidéo
    if (!enGlissement && delaiRecherche <= 0 && controleur.getDureeTotale() > 0) {
        valeurProgression = controleur.estTermine() ? controleur.getDureeTotale() : controleur.getProgressionActuelle();
    }
}

// --- DESSIN DE L'INTERFACE ---

void V_Master::dessinerInterface() {
    BeginDrawing();
    ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

    dessinerZoneVideo(); // Rendu de la frame VLC
    DrawRectangleRec(zones[3], GRAY); // Ligne de séparation

    dessinerListeFichiers(); // Panneau gauche
    dessinerListeLecteurs(); // Panneau droit
    dessinerPanneauControle(); // Barre de contrôle basse

    gererVitesse(); // Menu dropdown
    dessinerOverlayChargement(); // Spinner si FFmpeg travaille

    EndDrawing();
}

void V_Master::dessinerZoneVideo() const {
    DrawRectangleRec(zones[2], BLACK); // Fond noir (Letterbox)

    // Calcul du ratio d'aspect pour éviter de déformer la vidéo
    if (largeurVideoCache > 0 && hauteurVideoCache > 0 && textureVideo.id > 0) {
        float echelleX = zones[2].width / static_cast<float>(largeurVideoCache);
        float echelleY = zones[2].height / static_cast<float>(hauteurVideoCache);
        const float echelle = min(echelleX, echelleY); // Garder le ratio

        const float destLargeur = static_cast<float>(largeurVideoCache) * echelle;
        const float destHauteur = static_cast<float>(hauteurVideoCache) * echelle;
        const float destX = zones[2].x + (zones[2].width - destLargeur) / 2.0f;
        const float destY = zones[2].y + (zones[2].height - destHauteur) / 2.0f;

        // Dessiner la texture vidéo avec mise à l'échelle
        DrawTexturePro(textureVideo,
                       {0.0f, 0.0f, static_cast<float>(largeurVideoCache), static_cast<float>(hauteurVideoCache)},
                       {destX, destY, destLargeur, destHauteur},
                       {0, 0}, 0.0f, WHITE);
    }
}

void V_Master::dessinerListeFichiers() {
    // Désactiver l'interaction si une tâche lourde est en cours
    bool desactiver = controleur.estGenerationEnCours() || controleur.estRechercheEnCours();
    GuiSetState(desactiver ? STATE_DISABLED : STATE_NORMAL);

    // Bouton de lancement de la génération vidéo (UC2/UC6)
    if (GuiButton(zones[1], "GÉNÉRER ET\nTRANSFÉRER")) {
        controleur.initialiserSession(getVideosSelectionnees());
    }

    // Paramètres de mise en page pour la liste scrollable
    constexpr float HAUTEUR_LIGNE = 30.0f;
    constexpr float TAILLE_CHECKBOX = 20.0f;
    constexpr float MARGE = 10.0f;
    constexpr float LARGEUR_SCROLLBAR = 16.0f;

    Rectangle vue = {0};
    const float hauteurContenu = static_cast<float>(fichiersVideo.size()) * HAUTEUR_LIGNE;
    const Rectangle zoneScroll = {0, 0, zones[0].width - LARGEUR_SCROLLBAR, hauteurContenu};

    // Panneau de défilement Raygui
    GuiScrollPanel(zones[0], nullptr, zoneScroll, &positionDefilement, &vue);

    // Mode Scissor pour ne pas dessiner en dehors de la zone de scroll
    BeginScissorMode(static_cast<int>(vue.x), static_cast<int>(vue.y), static_cast<int>(vue.width),
                     static_cast<int>(vue.height));

    for (size_t i = 0; i < fichiersVideo.size(); ++i) {
        const float posY = zones[0].y + MARGE + static_cast<float>(i) * HAUTEUR_LIGNE + positionDefilement.y;
        const Rectangle rectItem = {zones[0].x + MARGE + positionDefilement.x, posY, TAILLE_CHECKBOX, TAILLE_CHECKBOX};

        // Optimisation : ne pas dessiner si hors écran
        if (rectItem.y + rectItem.height < vue.y || rectItem.y > vue.y + vue.height) continue;

        // Gestion de l'ordre de sélection pour FFmpeg
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

        // Mise à jour de la pile d'ordre lors du clic
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

        const Rectangle rectItem = {
            zones[12].x + MARGE + positionDefilementLecteurs.x, posY, TAILLE_CHECKBOX, TAILLE_CHECKBOX
        };

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

    // Bouton Play/Pause avec icônes Raylib (#131#, #132#)
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

    // Calcul du temps pour le label
    const int minutes = static_cast<int>(valeurProgression) / 60;
    const int secondes = static_cast<int>(valeurProgression) % 60;
    const int dureeMinutes = static_cast<int>(dureeTotale) / 60;
    const int dureeSecondes = static_cast<int>(dureeTotale) % 60;

    GuiLabel(zones[5], TextFormat("%02d:%02d / %02d:%02d", minutes, secondes, dureeMinutes, dureeSecondes));

    if (desactiverSlider) GuiSetState(STATE_DISABLED);

    // Détection du début de glissement sur la timeline
    if (!desactiverSlider && CheckCollisionPointRec(GetMousePosition(), zones[6]) && IsMouseButtonPressed(
            MOUSE_LEFT_BUTTON)) {
        enGlissement = true;
        etaitEnLectureAvantGlissement = controleur.estEnLecture();
    }

    const float ancienneProg = valeurProgression;
    GuiSliderBar(zones[6], "", nullptr, &valeurProgression, 0.0f, (dureeTotale <= 0.0f) ? 1.0f : dureeTotale);

    if (desactiverSlider) GuiSetState(STATE_NORMAL);

    // Pendant le glissement, on met à jour la position en temps réel (Seek)
    if (enGlissement && valeurProgression != ancienneProg) {
        controleur.modifierProgression(valeurProgression, true);
    }

    // Fin du glissement : on rétablit l'état de lecture
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