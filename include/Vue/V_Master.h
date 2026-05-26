#pragma once

#include "raylib.h"
#include "Controleur/C_LecteurPhysiqueLocal.h"
#include "Modele/M_ProtocoleReseau.h"
#include <string>
#include <vector>

using namespace std;

/**
 * @class V_Master
 * @brief Composant d'interface utilisateur principal basé sur Raylib et Raygui.
 */
class V_Master {
public:
    /**
     * @brief Initialise les fenêtres d'affichage, le sous-système audio, et charge les listes d'éléments.
     * @param ipMulticast Adresse IP utilisée pour joindre le groupe de contrôle UDP.
     * @param portCommandes Canal pour les requêtes d'ordres d'action.
     * @param portDecouverte Canal d'interrogation réseau.
     * @param portReponse Canal de traitement de configuration.
     * @param dossierSourceVideos Chemin d'accès contenant les vidéos d'entrée utilisateur.
     * @param cheminVideoMaster Fichier d'affichage Master généré attendu.
     */
    V_Master(const string &ipMulticast, int portCommandes, int portDecouverte, int portReponse,
             const string &dossierSourceVideos, const string &cheminVideoMaster);

    /**
     * @brief Libère l'ensemble des textures allouées, désactive les composants audio et ferme l'affichage graphique.
     */
    ~V_Master();

    /**
     * @brief Lance la boucle d'exécution principale de rendu de l'IHM.
     */
    void executer();

private:
    C_LecteurPhysiqueLocal controleur;
    const string m_dossierVideos;

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

    vector<string> fichiersVideo;
    vector<bool> videosCochees;
    vector<int> ordreSelection;
    Vector2 positionDefilement = {0, 0};

    vector<string> lecteursIPs;
    vector<bool> lecteursCoches;
    vector<int> ordreSelectionLecteurs;
    Vector2 positionDefilementLecteurs = {0, 0};

    void miseAJourDisposition();
    void chargerListeVideos();
    void chargerListeLecteurs();

    vector<string> getVideosSelectionnees() const;
    vector<string> getLecteursSelectionnes() const;
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