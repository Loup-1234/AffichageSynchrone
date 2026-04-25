#pragma once

#include "Modele/M_LecteurPhysique.h"
#include "Modele/M_SessionLecture.h"
#include "Modele/M_ExpediteurUDP_W.h"
#include <string>
#include <vector>
#include <atomic>
#include <thread>
#include <functional>

using namespace std;

class C_LecteurPhysiqueLocal {
public:
    C_LecteurPhysiqueLocal(const string &ipGroupe, int port, const vector<vector<string>> &configLecteurs);
    ~C_LecteurPhysiqueLocal();

    void initialiserSession(const vector<string>& fichiersSelectionnes);
    void basculerPlayPause();
    void modifierVolume(float volume, bool muet);
    void modifierProgression(float progression, bool enGlissement);

    void mettreAJour();
    void consommerFrameVideo(const std::function<void(void* pixels, unsigned int largeur, unsigned int hauteur, bool redimensionnement)>& action);

    float getDureeTotale() const { return modeleLecteur.getDureeTotale(); }
    float getProgressionActuelle() const { return modeleLecteur.getProgressionActuelle(); }
    bool estEnLecture() const { return modeleLecteur.estEnLecture(); }
    bool estGenerationEnCours() const { return generationEnCours; }

private:
    M_LecteurPhysique modeleLecteur;
    M_ExpediteurUDP_W udp;
    M_SessionLecture session;
    vector<vector<string>> configLecteurs;

    const string cheminVideoComplexe = "videosComplexes/VideoComplexe_0.mp4";

    // Threads
    atomic<bool> videoGeneree{false};
    atomic<bool> generationEnCours{false};
    thread threadGeneration;

    void chargerVideoLocal();
};