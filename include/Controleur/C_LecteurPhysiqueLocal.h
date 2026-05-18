#pragma once

#include "Modele/M_LecteurPhysique.h"
#include "Modele/M_SessionLecture.h"
#include "Modele/M_ExpediteurUDP.h"

#include <vector>
#include <map>
#include <atomic>
#include <thread>
#include <string>

// NOTE : Plus de "using namespace std;" ici pour éviter la pollution globale !

/**
 * @class C_LecteurPhysiqueLocal
 * @brief Contrôleur (MVC) orchestrant la lecture locale, la génération vidéo et le réseau Multicast/UDP.
 * Fait le lien entre la Vue (V_Master) et les Modèles. Il gère l'envoi de commandes UDP
 * aux autres lecteurs, le traitement asynchrone des générations vidéos complexes,
 * et la découverte réseau.
 */
class C_LecteurPhysiqueLocal {
public:
    C_LecteurPhysiqueLocal(const std::string &ipMulticast, int portCommandes, int portDecouverte, int portReponse, const std::vector<std::vector<std::string>> &configLecteurs);
    ~C_LecteurPhysiqueLocal();

    void initialiserSession(const std::vector<std::string> &fichiers);
    void basculerPlayPause();
    void modifierVolume(float volume, bool muet);
    void modifierProgression(float progression, bool enGlissement, bool restaurerLecture = false);
    void modifierVitesse(float vitesse);
    void mettreAJour();
    void stopper();

    // --- GESTION DE LA DÉCOUVERTE RÉSEAU ---
    void lancerRechercheLecteurs();
    void arreterEcouteMulticast();

    bool estRechercheEnCours() const { return rechercheEnCours; }
    bool resultatsRechercheDisponibles() const { return resultatsRecherchePrets; }
    std::vector<std::map<std::string, std::string>> getDerniersLecteursTrouves();

    // --- RELAIS DU MODÈLE VIDÉO (Getters) ---
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
    float volumeCourant = 100.0f; ///< Stocke le volume en mémoire.

    M_LecteurPhysique modeleLecteur;         ///< Moteur de lecture vidéo.
    M_ExpediteurUDP udp;                     ///< Envoi des commandes UDP.
    M_SessionLecture session;                ///< Logique de session et TFTP.
    std::vector<std::vector<std::string>> configLecteurs;  ///< Configuration des clients distants.

    const std::string m_adresseMulticast; ///< Adresse IP Multicast.
    const int m_portDecouverte;           ///< Port pour la découverte.
    const int m_portReponse;              ///< Port pour les retours JSON.

    static constexpr auto CHEMIN_VIDEO = "videosComplexes/VideoComplexe_0.mp4"; ///< Vidéo Master générée.

    // Variables de synchronisation
    std::atomic<bool> videoGeneree{false};
    std::atomic<bool> generationEnCours{false};
    std::atomic<bool> transfertEnCours{false};
    std::thread threadGeneration;

    std::atomic<bool> rechercheEnCours{false};
    std::atomic<bool> resultatsRecherchePrets{false};
    std::thread threadRecherche;
    std::vector<std::map<std::string, std::string>> cacheLecteurs;

    std::atomic<bool> ecouteMulticastActive{false};
    std::thread threadEcouteMulticast;

    void demarrerEcouteMulticast();
};