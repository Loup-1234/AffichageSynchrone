#pragma once

#include "Modele/M_SessionLecture.h"
#include "Modele/M_ExpediteurUDP_W.h"
#include <vlc/vlc.h>
#include <string>
#include <vector>
#include <atomic>
#include <mutex>
#include <thread>
#include <functional>

using namespace std;

class C_LecteurPhysiqueLocal {
public:
    C_LecteurPhysiqueLocal(const string &ipGroupe, int port, const vector<vector<string>> &configLecteurs);
    ~C_LecteurPhysiqueLocal();

    void initialiserSession(const vector<string>& fichiersSelectionnes);
    void basculerPlayPause();
    void modifierVolume(float volume, bool muet);
    void modifierProgression(float progression, bool enGlissement);

    void mettreAJour();
    void consommerFrameVideo(const std::function<void(void* pixels, unsigned int largeur, unsigned int hauteur, bool redimensionnement)>& action);

    float getDureeTotale() const { return dureeTotale; }
    float getProgressionActuelle() const;
    bool estEnLecture() const;
    bool estGenerationEnCours() const { return generationEnCours; }

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
    const string cheminVideoComplexe = "videosComplexes/VideoComplexe_0.mp4";
    float dureeTotale = 0.0f;

    // Threads
    atomic<bool> videoGeneree{false};
    atomic<bool> generationEnCours{false};
    atomic<bool> framePrete{false};
    atomic<bool> textureDoitEtreRedimensionnee{false};
    thread threadGeneration;

    void initialiserVLC();
    void chargerVideoLocal();

    static void* cb_verrouiller(void* opaque, void** plans);
    static void cb_deverrouiller(void* opaque, void* image, void* const* plans);
    static unsigned cb_configurerVideo(void** opaque, char* chrominance, unsigned* largeur, unsigned* hauteur, unsigned* pas, unsigned* lignes);
};