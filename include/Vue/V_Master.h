#pragma once

#include "raylib.h"
#include "Controleur/C_LecteurPhysiqueLocal.h"
#include <string>
#include <vector>

using namespace std;

/**
 * @class V_Master
 * @brief Classe gérant l'interface utilisateur principale (Vue) du Master.
 * * Elle s'appuie sur Raylib pour le rendu graphique et Raygui pour les composants de contrôle.
 * * Elle orchestre l'affichage de la vidéo, les listes de fichiers, les lecteurs détectés,
 * et transmet les actions de l'utilisateur au Contrôleur (C_LecteurPhysiqueLocal).
 */
class V_Master {
public:
    /**
     * @brief Constructeur aligné sur l'appel du main.
     * @param ipMulticast Adresse IP unique pour tous les flux Multicast (Commandes et Découverte).
     * @param portCommandes Port UDP pour la synchronisation Play/Pause.
     * @param portDecouverte Port UDP pour l'envoi de la découverte réseau.
     * @param portReponse Port UDP local pour recevoir les réponses JSON.
     * @param specLecteurs Configuration technique initiale issue de la BDD.
     */
    V_Master(const string &ipMulticast, int portCommandes, int portDecouverte, int portReponse, const vector<vector<string>> &specLecteurs);

    /**
     * @brief Destructeur.
     * Ferme proprement les ressources graphiques (fenêtre, textures), le module audio,
     * et demande l'arrêt du contrôleur.
     */
    ~V_Master();

    /**
     * @brief Point d'entrée de la Vue. Lance la boucle de rendu principale (bloquante).
     * Gère les événements, met à jour la logique et dessine l'interface à chaque frame.
     */
    void executer();

private:
    C_LecteurPhysiqueLocal controleur; ///< Référence vers le contrôleur (Architecture MVC).

    Texture2D textureVideo{};          ///< Texture GPU stockant la frame vidéo actuellement décodée par VLC.
    Rectangle zones[14]{};             ///< Définition des régions rectangulaires de l'UI (boutons, listes, sliders).

    float rotationChargement = 0.0f;   ///< Angle de rotation pour l'animation de l'icône de chargement.

    unsigned int largeurVideoCache = 0; ///< Largeur en pixels de la dernière frame vidéo reçue.
    unsigned int hauteurVideoCache = 0; ///< Hauteur en pixels de la dernière frame vidéo reçue.

    float valeurProgression = 0.0f;     ///< Valeur actuelle de la barre de progression (en secondes).
    float valeurVolume = 100.0f;        ///< Niveau sonore actuel (0-100).
    bool estMuet = false;               ///< État d'activation du mode muet (Mute).
    bool enGlissement = false;          ///< Vrai si l'utilisateur est en train de manipuler le slider de la timeline.
    float delaiRecherche = 0.0f;        ///< Compteur anti-rebond appliqué après une recherche temporelle (Seek).
    bool etaitEnLectureAvantGlissement = false; ///< Mémorise si la vidéo était en lecture avant de toucher la timeline.
    int indexVitesse = 1;               ///< Index de la vitesse sélectionnée dans la liste déroulante (0.5x, 1.0x, etc.).
    bool menuVitesseActif = false;      ///< Vrai si le menu déroulant de la vitesse est ouvert.

    // --- Gestion de la liste des vidéos (Panneau Gauche) ---
    vector<string> fichiersVideo;       ///< Liste des noms de fichiers multimédias trouvés dans le dossier local.
    vector<bool> videosCochees;         ///< État de sélection (coché/décoché) de chaque fichier.
    vector<int> ordreSelection;         ///< Pile mémorisant l'ordre chronologique de sélection pour la composition FFmpeg.
    Vector2 positionDefilement = {0, 0};///< État du défilement (scroll) dans la liste des fichiers.

    // --- Gestion de la liste des Lecteurs/IPs (Panneau Droit) ---
    vector<string> lecteursIPs;         ///< Liste formattée (IP - Nom) des lecteurs physiques détectés sur le réseau.
    vector<bool> lecteursCoches;        ///< État de sélection de chaque lecteur physique.
    vector<int> ordreSelectionLecteurs; ///< Pile mémorisant l'ordre d'affectation des lecteurs sélectionnés.
    Vector2 positionDefilementLecteurs = {0, 0}; ///< État du défilement (scroll) dans la liste des lecteurs.

    /** @brief Recalcule la taille et la position de tous les éléments d'interface lors d'un redimensionnement. */
    void miseAJourDisposition();

    /** @brief Scanne le dossier local "videos" pour lister les médias compatibles (.mp4, .avi, etc.). */
    void chargerListeVideos();

    /** @brief Interroge le contrôleur pour récupérer la liste des IP des lecteurs physiques trouvés. */
    void chargerListeLecteurs();

    /** @return La liste des chemins complets des vidéos sélectionnées dans l'ordre du clic. */
    vector<string> getVideosSelectionnees() const;

    /** @return La liste des adresses IP sélectionnées dans l'ordre du clic. */
    vector<string> getLecteursSelectionnes() const;

    /** @brief Ouvre l'explorateur de fichiers natif de l'OS (Windows/Mac/Linux) dans le dossier "videos". */
    static void ouvrirDossierVideos();

    /** @brief Traite les entrées utilisateur et synchronise l'état local (sliders, frames) avec le Contrôleur. */
    void gererLogique();

    /** @brief Orchestre le dessin de tous les composants de la fenêtre à chaque frame. */
    void dessinerInterface();

    /** @brief Affiche la texture vidéo centrale avec un maintien automatique du ratio d'aspect (Letterbox). */
    void dessinerZoneVideo() const;

    /** @brief Dessine le panneau latéral gauche (Liste des fichiers, Bouton Générer et Bouton Dossier). */
    void dessinerListeFichiers();

    /** @brief Dessine le panneau latéral droit (Liste des IP, Boutons Chercher et Valider). */
    void dessinerListeLecteurs();

    /** @brief Dessine le bandeau inférieur regroupant les contrôles de lecture (Play/Pause, Volume, Timeline). */
    void dessinerPanneauControle();

    /** @brief Gère l'affichage, le défilement et l'interaction avec le curseur temporel (Timeline). */
    void gererBarreProgression();

    /** @brief Gère l'affichage et l'interaction avec le bouton muet et la jauge de volume. */
    void gererControlesVolume();

    /** @brief Gère l'affichage et la sélection du menu déroulant modifiant la vitesse de lecture. */
    void gererVitesse();

    /** @brief Affiche un écran semi-transparent bloquant l'UI pendant les opérations asynchrones (Génération/Réseau). */
    void dessinerOverlayChargement();
};