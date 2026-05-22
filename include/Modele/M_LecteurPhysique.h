#pragma once

#include <vlc.h>
#include <string>
#include <vector>
#include <atomic>
#include <mutex>

using namespace std;

/**
 * @class M_LecteurPhysique
 * @brief Modèle gérant la lecture vidéo (LibVLC) ET les caractéristiques matérielles locales du lecteur.
 * * Cette classe fusionne les responsabilités de lecture multimédia en mémoire (pour Raylib)
 * et la collecte des métadonnées du système hôte (OS, RAM, IP) utiles pour la découverte réseau.
 */
class M_LecteurPhysique {
public:
    /**
     * @brief Constructeur par défaut.
     * Initialise l'instance VLC, le lecteur de média et les valeurs matérielles par défaut.
     */
    M_LecteurPhysique();

    /**
     * @brief Destructeur.
     * Libère proprement les ressources allouées par LibVLC.
     */
    ~M_LecteurPhysique();

    // ==========================================
    // --- PARTIE LECTURE VIDÉO (VLC) ---
    // ==========================================

    /**
     * @brief Charge et prépare une vidéo pour la lecture en mémoire.
     * @param cheminVideo Chemin local ou relatif vers le fichier vidéo à lire.
     */
    void lireVideo(const string &cheminVideo);

    /**
     * @brief Fournit les données de l'image actuelle pour l'affichage graphique (ex: Raylib).
     * @param pixels [out] Pointeur vers le tableau d'octets de l'image (Format RGBA).
     * @param largeur [out] Largeur de l'image en pixels.
     * @param hauteur [out] Hauteur de l'image en pixels.
     * @param redimensionnement [out] Indique si la résolution de la vidéo a changé depuis le dernier appel.
     * @return true si une nouvelle frame a été décodée et est prête à être affichée, false sinon.
     */
    bool recupererFrameVideo(void*& pixels, unsigned int& largeur, unsigned int& hauteur, bool& redimensionnement);

    /** @brief Démarre la lecture de la vidéo depuis le début. */
    void demarrer() const { libvlc_media_player_play(lecteurVLC); }

    /** @brief Arrête complètement la lecture de la vidéo et remet le curseur à zéro. */
    void stop() const { libvlc_media_player_stop(lecteurVLC); }

    /** @brief Reprend la lecture de la vidéo après une mise en pause. */
    void play() const { libvlc_media_player_set_pause(lecteurVLC, 0); }

    /** @brief Met la lecture de la vidéo en pause. */
    void pause() const { libvlc_media_player_set_pause(lecteurVLC, 1); }

    /**
     * @brief Définit le volume sonore du lecteur.
     * @param volume Valeur du volume (comprise entre 0 et 100).
     */
    void setVolume(const int volume) const { libvlc_audio_set_volume(lecteurVLC, volume); }

    /**
     * @brief Saute à une position temporelle spécifique dans la vidéo.
     * @param tempsSec Temps exact en secondes.
     */
    void setTime(const float tempsSec) const { libvlc_media_player_set_time(lecteurVLC, tempsSec * 1000); }

    /**
     * @brief Modifie la vitesse de lecture de la vidéo.
     * @param vitesse Facteur multiplicateur (1.0 = normal, 0.5 = ralenti, 2.0 = rapide).
     */
    void setVitesse(const float vitesse) const { libvlc_media_player_set_rate(lecteurVLC, vitesse); }

    /** @return true si la vidéo est actuellement en cours de lecture, false si en pause ou arrêtée. */
    bool estEnLecture() const { return libvlc_media_player_is_playing(lecteurVLC); }

    /** @return true si la vidéo a atteint la fin de son flux. */
    bool estTermine() const { return libvlc_media_player_get_state(lecteurVLC) == libvlc_Ended; }

    /** @return La durée totale du fichier vidéo chargé en secondes. */
    float getDureeTotale() const { return dureeTotale; }

    /** @return La position de lecture actuelle en secondes. */
    float getProgressionActuelle() const;

    // ==========================================
    // --- PARTIE CARACTÉRISTIQUES MATÉRIELLES ---
    // ==========================================

