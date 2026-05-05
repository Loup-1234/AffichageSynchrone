#pragma once

#include "raylib.h"
#include "Controleur/C_LecteurPhysiqueLocal.h"
#include <string>
#include <vector>

using namespace std;

class V_Master {
public:
    V_Master(const string &ipMulticast, int port, const vector<vector<string> > &specLecteurs);

    ~V_Master();

    void executer();

private:
    C_LecteurPhysiqueLocal controleur;

    Texture2D textureVideo{};
    Rectangle zones[12]{};

    float rotationChargement = 0.0f;

    // Cache des dimensions pour l'affichage
    unsigned int largeurVideoCache = 0;
    unsigned int hauteurVideoCache = 0;

    float valeurProgression = 0.0f;
    float valeurVolume = 100.0f;
    bool estMuet = false;
    bool enGlissement = false;
    float delaiRecherche = 0.0f;
    bool etaitEnLectureAvantGlissement = false;
    int indexVitesse = 1;
    bool menuVitesseActif = false;

    vector<string> fichiersVideo;
    vector<bool> videosCochees;
    vector<int> ordreSelection;
    Vector2 positionDefilement = {0, 0};

    // Fonctions utilitaires
    void miseAJourDisposition();

    void chargerListeVideos();

    vector<string> getVideosSelectionnees() const;

    static void ouvrirDossierVideos();

    // Boucle principale
    void gererLogique();

    void dessinerInterface();

    // Sous-fonctions de dessin
    void dessinerZoneVideo() const;

    void dessinerListeFichiers();

    void dessinerPanneauControle();

    void gererBarreProgression();

    void gererControlesVolume();

    void gererVitesse();

    void dessinerOverlayChargement();
};
