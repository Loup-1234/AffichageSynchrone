#pragma once

#include "raylib.h"
#include "Controleur/C_LecteurPhysiqueLocal.h"
#include "Modele/M_ProtocoleReseau.h" // Pour LecteurConfig
#include <string>
#include <vector>

class V_Master {
public:
    // Signature synchronisée à 7 arguments avec LecteurConfig
    V_Master(const std::string &ipMulticast, int portCommandes, int portDecouverte, int portReponse,
             const std::vector<LecteurConfig> &specLecteurs, const std::string &dossierSourceVideos, const std::string &cheminVideoMaster);
    ~V_Master();
    void executer();

private:
    C_LecteurPhysiqueLocal controleur;
    const std::string m_dossierVideos; // Stockage propre du chemin du dossier

    Texture2D textureVideo{};
    Rectangle zones[14]{};

    float rotationChargement = 0.0f;

    unsigned int largeurVideoCache = 0;
    unsigned int hauteurVideoCache = 0;

    float largeurPanneauGauche = 180.0f;
    float largeurPanneauDroit = 180.0f;
    bool enRedimensionnementGauche = false;
    bool enRedimensionnementDroit = false;

    float valeurProgression = 0.0f;
    float valeurVolume = 100.0f;
    bool estMuet = false;
    bool enGlissement = false;
    float delaiRecherche = 0.0f;
    bool etaitEnLectureAvantGlissement = false;
    int indexVitesse = 1;
    bool menuVitesseActif = false;

    std::vector<std::string> fichiersVideo;
    std::vector<bool> videosCochees;
    std::vector<int> ordreSelection;
    Vector2 positionDefilement = {0, 0};

    std::vector<std::string> lecteursIPs;
    std::vector<bool> lecteursCoches;
    std::vector<int> ordreSelectionLecteurs;
    Vector2 positionDefilementLecteurs = {0, 0};

    void miseAJourDisposition();
    void chargerListeVideos();
    void chargerListeLecteurs();

    std::vector<std::string> getVideosSelectionnees() const;
    std::vector<std::string> getLecteursSelectionnes() const;
    void ouvrirDossierVideos();

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