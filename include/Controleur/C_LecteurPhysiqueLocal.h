#pragma once

#include "Modele/M_LecteurPhysique.h"
#include "Modele/M_SessionLecture.h"
#include "Modele/M_ExpediteurUDP_W.h"
#include <vector>
#include <atomic>
#include <thread>
#include <functional>

using namespace std;

class C_LecteurPhysiqueLocal {
public:
    C_LecteurPhysiqueLocal(const string &ipGroupe, int port, const vector<vector<string>> &configLecteurs);
    ~C_LecteurPhysiqueLocal();

    void initialiserSession(const vector<string>& fichiers);
    void basculerPlayPause();
    void modifierVolume(float volume, bool muet);
    void modifierProgression(float progression, bool enGlissement);

    void mettreAJour();
    void consommerFrameVideo(const function<void(void*, unsigned int, unsigned int, bool)>& action) { modeleLecteur.consommerFrameVideo(action); }

    float getDureeTotale() const { return modeleLecteur.getDureeTotale(); }
    float getProgressionActuelle() const { return modeleLecteur.getProgressionActuelle(); }
    bool estEnLecture() const { return modeleLecteur.estEnLecture(); }
    bool estGenerationEnCours() const { return generationEnCours; }

private:
    float volumeCourant = 100.0f;

    M_LecteurPhysique modeleLecteur;
    M_ExpediteurUDP_W udp;
    M_SessionLecture session;
    vector<vector<string>> configLecteurs;

    static constexpr auto CHEMIN_VIDEO = "videosComplexes/VideoComplexe_0.mp4";

    atomic<bool> videoGeneree{false}, generationEnCours{false};
    thread threadGeneration;
};