#pragma once

#include "Modele/M_LecteurPhysique.h"
#include "Modele/M_SessionLecture.h"
#include "Modele/M_ExpediteurUDP.h"
#include <vector>
#include <atomic>
#include <thread>

using namespace std;

/**
 * @class C_LecteurPhysiqueLocal
 * @brief Contrôleur gérant la lecture vidéo locale et la synchronisation avec des lecteurs distants via UDP.
 */
class C_LecteurPhysiqueLocal {
public:
    /**
     * @brief Constructeur de la classe.
     * @param ipMulticast Adresse IP pour la diffusion des commandes UDP.
     * @param port Port utilisé pour la communication UDP.
     * @param configLecteurs Configuration des lecteurs (ID, IP, nombre de vidéos).
     */
    C_LecteurPhysiqueLocal(const string &ipMulticast, int port, const vector<vector<string> > &configLecteurs);

    /**
     * @brief Destructeur assurant la fermeture propre des threads.
     */
    ~C_LecteurPhysiqueLocal();

    /**
     * @brief Initialise une nouvelle session de lecture et génère les fichiers nécessaires.
     * @param fichiers Liste des chemins des fichiers vidéos à traiter.
     */
    void initialiserSession(const vector<string> &fichiers);

    /**
     * @brief Bascule entre l'état de lecture et de pause.
     */
    void basculerPlayPause();

    /**
     * @brief Modifie le volume sonore local et distant.
     * @param volume Valeur du volume (0 à 100).
     * @param muet Indicateur de mode muet.
     */
    void modifierVolume(float volume, bool muet);

    /**
     * @brief Modifie la position temporelle de la vidéo.
     * @param progression Temps en secondes.
     * @param enGlissement Indique si l'utilisateur est en train de déplacer le curseur.
     * @param restaurerLecture Indique s'il faut reprendre la lecture après modification.
     */
    void modifierProgression(float progression, bool enGlissement, bool restaurerLecture = false);

    /**
     * @brief Modifie la vitesse de lecture.
     * @param vitesse Facteur de vitesse (ex: 1.0f pour normal).
     */
    void modifierVitesse(float vitesse);

    /**
     * @brief Met à jour l'état du lecteur, notamment après une génération vidéo.
     */
    void mettreAJour();

    /**
     * @brief Arrête la lecture locale et distante.
     */
    void stopper();

    /**
     * @brief Récupère les données de la frame vidéo actuelle pour l'affichage.
     * @param pixels Pointeur vers les données de pixels.
     * @param largeur Largeur de l'image retournée.
     * @param hauteur Hauteur de l'image retournée.
     * @param redimensionnement Indique si la taille a changé.
     * @return true si une frame a été récupérée, false sinon.
     */
    bool recupererFrameVideo(void*& pixels, unsigned int& largeur, unsigned int& hauteur, bool& redimensionnement) {
        return modeleLecteur.recupererFrameVideo(pixels, largeur, hauteur, redimensionnement);
    }

    /** @return La durée totale de la vidéo en secondes. */
    float getDureeTotale() const { return modeleLecteur.getDureeTotale(); }
    /** @return La progression actuelle en secondes. */
    float getProgressionActuelle() const { return modeleLecteur.getProgressionActuelle(); }
    /** @return true si la vidéo est en cours de lecture. */
    bool estEnLecture() const { return modeleLecteur.estEnLecture(); }
    /** @return true si la vidéo est arrivée à la fin. */
    bool estTermine() const { return modeleLecteur.estTermine(); }
    /** @return true si une opération de génération est en cours. */
    bool estGenerationEnCours() const { return generationEnCours; }
    /** @return true si l'upload vers les clients est en cours. */
    bool estTransfertEnCours() const { return transfertEnCours; }

private:
    float volumeCourant = 100.0f; ///< Stocke le volume avant passage en mode muet.

    M_LecteurPhysique modeleLecteur; ///< Instance du moteur de lecture vidéo.
    M_ExpediteurUDP udp;             ///< Gestionnaire d'envoi des commandes réseau.
    M_SessionLecture session;        ///< Gestionnaire de la logique de session et composition.
    vector<vector<string> > configLecteurs; ///< Configuration réseau des clients.

    static constexpr auto CHEMIN_VIDEO = "videosComplexes/VideoComplexe_0.mp4"; ///< Chemin de la vidéo générée.

    atomic<bool> videoGeneree{false};     ///< Flag indiquant qu'une nouvelle vidéo est prête à être chargée.
    atomic<bool> generationEnCours{false}; ///< Verrou pour éviter plusieurs générations simultanées.
    atomic<bool> transfertEnCours{false};  ///< État du transfert réseau.
    thread threadGeneration;               ///< Thread dédié aux traitements lourds (FFmpeg/Upload).
};