#pragma once

#include "Modele/M_LecteurPhysique.h"
#include "Modele/M_SessionLecture.h"
#include "Modele/M_UDP.h"
#include <vector>
#include <string>
#include <atomic>
#include <thread>

using namespace std;

/**
 * @class C_LecteurPhysiqueLocal
 * @brief Contrôleur supervisant le fonctionnement du lecteur local et l'état de la session multi-écrans.
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

    /** @return true si un thread d'analyse réseau est en cours d'exécution. */
    bool estRechercheEnCours() const { return rechercheEnCours; }
    /** @return true si de nouvelles données de topologie réseau ont été consolidées. */
    bool resultatsRechercheDisponibles() const { return resultatsRecherchePrets; }
    /** @brief Récupère la liste mise à jour des périphériques découverts et réinitialise le flag de disponibilité. */
    vector<map<string, string>> getDerniersLecteursTrouves();

    /** @brief Transmet les informations de frame vidéo décodées issues du modèle multimédia. */
    bool recupererFrameVideo(void*& pixels, unsigned int& largeur, unsigned int& hauteur, bool& redimensionnement) {
        return modeleLecteur.recupererFrameVideo(pixels, largeur, hauteur, redimensionnement);
    }

    float getDureeTotale() const { return modeleLecteur.getDureeTotale(); }
    float getProgressionActuelle() const { return modeleLecteur.getProgressionActuelle(); }
    bool estEnLecture() const { return modeleLecteur.estEnLecture(); }
    bool estTermine() const { return modeleLecteur.estTermine(); }
    bool estGenerationEnCours() const { return generationEnCours; }
    bool estTransfertEnCours() const { return transfertEnCours; }

private:
    float volumeCourant = 100.0f;
    const string m_cheminVideoMaster;
    const string m_dossierSortie = "videosComplexes";

    M_LecteurPhysique modeleLecteur;
    M_UDP expediteur;
    M_SessionLecture session;

    const string m_adresseMulticast;
    const int m_portDecouverte;
    const int m_portReponse;

    atomic<bool> videoGeneree{false};
    atomic<bool> generationEnCours{false};
    atomic<bool> transfertEnCours{false};
    thread threadGeneration;

    atomic<bool> rechercheEnCours{false};
    atomic<bool> resultatsRecherchePrets{false};
    thread threadRecherche;
    vector<map<string, string>> cacheLecteurs;
};