/**
 * @file C_LecteurPhysiqueLocal.h
 * @brief Déclaration de la classe C_LecteurPhysiqueLocal chargée de la supervision du lecteur local et du réseau.
 * @author Alain HUMEAU
 * @date 2026
 */

#pragma once

#include "Modele/M_LecteurPhysique.h"
#include "Modele/M_SessionLecture.h"
#include "Modele/M_UDP.h"
#include <vector>
#include <string>
#include <atomic>
#include <thread>
#include <map>

using namespace std;

/**
 * @class C_LecteurPhysiqueLocal
 * @brief Contrôleur supervisant le fonctionnement du lecteur local et l'état de la session multi-écrans.
 * * Cette classe fait le lien entre les modèles de lecture physique, de session et de communication UDP.
 * Elle gère également les traitements asynchrones via des threads pour la recherche de périphériques
 * et la génération de médias.
 */
class C_LecteurPhysiqueLocal {
public:
    /**
     * @brief Constructeur configurant les paramètres de communication et chargeant le média du Master.
     * @param ipMulticast Adresse IP du groupe de diffusion UDP.
     * @param portCommandes Port de transmission des ordres d'exécution.
     * @param portDecouverte Port d'émission pour la découverte réseau.
     * @param portReponse Port de réception des rapports de configuration.
     * @param cheminVideoMaster Emplacement local de la vidéo dédiée à l'affichage principal.
     */
    C_LecteurPhysiqueLocal(const string &ipMulticast, int portCommandes, int portDecouverte, int portReponse,
                           const string &cheminVideoMaster);

    /**
     * @brief Destructeur assurant la synchronisation et la fermeture propre des threads de traitement secondaires.
     */
    ~C_LecteurPhysiqueLocal();

    /**
     * @brief Initialise de manière asynchrone la composition et le transfert des mosaïques vidéos aux terminaux.
     * @param fichiers Liste de l'ensemble des fichiers sources sélectionnés.
     * @param lecteursSelectionnes Adresses IP des terminaux distants désignés comme actifs.
     */
    void initialiserSession(const vector<string> &fichiers, const vector<string> &lecteursSelectionnes);

    /**
     * @brief Inverse l'état actuel d'exécution du lecteur (Lecture / Pause) et notifie le réseau.
     */
    void basculerPlayPause();

    /**
     * @brief Modifie le volume audio local et distant.
     * @param volume Valeur numérique du volume sonore de 0 à 100.
     * @param muet Indique si le mode sourdine doit être activé.
     */
    void modifierVolume(float volume, bool muet);

    /**
     * @brief Repositionne le curseur temporel de lecture.
     * @param progression Temps ciblé exprimé en secondes.
     * @param enGlissement Spécifie si l'utilisateur interagit activement avec le curseur de défilement.
     * @param restaurerLecture Spécifie si la lecture doit reprendre après le relâchement du curseur.
     */
    void modifierProgression(float progression, bool enGlissement, bool restaurerLecture = false);

    /**
     * @brief Ajuste la vitesse d'exécution temporelle du média.
     * @param vitesse Multiplicateur de cadence d'affichage.
     */
    void modifierVitesse(float vitesse);

    /**
     * @brief Effectue la mise à jour des états internes et rafraîchit le lien vers la vidéo Master si prête.
     */
    void mettreAJour();

    /**
     * @brief Stoppe la lecture vidéo et transmet l'ordre d'arrêt global au réseau.
     */
    void stopper();

    /**
     * @brief Déclenche de façon non bloquante la phase d'interrogation et de détection des terminaux réseaux.
     */
    void lancerRechercheLecteurs();

    /** * @brief Vérifie si une recherche réseau est active.
     * @return true si un thread d'analyse réseau est en cours d'exécution.
     */
    bool estRechercheEnCours() const { return rechercheEnCours; }

    /** * @brief Vérifie la disponibilité de nouveaux résultats.
     * @return true si de nouvelles données de topologie réseau ont été consolidées.
     */
    bool resultatsRechercheDisponibles() const { return resultatsRecherchePrets; }

    /** * @brief Récupère la liste mise à jour des périphériques découverts et réinitialise le flag de disponibilité.
     * @return Un vecteur de dictionnaires contenant les propriétés des lecteurs trouvés.
     */
    vector<map<string, string> > getDerniersLecteursTrouves();

