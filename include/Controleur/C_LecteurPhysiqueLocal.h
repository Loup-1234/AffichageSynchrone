#pragma once

#include "Modele/M_LecteurPhysique.h"
#include "Modele/M_SessionLecture.h"
#include "Modele/M_ExpediteurUDP.h"
#include "Modele/M_ProtocoleReseau.h"
#include <vector>
#include <string>
#include <atomic>
#include <thread>

class C_LecteurPhysiqueLocal {
public:
    C_LecteurPhysiqueLocal(const std::string &ipMulticast, int portCommandes, int portDecouverte, int portReponse,
                           const std::vector<LecteurConfig> &configLecteurs, const std::string &cheminVideoMaster);
    ~C_LecteurPhysiqueLocal();

    void initialiserSession(const std::vector<std::string> &fichiers);
    void basculerPlayPause();
    void modifierVolume(float volume, bool muet);
    void modifierProgression(float progression, bool enGlissement, bool restaurerLecture = false);
    void modifierVitesse(float vitesse);
    void mettreAJour();
    void stopper();

    void lancerRechercheLecteurs();

    bool estRechercheEnCours() const { return rechercheEnCours; }
    bool resultatsRechercheDisponibles() const { return resultatsRecherchePrets; }
    std::vector<std::map<std::string, std::string>> getDerniersLecteursTrouves();

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
    const std::string m_cheminVideoMaster;
    const std::string m_dossierSortie = "videosComplexes";

    M_LecteurPhysique modeleLecteur;
    M_ExpediteurUDP udp;
    M_SessionLecture session;
    std::vector<LecteurConfig> m_configLecteurs;

    const std::string m_adresseMulticast;
    const int m_portDecouverte;
    const int m_portReponse;

    std::atomic<bool> videoGeneree{false};
    std::atomic<bool> generationEnCours{false};
    std::atomic<bool> transfertEnCours{false};
    std::thread threadGeneration;

    std::atomic<bool> rechercheEnCours{false};
    std::atomic<bool> resultatsRecherchePrets{false};
    std::thread threadRecherche;
    std::vector<std::map<std::string, std::string>> cacheLecteurs;
};