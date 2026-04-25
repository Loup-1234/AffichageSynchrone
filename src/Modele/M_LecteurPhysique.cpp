#include "Modele/M_LecteurPhysique.h"
#include <filesystem>
#include <thread>
#include <chrono>

using namespace std;
namespace fs = filesystem;

M_LecteurPhysique::M_LecteurPhysique() {
    initialiserVLC();
}

M_LecteurPhysique::~M_LecteurPhysique() {
    if (lecteurVLC) {
        libvlc_media_player_stop(lecteurVLC);
        libvlc_media_player_release(lecteurVLC);
    }
    if (instanceVLC) {
        libvlc_release(instanceVLC);
    }
}

void M_LecteurPhysique::initialiserVLC() {
    instanceVLC = libvlc_new(0, nullptr);
    if (!instanceVLC) {
        throw runtime_error("Erreur critique : Impossible d'initialiser VLC.");
    }
    lecteurVLC = libvlc_media_player_new(instanceVLC);
}

void M_LecteurPhysique::lireVideoComplexe(const string& cheminVideo) {
    if (!fs::exists(cheminVideo)) {
        dureeTotale = 0.0f;
        return;
    }

    libvlc_media_t *media = libvlc_media_new_path(instanceVLC, cheminVideo.c_str());
    if (!media) { 
        dureeTotale = 0.0f; 
        return; 
    }

    libvlc_media_player_set_media(lecteurVLC, media);
    libvlc_media_release(media);

    libvlc_video_set_callbacks(lecteurVLC, cb_verrouiller, cb_deverrouiller, nullptr, this);
    libvlc_video_set_format_callbacks(lecteurVLC, cb_configurerVideo, nullptr);

    libvlc_media_player_play(lecteurVLC);
    this_thread::sleep_for(chrono::milliseconds(200));
    libvlc_media_player_set_pause(lecteurVLC, 1);

    const libvlc_time_t len = libvlc_media_player_get_length(lecteurVLC);
    dureeTotale = (len != -1) ? static_cast<float>(len) / 1000.0f : 0.0f;
}

void M_LecteurPhysique::play() {
    libvlc_media_player_set_pause(lecteurVLC, 0);
}

void M_LecteurPhysique::pause() {
    libvlc_media_player_set_pause(lecteurVLC, 1);
}

void M_LecteurPhysique::setVolume(int volume) {
    libvlc_audio_set_volume(lecteurVLC, volume);
}

void M_LecteurPhysique::setTime(float tempsEnSecondes) {
    libvlc_media_player_set_time(lecteurVLC, static_cast<libvlc_time_t>(tempsEnSecondes * 1000));
}

bool M_LecteurPhysique::estEnLecture() const {
    return libvlc_media_player_is_playing(lecteurVLC);
}

float M_LecteurPhysique::getProgressionActuelle() const {
    const libvlc_time_t t = libvlc_media_player_get_time(lecteurVLC);
    return (t != -1) ? static_cast<float>(t) / 1000.0f : 0.0f;
}

void M_LecteurPhysique::consommerFrameVideo(const std::function<void(void* pixels, unsigned int largeur, unsigned int hauteur, bool redimensionnement)>& action) {
    lock_guard<mutex> lock(mutexImage);
    if (textureDoitEtreRedimensionnee || framePrete) {
        action(pixelsVideo.data(), largeurVideo, hauteurVideo, textureDoitEtreRedimensionnee);
        textureDoitEtreRedimensionnee = false;
        framePrete = false;
    }
}

// CALLBACKS VLC

void* M_LecteurPhysique::cb_verrouiller(void* opaque, void** plans) {
    auto* modele = static_cast<M_LecteurPhysique*>(opaque);
    modele->mutexImage.lock();
    *plans = modele->pixelsVideo.data();
    return nullptr;
}

void M_LecteurPhysique::cb_deverrouiller(void* opaque, void* /*image*/, void* const* /*plans*/) {
    auto* modele = static_cast<M_LecteurPhysique*>(opaque);
    modele->framePrete = true;
    modele->mutexImage.unlock();
}

unsigned M_LecteurPhysique::cb_configurerVideo(void** opaque, char* chrominance, unsigned* largeur, unsigned* hauteur, unsigned* pas, unsigned* lignes) {
    auto* modele = static_cast<M_LecteurPhysique*>(*opaque);
    lock_guard lock(modele->mutexImage);

    modele->largeurVideo = *largeur;
    modele->hauteurVideo = *hauteur;
    modele->pixelsVideo.resize(modele->largeurVideo * modele->hauteurVideo * 4);

    memcpy(chrominance, "RGBA", 4);
    *pas = modele->largeurVideo * 4;
    *lignes = modele->hauteurVideo;
    modele->textureDoitEtreRedimensionnee = true;

    return 1;
}