    /** * @brief Transmet les informations de frame vidéo décodées issues du modèle multimédia.
     * @param[out] pixels Pointeur de réception vers les données brutes des pixels.
     * @param[out] largeur Variable de réception pour la largeur de l'image.
     * @param[out] hauteur Variable de réception pour la hauteur de l'image.
     * @param[out] redimensionnement Booléen indiquant si un changement de dimension a eu lieu.
     * @return true si une nouvelle frame a été récupérée avec succès.
     */
    bool recupererFrameVideo(void *&pixels, unsigned int &largeur, unsigned int &hauteur, bool &redimensionnement) {
        return modeleLecteur.recupererFrameVideo(pixels, largeur, hauteur, redimensionnement);
    }

    /** * @brief Récupère la durée totale du média chargé.
     * @return Durée en secondes.
     */
    float getDureeTotale() const { return modeleLecteur.getDureeTotale(); }

    /** * @brief Récupère la progression temporelle actuelle.
     * @return Position actuelle en secondes.
     */
    float getProgressionActuelle() const { return modeleLecteur.getProgressionActuelle(); }

    /** * @brief Vérifie si le lecteur local est en cours de lecture.
     * @return true si le média défile.
     */
    bool estEnLecture() const { return modeleLecteur.estEnLecture(); }

    /** * @brief Vérifie si le média est arrivé à son terme.
     * @return true si la vidéo est finie.
     */
    bool estTermine() const { return modeleLecteur.estTermine(); }

    /** * @brief Indique si la génération asynchrone d'une vidéo est active.
     * @return true si le thread de génération travaille.
     */
    bool estGenerationEnCours() const { return generationEnCours; }

    /** * @brief Indique si le transfert réseau de médias est en cours.
     * @return true si un envoi est actif.
     */
    bool estTransfertEnCours() const { return transfertEnCours; }

    /** * @brief Récupère la configuration courante de la session et réinitialise le flag réseau.
     * @return Matrice de chaînes de caractères représentant la configuration.
     */
    vector<vector<string> > getConfig() {
        resultatsRecherchePrets = false;
        return session.getConfig();
    }

    /** @brief Sauvegarde la configuration actuelle de la session sur le disque. */
    void sauvegarderConfig() { session.sauvegarderConfig(); }

    /** @brief Charge la configuration de la session depuis le fichier de sauvegarde. */
    void chargerConfig() { session.chargerConfig(); }

private:
    float volumeCourant = 100.0f; ///< Volume sonore courant du lecteur (0.0 à 100.0).
    const string m_cheminVideoMaster; ///< Chemin d'accès local de la vidéo principale.
    const string m_dossierSortie = "videosComplexes"; ///< Répertoire de destination pour les vidéos générées.

    M_LecteurPhysique modeleLecteur; ///< Instance du modèle de lecture physique locale.
    M_UDP expediteur; ///< Instance du gestionnaire de protocole réseau UDP.
    M_SessionLecture session; ///< Instance assurant la gestion de la session de lecture partagée.

    const string m_adresseMulticast; ///< Adresse IP utilisée pour le groupe multicast.
    const int m_portDecouverte; ///< Port réseau affecté aux requêtes de découverte.
    const int m_portReponse; ///< Port réseau dédié à la réception des réponses.

    atomic<bool> videoGeneree{false}; ///< Flag atomique indiquant si le traitement vidéo est terminé.
    atomic<bool> generationEnCours{false}; ///< Flag atomique signalant qu'une génération est active.
    atomic<bool> transfertEnCours{false}; ///< Flag atomique signalant qu'un transfert réseau est en cours.
    thread threadGeneration; ///< Thread secondaire alloué aux calculs de génération vidéo.

    atomic<bool> rechercheEnCours{false}; ///< Flag atomique indiquant qu'une recherche réseau est active.
    atomic<bool> resultatsRecherchePrets{false}; ///< Flag atomique indiquant que le cache des lecteurs a changé.
    thread threadRecherche; ///< Thread secondaire dédié à l'écoute des terminaux réseau.
    vector<map<string, string> > cacheLecteurs; ///< Tampon de stockage des informations des terminaux détectés.
};
