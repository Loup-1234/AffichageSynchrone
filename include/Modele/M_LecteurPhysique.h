#pragma once

#include <vlc/vlc.h>
#include <string>
#include <vector>
#include <atomic>
#include <mutex>

using namespace std;

/**
 * @class M_LecteurPhysique
 * @brief Moteur de lecture vidéo basé sur LibVLC utilisant un rendu en mémoire (callbacks).
 */
class M_LecteurPhysique {
public:
    /**
     * @brief Constructeur. Initialise l'instance VLC et le lecteur.
     */
    M_LecteurPhysique();

    /**
     * @brief Destructeur. Libère les ressources VLC et arrête les threads internes.
     */
    ~M_LecteurPhysique();

    /**
     * @brief Charge et prépare une vidéo pour la lecture.
     * @param cheminVideo Chemin local vers le fichier vidéo.
     */
    void lireVideo(const string &cheminVideo);

    /**
     * @brief Fournit les données de l'image actuelle pour l'affichage externe.
     * @param pixels [out] Pointeur vers le tableau d'octets (RGBA).
     * @param largeur [out] Largeur de l'image en pixels.
     * @param hauteur [out] Hauteur de l'image en pixels.
     * @param redimensionnement [out] Indique si le buffer a changé de taille.
     * @return true si une nouvelle frame ou un redimensionnement est disponible.
     */
    bool recupererFrameVideo(void*& pixels, unsigned int& largeur, unsigned int& hauteur, bool& redimensionnement);

    /** @brief Démarre la lecture. */
    void demarrer() const { libvlc_media_player_play(lecteurVLC); }
    /** @brief Arrête la lecture. */
    void stop() const { libvlc_media_player_stop(lecteurVLC); }
    /** @brief Reprend la lecture après une pause. */
    void play() const { libvlc_media_player_set_pause(lecteurVLC, 0); }
    /** @brief Met la lecture en pause. */
    void pause() const { libvlc_media_player_set_pause(lecteurVLC, 1); }
    /** @brief Définit le volume sonore (0-100). */
    void setVolume(const int volume) const { libvlc_audio_set_volume(lecteurVLC, volume); }
    /** @brief Définit la position temporelle en secondes. */
    void setTime(const float tempsSec) const { libvlc_media_player_set_time(lecteurVLC, tempsSec * 1000); }
    /** @brief Définit le facteur de vitesse de lecture. */
    void setVitesse(const float vitesse) const { libvlc_media_player_set_rate(lecteurVLC, vitesse); }

    /** @return true si le média est en cours de lecture. */
    bool estEnLecture() const { return libvlc_media_player_is_playing(lecteurVLC); }
    /** @return true si la vidéo a atteint la fin. */
    bool estTermine() const { return libvlc_media_player_get_state(lecteurVLC) == libvlc_Ended; }
    /** @return La durée totale du média chargé en secondes. */
    float getDureeTotale() const { return dureeTotale; }

    /**
     * @brief Calcule la position actuelle dans le média.
     * @return Progression en secondes.
     */
    float getProgressionActuelle() const {
        libvlc_time_t t = libvlc_media_player_get_time(lecteurVLC);
        return (t != -1) ? static_cast<float>(t) / 1000.0f : 0.0f;
    }

private:
    libvlc_instance_t *instanceVLC = nullptr;    ///< Instance moteur VLC.
    libvlc_media_player_t *lecteurVLC = nullptr; ///< Instance du lecteur de média.

    vector<unsigned char> pixelsVideo;           ///< Buffer de stockage des pixels (RGBA).
    mutex mutexImage;                            ///< Mutex pour synchroniser l'accès aux pixels entre VLC et l'UI.
    unsigned int largeurVideo = 0; ///< Largeur actuelle de la vidéo.
    unsigned int hauteurVideo = 0; ///< Hauteur actuelle de la vidéo.
    float dureeTotale = 0.0f;                    ///< Durée mise en cache.

    atomic<bool> framePrete{false};              ///< Indique qu'une nouvelle image a été décodée.
    atomic<bool> textureDoitEtreRedimensionnee{false}; ///< Flag de changement de résolution.

    /** @brief Callback VLC : Verrouille le buffer pour l'écriture des pixels. */
    static void *cb_verrouiller(void *opaque, void **plans);
    /** @brief Callback VLC : Déverrouille le buffer après écriture. */
    static void cb_deverrouiller(void *opaque, void *image, void *const*plans);
    /** @brief Callback VLC : Définit le format de sortie (chroma, taille). */
    static unsigned cb_configurerVideo(void **opaque, char *chrominance, const unsigned *largeur,
                                       const unsigned *hauteur, unsigned *pas, unsigned *lignes);
};