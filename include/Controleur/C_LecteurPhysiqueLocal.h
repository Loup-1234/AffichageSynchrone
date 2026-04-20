#pragma once

#include "Modele/M_SessionLecture.h"
#include "Modele/M_ExpediteurUDP_W.h"
#include <vlc/vlc.h>
#include <string>
#include <vector>
#include <atomic>
#include <mutex>
#include <thread>

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

class C_LecteurPhysiqueLocal {
public:
    C_LecteurPhysiqueLocal(const string &ipGroupe, int port, const vector<vector<string>> &configLecteurs);
    ~C_LecteurPhysiqueLocal();

    void initialiserSession(const vector<string>& fichiersSelectionnes);
    void basculerPlayPause();
    void modifierVolume(float volume, bool muet);
    void modifierProgression(float progression, bool enGlissement);

    void* getPixelsVideo();
    void lockMutexImage();
    void unlockMutexImage();
    bool getFramePrete() const { return framePrete; }
    void setFramePrete(const bool val) { framePrete = val; }

    unsigned int getLargeurVideo() const { return largeurVideo; }
    unsigned int getHauteurVideo() const { return hauteurVideo; }
    float getDureeTotale() const { return dureeTotale; }
    float getProgressionActuelle() const;
    bool estEnLecture() const;
    bool estGenerationEnCours() const { return generationEnCours; }
    bool doitRedimensionnerTexture() const { return textureDoitEtreRedimensionnee; }
    void resetRedimensionnementTexture() { textureDoitEtreRedimensionnee = false; }

    void actualiserVideoGeneree();

private:
    M_ExpediteurUDP_W udp;
    M_SessionLecture session;
    vector<vector<string>> configLecteurs;

    // VLC
    libvlc_instance_t *instanceVLC = nullptr;
    libvlc_media_player_t *lecteurVLC = nullptr;
    vector<unsigned char> pixelsVideo;
    mutex mutexImage;
    unsigned int largeurVideo = 0;
    unsigned int hauteurVideo = 0;
    string cheminVideoComplexe = "videosComplexes/VideoComplexe_0.mp4";
    float dureeTotale = 0.0f;

    // Threads
    atomic<bool> videoGeneree{false};
    atomic<bool> generationEnCours{false};
    atomic<bool> framePrete{false};
    atomic<bool> textureDoitEtreRedimensionnee{false};
    thread threadGeneration;

    void initialiserVLC();
    void chargerVideoLocal();
    void transmettreCommande(TypeCommande type, float valeur); // Signature propre

    // Callbacks C
    friend void *verrouiller(void *donnees, void **p_pixels);
    friend void deverrouiller(void *donnees);
    friend unsigned configurerVideo(void **donnees, char *chroma, const unsigned *largeur, const unsigned *hauteur, unsigned *pas, unsigned *lignes);
};