#include "Modele/M_LecteurPhysique.h"

#include <filesystem>
#include <thread>
#include <chrono>

using namespace std;

M_LecteurPhysique::M_LecteurPhysique() {
    instanceVLC = libvlc_new(0, nullptr);
    lecteurVLC = libvlc_media_player_new(instanceVLC);
}

M_LecteurPhysique::~M_LecteurPhysique() {
    if (lecteurVLC) { libvlc_media_player_release(lecteurVLC); }
    if (instanceVLC) { libvlc_release(instanceVLC); }
}

void M_LecteurPhysique::lireVideo(const string &cheminVideo) {
    if (!filesystem::exists(cheminVideo)) return;

    libvlc_media_t *media = libvlc_media_new_path(instanceVLC, cheminVideo.c_str());
    if (!media) return;

    libvlc_media_player_set_media(lecteurVLC, media);
    libvlc_media_release(media);

    libvlc_video_set_callbacks(lecteurVLC, cb_verrouiller, cb_deverrouiller, nullptr, this);
    libvlc_video_set_format_callbacks(lecteurVLC, libvlc_video_format_cb(cb_configurerVideo), nullptr);

    demarrer();
    this_thread::sleep_for(chrono::milliseconds(200));
    pause();

    const libvlc_time_t len = libvlc_media_player_get_length(lecteurVLC);
    dureeTotale = (len != -1) ? static_cast<float>(len) / 1000.0f : 0.0f;
}

bool M_LecteurPhysique::recupererFrameVideo(void*& outPixels, unsigned int& outLargeur, unsigned int& outHauteur, bool& outRedimensionnement) {
    lock_guard lock(mutexImage);

    if (textureDoitEtreRedimensionnee || framePrete) {
        outPixels = pixelsVideo.data();
        outLargeur = largeurVideo;
        outHauteur = hauteurVideo;
        outRedimensionnement = textureDoitEtreRedimensionnee;

        textureDoitEtreRedimensionnee = false;
        framePrete = false;

        return true;
    }

    return false;
}

// --- CALLBACKS VLC ---
void *M_LecteurPhysique::cb_verrouiller(void *opaque, void **plans) {
    auto *mod = static_cast<M_LecteurPhysique *>(opaque);
    mod->mutexImage.lock();
    *plans = mod->pixelsVideo.data();
    return nullptr;
}

void M_LecteurPhysique::cb_deverrouiller(void *opaque, void *, void *const*) {
    auto *mod = static_cast<M_LecteurPhysique *>(opaque);
    mod->framePrete = true;
    mod->mutexImage.unlock();
}

unsigned M_LecteurPhysique::cb_configurerVideo(void **opaque, char *chrominance, const unsigned *largeur,
                                               const unsigned *hauteur, unsigned *pas, unsigned *lignes) {
    auto *mod = static_cast<M_LecteurPhysique *>(*opaque);
    lock_guard lock(mod->mutexImage);

    mod->largeurVideo = *largeur;
    mod->hauteurVideo = *hauteur;
    mod->pixelsVideo.resize(*largeur * *hauteur * 4);

    memcpy(chrominance, "RGBA", 4);
    *pas = *largeur * 4;
    *lignes = *hauteur;
    mod->textureDoitEtreRedimensionnee = true;

    return 1;
}