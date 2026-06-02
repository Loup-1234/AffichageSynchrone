/**
 * @file V_Master.h
 * @brief Déclaration de la classe V_Master gérant l'interface graphique utilisateur (IHM) du Master via Raylib.
 * @author Alain HUMEAU
 * @date 2026
 */

#pragma once

#include "raylib.h"
#include "Controleur/C_LecteurPhysiqueLocal.h"
#include <string>
#include <vector>

using namespace std;

/**
 * @class V_Master
 * @brief Classe de représentation de la vue principale (IHM) utilisant Raylib et Raygui.
 * * Cette classe orchestre l'affichage des panneaux latéraux (gestion des fichiers et des terminaux réseau),
 * le rendu du flux vidéo de contrôle, ainsi que l'interactivité des curseurs multimédias (volume, progression).
 */
class V_Master {
public:
    /**
     * @brief Constructeur de la classe V_Master.
     * * Initialise et configure l'interface graphique ainsi que le contrôleur local sous-jacent.
     * @param ipMulticast Adresse IP multicast dédiée à la diffusion des ordres réseau.
     * @param portCommandes Port d'envoi des paquets de commandes de lecture.
     * @param portDecouverte Port alloué aux requêtes de découverte des terminaux.
     * @param portReponse Port configuré pour intercepter les réponses des terminaux.
     * @param dossierSourceVideos Répertoire contenant l'ensemble des vidéos locales.
     * @param cheminVideoMaster Emplacement de la vidéo master générée attendue pour le rendu.
     */
    V_Master(const string &ipMulticast, int portCommandes, int portDecouverte, int portReponse,
             const string &dossierSourceVideos, const string &cheminVideoMaster);

    /**
     * @brief Destructeur de la classe V_Master chargé de libérer proprement les ressources graphiques et audio.
     */
    ~V_Master();

    /**
     * @brief Exécute la boucle principale de rendu de la fenêtre graphique et de gestion des événements utilisateur.
     * * Bloquant tant que la fenêtre de l'application reste ouverte.
     */
    void executer();

private:
    /**
     * @brief Recalcule dynamiquement l'agencement géométrique de l'ensemble des zones de l'IHM.
     * * S'exécute à chaque frame pour adapter l'interface aux redimensionnements des volets ou de la fenêtre.
     */
    void miseAJourDisposition();

    /**
     * @brief Scanne le répertoire des sources multimédias pour indexer les fichiers vidéo compatibles.
     */
    void chargerListeVideos();

    /**
     * @brief Interroge le contrôleur pour actualiser les éléments de la liste des terminaux distants connectés.
     */
    void chargerListeLecteurs();

    /**
     * @brief Construit la liste complète des chemins absolus des fichiers vidéos cochés par l'utilisateur.
     * @return Un vecteur contenant les chaînes de caractères représentant les chemins d'accès des vidéos.
     */
    vector<string> getVideosSelectionnees() const;

    /**
     * @brief Récupère la liste des adresses IP des lecteurs sélectionnés pour la session multi-écrans.
     * @return Un vecteur contenant les chaînes de caractères des adresses IP.
     */
    vector<string> getLecteursSelectionnes() const;

    /**
     * @brief Déclenche l'ouverture du répertoire de stockage des vidéos via l'explorateur de fichiers du système hôte.
     */
    void abrirDossierVideos(); // Note : Renommé en interne ouvrirDossierVideos selon le code fourni
    void ouvrirDossierVideos();

    /**
     * @brief Exécute la logique interne de mise à jour des interactions et de rafraîchissement des textures vidéo.
     */
    void gererLogique();

    /**
     * @brief Appelle les primitives de dessin pour effectuer le rendu graphique complet de la scène.
     */
    void dessinerInterface();

    /**
     * @brief Effectue le rendu de la texture vidéo en respectant le ratio d'aspect original dans la zone centrale.
     */
    void dessinerZoneVideo() const;

    /**
     * @brief Dessine le volet latéral gauche de sélection des fichiers multimédias avec sa barre de défilement.
     */
    void dessinerListeFichiers();

