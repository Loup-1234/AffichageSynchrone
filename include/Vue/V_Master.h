#pragma once

#include "raylib.h"
#include "Controleur/C_LecteurPhysiqueLocal.h"
#include <string>
#include <vector>

using namespace std;

/**
 * @class V_Master
 * @brief Classe gérant l'interface utilisateur (Vue) et les interactions avec le contrôleur.
 * * Utilise Raylib pour le rendu et Raygui pour les composants de contrôle.
 */
class V_Master {
public:
    /**
     * @brief Initialise la fenêtre Raylib et configure le contrôleur.
     * @param ipMulticast Adresse pour les commandes UDP.
     * @param port Port pour les commandes UDP.
     * @param specLecteurs Spécifications techniques des clients distants.
     */
    V_Master(const string &ipMulticast, int port, const vector<vector<string> > &specLecteurs);

    /**
     * @brief Ferme proprement les ressources graphiques, sonores et réseau.
     */
    ~V_Master();

    /**
     * @brief Lance la boucle de rendu et de gestion des événements.
     */
    void executer();

private:
    C_LecteurPhysiqueLocal controleur; ///< Référence vers le contrôleur (MVC).

    Texture2D textureVideo{};          ///< Texture GPU stockant la frame vidéo actuelle.
    Rectangle zones[12]{};             ///< Définition des régions rectangulaires de l'UI.

    float rotationChargement = 0.0f;   ///< Angle pour l'animation du spinner.

    unsigned int largeurVideoCache = 0; ///< Largeur de la dernière frame reçue.
    unsigned int hauteurVideoCache = 0; ///< Hauteur de la dernière frame reçue.

    float valeurProgression = 0.0f;    ///< Valeur actuelle du slider de temps.
    float valeurVolume = 100.0f;       ///< Niveau sonore (0-100).
    bool estMuet = false;              ///< État du mode muet.
    bool enGlissement = false;         ///< Indique si l'utilisateur manipule le slider de temps.
    float delaiRecherche = 0.0f;       ///< Anti-rebond après une recherche temporelle.
    bool etaitEnLectureAvantGlissement = false; ///< État mémorisé pour reprise après recherche.
    int indexVitesse = 1;              ///< Index sélectionné dans le menu vitesse.
    bool menuVitesseActif = false;     ///< État d'ouverture du menu déroulant vitesse.

    vector<string> fichiersVideo;      ///< Liste des noms de fichiers dans le dossier /videos.
    vector<bool> videosCochees;        ///< État de sélection de chaque fichier.
    vector<int> ordreSelection;        ///< Pile mémorisant l'ordre de sélection pour la composition.
    Vector2 positionDefilement = {0, 0}; ///< Position du scroll dans la liste de fichiers.

    /** @brief Recalcule la taille et la position des éléments lors d'un redimensionnement. */
    void miseAJourDisposition();

    /** @brief Scanne le dossier "videos" pour lister les médias compatibles. */
    void chargerListeVideos();

    /** @return La liste des chemins complets des vidéos sélectionnées dans l'ordre. */
    vector<string> getVideosSelectionnees() const;

    /** @brief Ouvre l'explorateur de fichiers du système dans le dossier videos. */
    static void ouvrirDossierVideos();

    /** @brief Traite les entrées utilisateur et met à jour l'état du contrôleur. */
    void gererLogique();

    /** @brief Orchestre le dessin de tous les composants de la fenêtre. */
    void dessinerInterface();

    /** @brief Affiche la texture vidéo avec maintien du ratio d'aspect. */
    void dessinerZoneVideo() const;

    /** @brief Affiche la liste des fichiers avec cases à cocher et numéros d'ordre. */
    void dessinerListeFichiers();

    /** @brief Dessine les boutons principaux (Générer, Play/Pause). */
    void dessinerPanneauControle();

    /** @brief Gère l'affichage et l'interaction avec le slider de temps. */
    void gererBarreProgression();

    /** @brief Gère l'affichage et l'interaction avec les contrôles audio. */
    void gererControlesVolume();

    /** @brief Gère le menu déroulant de sélection de vitesse. */
    void gererVitesse();

    /** @brief Affiche un écran de verrouillage durant la génération ou le transfert. */
    void dessinerOverlayChargement();
};