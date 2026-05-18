#pragma once

#include "raylib.h"
#include "Controleur/C_LecteurPhysiqueLocal.h"
#include <string>
#include <vector>

/**
 * @class V_Master
 * @brief Classe gérant l'interface utilisateur principale (Vue) du Master.
 * * Elle s'appuie sur Raylib pour le rendu graphique et Raygui pour les composants de contrôle.
 * * Elle orchestre l'affichage de la vidéo, les listes de fichiers, les lecteurs détectés,
 * et transmet les actions de l'utilisateur au Contrôleur (C_LecteurPhysiqueLocal).
 */
class V_Master {
public:
    V_Master(const std::string &ipMulticast, int portCommandes, int portDecouverte, int portReponse, const std::vector<std::vector<std::string>> &specLecteurs);
    ~V_Master();
    void executer();

private:
    C_LecteurPhysiqueLocal controleur;

    Texture2D textureVideo{};
    Rectangle zones[14]{};

    float rotationChargement = 0.0f;

    unsigned int largeurVideoCache = 0;
    unsigned int hauteurVideoCache = 0;

    // --- VARIABLES DE REDIMENSIONNEMENT DES PANNEAUX ---
    float largeurPanneauGauche = 150.0f;   ///< Largeur dynamique du panneau des vidéos.
    float largeurPanneauDroit = 150.0f;    ///< Largeur dynamique du panneau des lecteurs.
    bool enRedimensionnementGauche = false;///< Vrai si l'utilisateur glisse le bord gauche.
    bool enRedimensionnementDroit = false; ///< Vrai si l'utilisateur glisse le bord droit.

    float valeurProgression = 0.0f;
    float valeurVolume = 100.0f;
    bool estMuet = false;
    bool enGlissement = false;
    float delaiRecherche = 0.0f;
    bool etaitEnLectureAvantGlissement = false;
    int indexVitesse = 1;
    bool menuVitesseActif = false;

    // --- Gestion de la liste des vidéos (Panneau Gauche) ---
    std::vector<std::string> fichiersVideo;
    std::vector<bool> videosCochees;
    std::vector<int> ordreSelection;
    Vector2 positionDefilement = {0, 0};

    // --- Gestion de la liste des Lecteurs/IPs (Panneau Droit) ---
    std::vector<std::string> lecteursIPs;
    std::vector<bool> lecteursCoches;
    std::vector<int> ordreSelectionLecteurs;
    Vector2 positionDefilementLecteurs = {0, 0};

    void miseAJourDisposition();
    void chargerListeVideos();
    void chargerListeLecteurs();

    std::vector<std::string> getVideosSelectionnees() const;
    std::vector<std::string> getLecteursSelectionnes() const;
    static void ouvrirDossierVideos();

    void gererLogique();
    void dessinerInterface();
    void dessinerZoneVideo() const;
    void dessinerListeFichiers();
    void dessinerListeLecteurs();
    void dessinerPanneauControle();
    void gererBarreProgression();
    void gererControlesVolume();
    void gererVitesse();
    void dessinerOverlayChargement();
};