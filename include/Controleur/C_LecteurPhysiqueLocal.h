#pragma once

#include "Modele/M_LecteurPhysique.h"
#include "Modele/M_SessionLecture.h"
#include "Modele/M_ExpediteurUDP.h"

#include <vector>
#include <map>
#include <atomic>
#include <thread>
#include <string>

using namespace std;

/**
 * @class C_LecteurPhysiqueLocal
 * @brief Contrôleur (MVC) orchestrant la lecture locale, la génération vidéo et le réseau Multicast/UDP.
 * * Fait le lien entre la Vue (V_Master) et les Modèles. Il gère l'envoi de commandes UDP
 * aux autres lecteurs, le traitement asynchrone des générations vidéos complexes,
 * et la découverte réseau.
 */
class C_LecteurPhysiqueLocal {
public:
    /**
     * @brief Constructeur avec IP fusionnée.
     * @param ipMulticast IP partagée pour udp (commandes) et m_adresseMulticast (découverte).
     * @param portCommandes Port pour l'envoi des ordres de lecture.
     * @param portDecouverte Port pour l'envoi de la recherche UC4.
     * @param portReponse Port d'écoute pour les retours JSON.
     * @param configLecteurs Liste des spécifications lecteurs.
     */
    C_LecteurPhysiqueLocal(const string &ipMulticast, int portCommandes, int portDecouverte, int portReponse, const vector<vector<string>> &configLecteurs);

    /**
     * @brief Destructeur de la classe.
     * Assure la fermeture propre et sécurisée de tous les threads d'arrière-plan (Réseau et Génération).
     */
    ~C_LecteurPhysiqueLocal();

    /**
     * @brief [UC2/UC6] Initialise une session de lecture.
     * Répartit les fichiers, génère les mosaïques avec FFmpeg puis les transfère en TFTP.
     * Exécuté de manière asynchrone pour ne pas bloquer l'interface graphique.
     * @param fichiers Liste des chemins relatifs des fichiers vidéos sélectionnés dans l'IHM.
     */
    void initialiserSession(const vector<string> &fichiers);

    /**
     * @brief [UC1] Bascule l'état de lecture locale (Lecture/Pause) et diffuse la commande sur le réseau.
     */
    void basculerPlayPause();

    /**
     * @brief [UC1] Modifie le volume sonore local et diffuse l'ordre de volume sur le réseau UDP.
     * @param volume Valeur du volume souhaité (0 à 100).
     * @param muet Vrai si le mode muet (mute) est activé par l'utilisateur.
     */
    void modifierVolume(float volume, bool muet);

    /**
     * @brief [UC1] Déplace le curseur de temps dans la vidéo locale et distante.
     * @param progression Position temporelle cible en secondes.
     * @param enGlissement Vrai si l'utilisateur est en train de glisser la barre (coupe le son temporairement).
     * @param restaurerLecture Vrai s'il faut relancer la lecture ("Play") une fois le clic relâché.
     */
    void modifierProgression(float progression, bool enGlissement, bool restaurerLecture = false);

    /**
     * @brief [UC1] Modifie la vitesse de lecture globale.
     * @param vitesse Facteur multiplicateur de la vitesse (ex: 1.5 pour accélérer).
     */
    void modifierVitesse(float vitesse);

    /**
     * @brief Met à jour l'état du contrôleur avec le rendu graphique.
     * Charge notamment la nouvelle vidéo une fois que le thread de génération a terminé son travail.
     * Doit être appelé à chaque frame dans la boucle principale de rendu.
     */
    void mettreAJour();

    /**
     * @brief [UC1] Arrête totalement la lecture (Stop) localement et envoie l'ordre au réseau.
     */
    void stopper();

    // ==========================================
    // --- GESTION DE LA DÉCOUVERTE RÉSEAU ---
    // ==========================================

    /**
     * @brief [UC4] Lance une recherche active des lecteurs sur le réseau via Multicast.
     * Tourne dans un thread asynchrone pour ne pas figer la Vue.
     */
    void lancerRechercheLecteurs();

    /**
     * @brief Demande l'arrêt manuel et propre de la boucle d'écoute Multicast UDP.
     */
    void arreterEcouteMulticast();

    /**
     * @brief Indique si une opération de recherche réseau est en cours de traitement.
     * @return true si le scan réseau tourne en arrière-plan.
     */
    bool estRechercheEnCours() const { return rechercheEnCours; }

    /**
     * @brief Indique si le thread de recherche a terminé et si des résultats sont disponibles.
     * @return true si de nouvelles données réseau sont prêtes à être récupérées.
     */
    bool resultatsRechercheDisponibles() const { return resultatsRecherchePrets; }

