#pragma once

#include "raylib.h"
#include "Controleur/C_LecteurPhysiqueLocal.h"
#include <string>
#include <vector>

using namespace std;

/**
 * @class V_Master
 * @brief Classe de représentation de la vue principale (IHM) utilisant Raylib et Raygui.
 */
class V_Master {
public:
    /**
     * @brief Constructeur de la classe V_Master.
     * @param ipMulticast Adresse IP multicast pour les commandes réseau.
     * @param portCommandes Port pour l'envoi des commandes.
     * @param portDecouverte Port pour la découverte des lecteurs.
     * @param portReponse Port pour la réception des réponses.
     * @param dossierSourceVideos Chemin du dossier contenant les vidéos sources.
     * @param cheminVideoMaster Chemin de la vidéo master générée attendue.
     */
    V_Master(const string &ipMulticast, int portCommandes, int portDecouverte, int portReponse,
             const string &dossierSourceVideos, const string &cheminVideoMaster);

    /**
     * @brief Destructeur de la classe V_Master chargé de libérer les ressources graphiques et audio.
     */
    ~V_Master();

    /**
     * @brief Exécute la boucle principale de rendu et de gestion de l'interface utilisateur.
     */
    void executer();

private:
    C_LecteurPhysiqueLocal controleur; /**< Instance du contrôleur associé à l'interface. */
    const string m_dossierVideos; /**< Chemin du dossier de stockage des vidéos sources. */

    Texture2D textureVideo{}; /**< Texture Raylib employée pour l'affichage du flux vidéo décodé. */
    Rectangle zones[16]{}; /**< Tableau des régions de découpage de la disposition de l'interface. */

    float rotationChargement = 0.0f; /**< Angle de rotation pour l'indicateur visuel de chargement. */

    unsigned int largeurVideoCache = 0; /**< Largeur mise en cache de la vidéo courante. */
    unsigned int hauteurVideoCache = 0; /**< Hauteur mise en cache de la vidéo courante. */

    float largeurPanneauGauche = 180.0f; /**< Largeur courante du panneau latéral gauche. */
    float largeurPanneauDroit = 180.0f; /**< Largeur courante du panneau latéral droit. */
    bool enRedimensionnementGauche = false; /**< État de redimensionnement du panneau gauche par l'utilisateur. */
    bool enRedimensionnementDroit = false; /**< État de redimensionnement du panneau droit par l'utilisateur. */

    float valeurProgression = 0.0f; /**< Position temporelle actuelle du curseur de lecture. */
    float valeurVolume = 100.0f; /**< Niveau de volume sonore sélectionné de 0 à 100. */
    bool estMuet = false; /**< État de sourdine de l'audio. */
    bool enGlissement = false; /**< Indique si l'utilisateur déplace activement la barre de progression. */
    float delaiRecherche = 0.0f; /**< Temporisateur utilisé pour la réacquisition temporelle après glissement. */
    bool etaitEnLectureAvantGlissement = false; /**< Sauvegarde de l'état de lecture avant l'interaction avec la barre. */
    int indexVitesse = 1; /**< Index sélectionné pour l'option de rapidité de lecture. */
    bool menuVitesseActif = false; /**< État d'affichage déroulant du menu de sélection de vitesse. */

    vector<string> fichiersVideo; /**< Liste des noms des fichiers vidéos détectés localement. */
    vector<bool> videosCochees; /**< États de sélection associés à chaque élément de la liste des vidéos. */
    vector<int> ordreSelection; /**< Ordre d'enregistrement séquentiel des sélections de vidéos. */
    Vector2 positionDefilement = {0, 0}; /**< Position de scroll du panneau de liste des fichiers. */

    vector<string> lecteursIPs; /**< Liste des adresses IP des lecteurs réseaux trouvés. */
    vector<bool> lecteursCoches; /**< États de sélection de chacun des lecteurs distants. */
    vector<int> ordreSelectionLecteurs; /**< Ordre chronologique de sélection des terminaux réseau. */
    Vector2 positionDefilementLecteurs = {0, 0}; /**< Position de scroll du panneau de liste des terminaux. */

    /**
     * @brief Recalcule l'agencement géométrique de l'ensemble des zones de l'IHM.
     */
    void miseAJourDisposition();

    /**
     * @brief Scanne le répertoire des sources multimédias pour indexer les fichiers compatibles.
     */
    void chargerListeVideos();

    /**
     * @brief Interroge le contrôleur pour actualiser les éléments de la liste des terminaux distants.
     */
    void chargerListeLecteurs();

    /**
     * @brief Construit la liste complète des chemins absolus des fichiers vidéos sélectionnés.
     * @return Vecteur contenant les chaînes de caractères des chemins.
     */
    vector<string> getVideosSelectionnees() const;

    /**
     * @brief Récupère la liste des adresses IP sélectionnées pour la session.
     * @return Vecteur contenant les adresses IP.
     */
    vector<string> getLecteursSelectionnes() const;

    /**
     * @brief Déclenche l'ouverture du répertoire de stockage des vidéos via l'explorateur système hôte.
     */
    void ouvrirDossierVideos();

    /**
     * @brief Execute la logique de mise à jour des interactions et des frames vidéo à chaque cycle.
     */
    void gererLogique();

    /**
     * @brief Appelle les primitives de dessin pour effectuer le rendu complet de l'interface.
     */
    void dessinerInterface();

    /**
     * @brief Effectue le rendu de la texture vidéo en respectant le ratio d'aspect dans la zone centrale.
     */
    void dessinerZoneVideo() const;

    /**
     * @brief Dessine le volet de sélection des fichiers multimédia avec barre de défilement.
     */
    void dessinerListeFichiers();

    /**
     * @brief Dessine le volet de sélection des terminaux réseau.
     */
    void dessinerListeLecteurs();

    /**
     * @brief Dessine la barre basse regroupant les éléments de contrôle multimédia.
     */
    void dessinerPanneauControle();

    /**
     * @brief Assure la logique d'interaction graphique et textuelle du curseur de temps.
     */
    void gererBarreProgression();

    /**
     * @brief Organise l'affichage et l'interaction des éléments liés au volume sonore.
     */
    void gererControlesVolume();

    /**
     * @brief Traite le rendu et les changements d'états de la boîte de sélection de cadence.
     */
    void gererVitesse();

    /**
     * @brief Rendu de l'écran d'attente opaque lors d'opérations asynchrones lourdes.
     */
    void dessinerOverlayChargement();
};