    /**
     * @brief Scanne le système hôte (Windows ou Linux) pour remplir automatiquement
     * les attributs matériels (OS, IP, MAC, Écran).
     */
    void collecterInfosLocales();

    /**
     * @brief Sérialise l'ensemble des caractéristiques matérielles et logicielles en JSON.
     * @return std::string contenant le JSON formaté pour l'envoi sur le réseau.
     */
    string versJson() const;

    // --- Getters Matériels ---
    /** @return L'adresse IPv4 locale sur le réseau. */
    const string& getIp() const { return m_ip; }
    /** @return L'adresse MAC de l'interface réseau active. */
    const string& getMac() const { return m_mac; }
    /** @return Le nom du système d'exploitation (ex: "Windows", "Linux"). */
    const string& getOs() const { return m_os; }
    /** @return La largeur de l'écran en pixels. */
    int getLargeurEcran() const { return m_largeurEcran; }
    /** @return La hauteur de l'écran en pixels. */
    int getHauteurEcran() const { return m_hauteurEcran; }

    // --- Setters Matériels ---
    /** @param ip La nouvelle adresse IP à forcer. */
    void setIp(const string& ip) { m_ip = ip; }
    /** @param mac La nouvelle adresse MAC à forcer. */
    void setMac(const string& mac) { m_mac = mac; }
    /** @param os Le nom du système d'exploitation à forcer. */
    void setOs(const string& os) { m_os = os; }
    /** @param largeur La largeur de l'écran en pixels. */
    void setLargeurEcran(int largeur) { m_largeurEcran = largeur; }
    /** @param hauteur La hauteur de l'écran en pixels. */
    void setHauteurEcran(int hauteur) { m_hauteurEcran = hauteur; }

private:
    // --- Attributs VLC ---
    libvlc_instance_t *instanceVLC = nullptr;    ///< Instance principale du moteur VLC.
    libvlc_media_player_t *lecteurVLC = nullptr; ///< Instance spécifique du lecteur de média.

    vector<unsigned char> pixelsVideo;           ///< Buffer mémoire stockant les pixels de la frame courante (RGBA).
    mutex mutexImage;                            ///< Mutex garantissant la synchronisation entre VLC (écriture) et l'IHM (lecture).

    unsigned int largeurVideo = 0;               ///< Largeur actuelle de la vidéo chargée.
    unsigned int hauteurVideo = 0;               ///< Hauteur actuelle de la vidéo chargée.
    float dureeTotale = 0.0f;                    ///< Durée totale pré-calculée de la vidéo.

    atomic<bool> framePrete{false};              ///< Flag indiquant qu'une nouvelle image a été décodée par VLC.
    atomic<bool> textureDoitEtreRedimensionnee{false}; ///< Flag indiquant un changement de résolution de la source.

    /** @brief Callback VLC : Appelé avant le décodage d'une frame pour verrouiller le buffer. */
    static void *cb_verrouiller(void *opaque, void **plans);
    /** @brief Callback VLC : Appelé après le décodage pour déverrouiller le buffer. */
    static void cb_deverrouiller(void *opaque, void *image, void *const*plans);
    /** @brief Callback VLC : Appelé à l'initialisation du média pour imposer le format RGBA. */
    static unsigned cb_configurerVideo(void **opaque, char *chrominance, const unsigned *largeur, const unsigned *hauteur, unsigned *pas, unsigned *lignes);

    // --- Attributs Matériels ---
    string m_ip;                ///< Adresse IPv4 sur le réseau local.
    string m_mac;               ///< Adresse physique MAC de la carte réseau.
    string m_os;                ///< Nom et version du système d'exploitation.
    int    m_largeurEcran = 0;  ///< Largeur de l'écran physique en pixels.
    int    m_hauteurEcran = 0;  ///< Hauteur de l'écran physique en pixels.

    /** @brief Fonction interne détectant l'adresse IP locale. */
    void collecterIp();
    /** @brief Fonction interne détectant l'adresse MAC de l'interface principale. */
    void collecterMac();
    /** @brief Fonction interne identifiant le système d'exploitation. */
    void collecterOs();
    /** @brief Fonction interne récupérant la largeur et hauteur de l'écran. */
    void collecterTailleEcran();
};