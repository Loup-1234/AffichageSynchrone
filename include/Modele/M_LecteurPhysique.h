#pragma once

#include <vlc.h>
#include <string>
#include <vector>
#include <atomic>
#include <mutex>
#include <functional>

using namespace std;

class M_LecteurPhysique {
public:
    M_LecteurPhysique();

    ~M_LecteurPhysique();

    void lireVideo(const string &cheminVideo);

    void consommerFrameVideo(const function<void(void *pixels, unsigned int largeur, unsigned int hauteur,
                                                 bool redimensionnement)> &action);

    void demarrer() const { libvlc_media_player_play(lecteurVLC); }
    void play() const { libvlc_media_player_set_pause(lecteurVLC, 0); }
    void pause() const { libvlc_media_player_set_pause(lecteurVLC, 1); }
    void setVolume(const int volume) const { libvlc_audio_set_volume(lecteurVLC, volume); }
    void setTime(const float tempsSec) const { libvlc_media_player_set_time(lecteurVLC, tempsSec * 1000); }

    bool estEnLecture() const { return libvlc_media_player_is_playing(lecteurVLC); }
    float getDureeTotale() const { return dureeTotale; }

    float getProgressionActuelle() const {
        libvlc_time_t t = libvlc_media_player_get_time(lecteurVLC);
        return (t != -1) ? static_cast<float>(t) / 1000.0f : 0.0f;
    }

private:
    libvlc_instance_t *instanceVLC = nullptr;
    libvlc_media_player_t *lecteurVLC = nullptr;

    vector<unsigned char> pixelsVideo;
    mutex mutexImage;
    unsigned int largeurVideo = 0, hauteurVideo = 0;
    float dureeTotale = 0.0f;

    atomic<bool> framePrete{false}, textureDoitEtreRedimensionnee{false};

    // Callbacks VLC
    static void *cb_verrouiller(void *opaque, void **plans);

    static void cb_deverrouiller(void *opaque, void *image, void *const*plans);

    static unsigned cb_configurerVideo(void **opaque, char *chrominance, const unsigned *largeur,
                                       const unsigned *hauteur, unsigned *pas, unsigned *lignes);
};
