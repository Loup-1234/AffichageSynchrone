#pragma once

#include "Modele/M_LecteurPhysique.h"
#include "Modele/M_SessionLecture.h"
#include "Modele/M_ExpediteurUDP.h"
#include "Modele/M_ProtocoleReseau.h"
#include <vector>
#include <string>
#include <atomic>
#include <thread>

using namespace std;

class C_LecteurPhysiqueLocal {
public:
    C_LecteurPhysiqueLocal(const string &ipMulticast, int portCommandes, int portDecouverte, int portReponse,
                           const vector<LecteurConfig> &configLecteurs, const string &cheminVideoMaster);
    ~C_LecteurPhysiqueLocal();

    void initialiserSession(const vector<string> &fichiers);
    void basculerPlayPause();
    void modifierVolume(float volume, bool muet);
    void modifierProgression(float progression, bool enGlissement, bool restaurerLecture = false);
    void modifierVitesse(float vitesse);
    void mettreAJour();
    void stopper();

    void lancerRechercheLecteurs();

    bool estRechercheEnCours() const { return rechercheEnCours; }
    bool resultatsRechercheDisponibles() const { return resultatsRecherchePrets; }
    vector<map<string, string>> getDerniersLecteursTrouves();

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
    M_ExpediteurUDP udp;
    M_SessionLecture session;
    vector<LecteurConfig> m_configLecteurs;

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