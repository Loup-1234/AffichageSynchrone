#include "Controleur/C_LecteurPhysiqueLocal.h"
#include <filesystem>
#include <iostream>
#include <chrono>

using namespace std;
namespace fs = filesystem;

// Callbacks VLC

void *verrouiller(void *donnees, void **p_pixels) {
    auto *ctrl = static_cast<C_LecteurPhysiqueLocal *>(donnees);
    ctrl->mutexImage.lock();
    *p_pixels = ctrl->pixelsVideo.data();
    return nullptr;
}

void deverrouiller(void *donnees) {
    auto *ctrl = static_cast<C_LecteurPhysiqueLocal *>(donnees);
    ctrl->mutexImage.unlock();
    ctrl->framePrete = true;
}

unsigned configurerVideo(void **donnees, char *chroma, const unsigned *largeur, const unsigned *hauteur, unsigned *pas, unsigned *lignes) {
    auto *ctrl = static_cast<C_LecteurPhysiqueLocal *>(*donnees);
    ctrl->mutexImage.lock();
    ctrl->largeurVideo = *largeur;
    ctrl->hauteurVideo = *hauteur;
    ctrl->pixelsVideo.resize(ctrl->largeurVideo * ctrl->hauteurVideo * 4);
    memcpy(chroma, "RGBA", 4);
    *pas = ctrl->largeurVideo * 4;
    *lignes = ctrl->hauteurVideo;
    ctrl->textureDoitEtreRedimensionnee = true;
    ctrl->mutexImage.unlock();
    return 1;
}

// Implémentation du Contrôleur

C_LecteurPhysiqueLocal::C_LecteurPhysiqueLocal(const string &ipGroupe, int port, const vector<vector<string>> &configLecteurs)
    : udp(ipGroupe, port), configLecteurs(configLecteurs) {
    initialiserVLC();
}

C_LecteurPhysiqueLocal::~C_LecteurPhysiqueLocal() {
    if (threadGeneration.joinable()) threadGeneration.join();
    if (lecteurVLC) {
        libvlc_media_player_stop(lecteurVLC);
        libvlc_media_player_release(lecteurVLC);
    }
    if (instanceVLC) libvlc_release(instanceVLC);
}

void C_LecteurPhysiqueLocal::initialiserVLC() {
    instanceVLC = libvlc_new(0, nullptr);
    if (!instanceVLC) {
        throw runtime_error("Erreur critique : Impossible d'initialiser VLC. Le dossier 'plugins' est-il manquant ?");
    }
    lecteurVLC = libvlc_media_player_new(instanceVLC);

    if (fs::exists(cheminVideoComplexe)) chargerVideoLocal();
}

void C_LecteurPhysiqueLocal::chargerVideoLocal() {
    libvlc_media_t *media = libvlc_media_new_path(instanceVLC, cheminVideoComplexe.c_str());
    if (!media) { dureeTotale = 0.0f; return; }

    libvlc_media_player_set_media(lecteurVLC, media);
    libvlc_media_release(media);

    libvlc_video_set_callbacks(lecteurVLC, verrouiller, reinterpret_cast<libvlc_video_unlock_cb>(deverrouiller), nullptr, this);
    libvlc_video_set_format_callbacks(lecteurVLC, reinterpret_cast<libvlc_video_format_cb>(configurerVideo), nullptr);

    libvlc_media_player_play(lecteurVLC);
    this_thread::sleep_for(chrono::milliseconds(200));
    libvlc_media_player_set_pause(lecteurVLC, 1);

    const libvlc_time_t len = libvlc_media_player_get_length(lecteurVLC);
    dureeTotale = (len != -1) ? static_cast<float>(len) / 1000.0f : 0.0f;
}

void C_LecteurPhysiqueLocal::initialiserSession(const vector<string>& fichiersSelectionnes) {
    if (generationEnCours || fichiersSelectionnes.size() < 2) return;

    libvlc_media_player_stop(lecteurVLC);
    dureeTotale = 0.0f;
    generationEnCours = true;

    if (threadGeneration.joinable()) threadGeneration.join();

    threadGeneration = thread([this, fichiersSelectionnes]() {
        try {
            if (!fs::exists("videosComplexes")) fs::create_directory("videosComplexes");
            session.preparerSessionLecture(configLecteurs);
            session.genererVideoComplexe(fichiersSelectionnes);
            session.uploaderVideoComplexe();
            videoGeneree = true;
        } catch (const exception& e) {
            cerr << "Erreur Génération: " << e.what() << endl;
        }
        generationEnCours = false;
    });
}

void C_LecteurPhysiqueLocal::actualiserVideoGeneree() {
    if (videoGeneree) {
        chargerVideoLocal();
        videoGeneree = false;
    }
}

// RÉSEAU UDP ET COMMANDES

void C_LecteurPhysiqueLocal::transmettreCommande(const TypeCommande type, const float valeur) {
    const PaquetControle p{0x5453454D, type, valeur};
    udp.envoyer(&p, sizeof(p));
}

void C_LecteurPhysiqueLocal::basculerPlayPause() {
    const bool enLecture = libvlc_media_player_is_playing(lecteurVLC);
    libvlc_media_player_set_pause(lecteurVLC, enLecture ? 1 : 0);
    transmettreCommande(TypeCommande::LECTURE_PAUSE, enLecture ? 0.0f : 1.0f);
}

void C_LecteurPhysiqueLocal::modifierVolume(const float volume, const bool muet) {
    const float volumeCible = muet ? 0.0f : volume;
    libvlc_audio_set_volume(lecteurVLC, static_cast<int>(volumeCible));
    transmettreCommande(TypeCommande::VOLUME, volumeCible);
}

void C_LecteurPhysiqueLocal::modifierProgression(const float progression, const bool enGlissement) {
    if (enGlissement) {
        libvlc_media_player_set_pause(lecteurVLC, 1);
        libvlc_audio_set_volume(lecteurVLC, 0);
    }
    libvlc_media_player_set_time(lecteurVLC, progression * 1000);
    transmettreCommande(TypeCommande::PROGRESSION, progression);
}

void* C_LecteurPhysiqueLocal::getPixelsVideo() { return pixelsVideo.data(); }
void C_LecteurPhysiqueLocal::lockMutexImage() { mutexImage.lock(); }
void C_LecteurPhysiqueLocal::unlockMutexImage() { mutexImage.unlock(); }
bool C_LecteurPhysiqueLocal::estEnLecture() const { return libvlc_media_player_is_playing(lecteurVLC); }

float C_LecteurPhysiqueLocal::getProgressionActuelle() const {
    const libvlc_time_t t = libvlc_media_player_get_time(lecteurVLC);
    return (t != -1) ? static_cast<float>(t) / 1000.0f : 0.0f;
}