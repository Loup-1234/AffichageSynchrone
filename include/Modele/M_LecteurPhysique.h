#pragma once

#include <vlc/vlc.h>
#include <string>
#include <vector>
#include <atomic>
#include <mutex>
#include <functional>

class M_LecteurPhysique {
public:
    M_LecteurPhysique();
    ~M_LecteurPhysique();

    void initialiserVLC();
    void lireVideoComplexe(const std::string& cheminVideo);
    
    void play();
    void pause();
    void setVolume(int volume);
    void setTime(float tempsEnSecondes);

    void consommerFrameVideo(const std::function<void(void* pixels, unsigned int largeur, unsigned int hauteur, bool redimensionnement)>& action);

    float getDureeTotale() const { return dureeTotale; }
    float getProgressionActuelle() const;
    bool estEnLecture() const;

private:
    libvlc_instance_t *instanceVLC = nullptr;
    libvlc_media_player_t *lecteurVLC = nullptr;
    std::vector<unsigned char> pixelsVideo;
    std::mutex mutexImage;
    
    unsigned int largeurVideo = 0;
    unsigned int hauteurVideo = 0;
    float dureeTotale = 0.0f;

    std::atomic<bool> framePrete{false};
    std::atomic<bool> textureDoitEtreRedimensionnee{false};

    // Callbacks VLC
    static void* cb_verrouiller(void* opaque, void** plans);
    static void cb_deverrouiller(void* opaque, void* image, void* const* plans);
    static unsigned cb_configurerVideo(void** opaque, char* chrominance, unsigned* largeur, unsigned* hauteur, unsigned* pas, unsigned* lignes);
};