    /**
     * @brief Récupère les données des lecteurs distants trouvés et remet le flag de disponibilité à zéro.
     * @return Un vecteur de dictionnaires (std::map) contenant toutes les caractéristiques de chaque lecteur.
     */
    vector<map<string, string>> getDerniersLecteursTrouves();

    // ==========================================
    // --- RELAIS DU MODÈLE VIDÉO (Getters) ---
    // ==========================================

    /**
     * @brief Récupère l'image vidéo générée par le Modèle (VLC) pour l'afficher dans la Vue.
     * @param pixels [out] Pointeur vers le buffer d'image RGBA.
     * @param largeur [out] Largeur de l'image modifiée.
     * @param hauteur [out] Hauteur de l'image modifiée.
     * @param redimensionnement [out] Vrai si la taille de la texture a changé.
     * @return Vrai s'il y a une nouvelle image à afficher sur cette frame.
     */
    bool recupererFrameVideo(void*& pixels, unsigned int& largeur, unsigned int& hauteur, bool& redimensionnement) {
        return modeleLecteur.recupererFrameVideo(pixels, largeur, hauteur, redimensionnement);
    }

    /** @return La durée totale du média chargé en secondes. */
    float getDureeTotale() const { return modeleLecteur.getDureeTotale(); }

    /** @return La progression de lecture actuelle en secondes. */
    float getProgressionActuelle() const { return modeleLecteur.getProgressionActuelle(); }

    /** @return true si le lecteur local est actuellement sur "Play". */
    bool estEnLecture() const { return modeleLecteur.estEnLecture(); }

    /** @return true si la vidéo locale est arrivée à la fin du flux. */
    bool estTermine() const { return modeleLecteur.estTermine(); }

    /** @return true si le traitement asynchrone FFmpeg tourne actuellement en arrière-plan. */
    bool estGenerationEnCours() const { return generationEnCours; }

    /** @return true si le serveur TFTP asynchrone est en train d'envoyer les fichiers sur le réseau. */
    bool estTransfertEnCours() const { return transfertEnCours; }

private:
    float volumeCourant = 100.0f; ///< Stocke le volume en mémoire pour le rétablir après un mute ou un déplacement.

    M_LecteurPhysique modeleLecteur;         ///< Instance du moteur de lecture vidéo et des infos matérielles.
    M_ExpediteurUDP udp;                     ///< Gestionnaire d'envoi des commandes de synchronisation réseau.
    M_SessionLecture session;                ///< Gestionnaire de la logique de session, de composition et d'upload TFTP.
    vector<vector<string> > configLecteurs;  ///< Cache de la configuration réseau initiale des clients distants.

    const string m_adresseMulticast; ///< Adresse IP Multicast pour la découverte réseau.
    const int m_portDecouverte;       ///< Port Multicast pour la découverte réseau.
    const int m_portReponse;         ///< Port d'écoute pour les retours JSON des lecteurs distants.

    static constexpr auto CHEMIN_VIDEO = "videosComplexes/VideoComplexe_0.mp4"; ///< Chemin relatif de la vidéo Master générée.

    // Variables pour la synchronisation du thread de génération et lecture
    atomic<bool> videoGeneree{false};      ///< Flag indiquant à l'IHM qu'une nouvelle vidéo est prête à être chargée.
    atomic<bool> generationEnCours{false}; ///< Verrou évitant de lancer plusieurs générations FFmpeg simultanées.
    atomic<bool> transfertEnCours{false};  ///< Indique l'état actuel du transfert réseau TFTP.
    thread threadGeneration;               ///< Thread dédié aux traitements lourds (FFmpeg + Upload).

    // Variables pour la synchronisation du thread de recherche des autres lecteurs
    atomic<bool> rechercheEnCours{false};        ///< Flag verrouillant les requêtes de découvertes successives.
    atomic<bool> resultatsRecherchePrets{false}; ///< Flag notifiant l'IHM de la fin du scan réseau.
    thread threadRecherche;                      ///< Thread dédié au scan asynchrone du réseau.
    vector<map<string, string>> cacheLecteurs;   ///< Stockage temporaire des JSON parsés depuis les réponses réseau.

    // Variables pour répondre aux requêtes de découverte Multicast entrantes
    atomic<bool> ecouteMulticastActive{false}; ///< Flag permettant l'arrêt propre de la boucle d'écoute.
    thread threadEcouteMulticast;              ///< Thread résident écoutant les requêtes Multicast.

    /** * @brief Fonction bloquante exécutée par threadEcouteMulticast.
     * Écoute le port UDP et renvoie le JSON des caractéristiques locales si une requête est reçue.
     */
    void demarrerEcouteMulticast();
};