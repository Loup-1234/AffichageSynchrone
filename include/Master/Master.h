#pragma once

#include "raylib.h"
#include <vlc/vlc.h>
#include <string>
#include <vector>
#include <atomic>
#include <mutex>
#include <thread>
#include "../ExpediteurUDP_W/ExpediteurUDP_W.h" // Vérifie ce chemin selon ton projet

using namespace std;

// --- Définitions pour le réseau ---
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

// --- Forward declarations pour les callbacks C de VLC ---
static void *verrouiller(void *donnees, void **p_pixels);
static void deverrouiller(void *donnees);
static unsigned configurerVideo(void **donnees, char *chroma, const unsigned *largeur, const unsigned *hauteur, unsigned *pas, unsigned *lignes);

/**
 * @class Master
 * @brief Gère l'interface utilisateur, la lecture VLC locale et la synchronisation UDP.
 */
class Master {
public:
    Master(const string &ipGroupe, const int port);
    ~Master();

    void executer();
    void setVolume(float volume);

private:
    ExpediteurUDP_W udp;

    // --- VLC & Gestion Vidéo ---
    libvlc_instance_t *instanceVLC = nullptr;
    libvlc_media_player_t *lecteurVLC = nullptr;
    Texture2D textureVideo{};
    vector<unsigned char> pixelsVideo;
    mutex mutexImage;
    unsigned int largeurVideo = 0;
    unsigned int hauteurVideo = 0;
    string cheminVideoComplexe = "sortie_synchro.mp4";
    float duree{};

    // --- Callbacks VLC (Amis) ---
    friend void *verrouiller(void *donnees, void **p_pixels);
    friend void deverrouiller(void *donnees);
    friend unsigned configurerVideo(void **donnees, char *chroma, const unsigned *largeur, const unsigned *hauteur, unsigned *pas, unsigned *lignes);

    // --- État de l'Interface Utilisateur ---
    float valeurSliderProgression = 0.0f;
    float valeurSliderSon = 100.0f;
    float valeurSliderSonPrecedent{};
    bool estMuet = false;
    bool playPause = false;

    // --- Gestion de la Génération ---
    atomic<bool> videoGeneree{false};
    atomic<bool> generationEnCours{false};
    atomic<bool> framePrete{false};
    atomic<bool> textureDoitEtreRedimensionnee{false};
    thread threadGeneration;

    // --- Gestion des Fichiers ---
    vector<string> fichiersVideo;
    vector<bool> videosSelectionnees;
    vector<int> ordreSelection;
    Vector2 positionDefilement = {0, 0};

    // --- Constantes UI ---
    const char *TEXTE_BOUTON_GENERER = "GÉNÉRER";
    const char *TEXTE_BOUTON_OUVRIR_DOSSIER = "DOSSIER";
    Rectangle zones[11]{};

    // --- Méthodes Internes ---
    void chargerListeVideos();
    void chargerVideo();
    void generer();
    static void ouvrirDossierVideos();
    void lecturePause();
    void son();
    void barreProgression(bool &enGlissement, bool &etaitEnLecture, float &delaiRecherche);
    void barreVolume();
    void afficherVideo();
    void afficherListeFichiers();
    void miseAJourDisposition();

    // --- Méthode Réseau ---
    void envoyerCommande(TypeCommande type, float valeur);
};