    /**
     * @brief Dessine le volet latéral droit de sélection et de statut des terminaux réseau découverts.
     */
    void dessinerListeLecteurs();

    /**
     * @brief Dessine le panneau inférieur regroupant les boutons de contrôle multimédia (Play, Pause, Stop).
     */
    void dessinerPanneauControle();

    /**
     * @brief Assure la logique d'interaction et le rendu de la barre temporelle de défilement (Slider).
     */
    void gererBarreProgression();

    /**
     * @brief Organise l'affichage et l'interaction des éléments graphiques liés au volume sonore et à la sourdine.
     */
    void gererControlesVolume();

    /**
     * @brief Traite le rendu et les changements d'états du menu déroulant lié à la cadence de lecture.
     */
    void gererVitesse();

    /**
     * @brief Affiche un écran de chargement opaque animé lors des traitements asynchrones lourds (génération/transfert).
     */
    void dessinerOverlayChargement();

    C_LecteurPhysiqueLocal controleur;           ///< Instance du contrôleur associé supervisant la logique métier.
    const string m_dossierVideos;                 ///< Chemin absolu ou relatif du dossier contenant les vidéos sources.

    Texture2D textureVideo{};                     ///< Texture GPU Raylib employée pour le rendu du flux vidéo décodé.
    Rectangle zones[16]{};                        ///< Tableau stockant les régions géométriques de découpage de la mise en page.

    float rotationChargement = 0.0f;              ///< Angle de rotation courant (en degrés) pour l'indicateur visuel de chargement.

    unsigned int largeurVideoCache = 0;           ///< Largeur mémorisée de la frame vidéo pour détecter les changements de résolution.
    unsigned int hauteurVideoCache = 0;           ///< Hauteur mémorisée de la frame vidéo pour détecter les changements de résolution.

    float largeurPanneauGauche = 180.0f;          ///< Largeur en pixels du volet de sélection des fichiers.
    float largeurPanneauDroit = 180.0f;           ///< Largeur en pixels du volet de gestion des terminaux réseau.
    bool enRedimensionnementGauche = false;       ///< Booléen indiquant si l'utilisateur étire manuellement le panneau gauche.
    bool enRedimensionnementDroit = false;        ///< Booléen indiquant si l'utilisateur étire manuellement le panneau droit.

    float valeurProgression = 0.0f;               ///< Position temporelle courante du curseur de lecture.
    float valeurVolume = 100.0f;                  ///< Niveau d'amplitude sonore globale de 0.0f à 100.0f.
    bool estMuet = false;                         ///< Flag d'activation du mode sourdine.
    bool enGlissement = false;                    ///< Indique si l'utilisateur est en train de déplacer le curseur temporel.
    float delaiRecherche = 0.0f;                  ///< Accumulateur de temps servant à temporiser la réacquisition réseau après glissement.
    bool etaitEnLectureAvantGlissement = false;   ///< Mémoire d'état de lecture avant l'interruption par glissement du curseur.
    int indexVitesse = 1;                         ///< Index sélectionné correspondant au multiplicateur de cadence.
    bool menuVitesseActif = false;                ///< Indicateur d'ouverture du composant DropdownBox de vitesse.

    vector<string> fichiersVideo;                 ///< Liste indexée des noms de fichiers vidéos locaux disponibles.
    vector<bool> videosCochees;                   ///< Tableau de correspondance d'état de sélection (coché/décoché) des vidéos.
    vector<int> ordreSelection;                   ///< Pile séquentielle enregistrant la chronologie de sélection des pistes.
    Vector2 positionDefilement = {0, 0};          ///< Vecteur de défilement (Scroll) pour le panneau de la liste de vidéos.

    vector<string> lecteursIPs;                   ///< Liste consolidée des adresses IP des terminaux distants actifs.
    vector<bool> lecteursCoches;                  ///< Tableau de correspondance d'état de sélection des terminaux distants.
    vector<int> ordreSelectionLecteurs;           ///< Pile séquentielle enregistrant la chronologie de sélection des écrans.
    Vector2 positionDefilementLecteurs = {0, 0};  ///< Vecteur de défilement (Scroll) pour le panneau de la liste des lecteurs.
};