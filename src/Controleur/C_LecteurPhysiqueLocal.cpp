#include "Controleur/C_LecteurPhysiqueLocal.h"
#include <filesystem>
#include <iostream>

using namespace std;
namespace fs = filesystem;

C_LecteurPhysiqueLocal::C_LecteurPhysiqueLocal(const string &ipGroupe, int port, const vector<vector<string>> &configLecteurs)
    : udp(ipGroupe, port), configLecteurs(configLecteurs) {
    if (fs::exists(cheminVideoComplexe)) {
        chargerVideoLocal();
    }
}

C_LecteurPhysiqueLocal::~C_LecteurPhysiqueLocal() {
    if (threadGeneration.joinable()) {
        threadGeneration.join();
    }
}

void C_LecteurPhysiqueLocal::chargerVideoLocal() {
    modeleLecteur.lireVideoComplexe(cheminVideoComplexe);
}

void C_LecteurPhysiqueLocal::initialiserSession(const vector<string>& fichiersSelectionnes) {
    if (generationEnCours || fichiersSelectionnes.size() < 2) return;

    modeleLecteur.pause();
    generationEnCours = true;

    if (threadGeneration.joinable()) threadGeneration.join();

    threadGeneration = thread([this, fichiersSelectionnes]() {
        try {
            if (!fs::exists("videosComplexes")) fs::create_directory("videosComplexes");
            session.preparerSessionLecture(configLecteurs);
            session.genererVideoComplexe(fichiersSelectionnes);
            session.uploaderVideoComplexe();
            videoGeneree = true;
        } catch (const exception& e) {
            cerr << "Erreur Génération: " << e.what() << endl;
        }
        generationEnCours = false;
    });
}

void C_LecteurPhysiqueLocal::mettreAJour() {
    if (videoGeneree) {
        chargerVideoLocal();
        videoGeneree = false;
    }
}

void C_LecteurPhysiqueLocal::consommerFrameVideo(const std::function<void(void* pixels, unsigned int largeur, unsigned int hauteur, bool redimensionnement)>& action) {
    modeleLecteur.consommerFrameVideo(action);
}

void C_LecteurPhysiqueLocal::basculerPlayPause() {
    const bool enLecture = modeleLecteur.estEnLecture();
    if (enLecture) {
        modeleLecteur.pause();
    } else {
        modeleLecteur.play();
    }
    udp.transmettreCommande(TypeCommande::LECTURE_PAUSE, enLecture ? 0.0f : 1.0f);
}

void C_LecteurPhysiqueLocal::modifierVolume(const float volume, const bool muet) {
    const float volumeCible = muet ? 0.0f : volume;
    modeleLecteur.setVolume(static_cast<int>(volumeCible));
    udp.transmettreCommande(TypeCommande::VOLUME, volumeCible);
}

void C_LecteurPhysiqueLocal::modifierProgression(const float progression, const bool enGlissement) {
    if (enGlissement) {
        modeleLecteur.pause();
        modeleLecteur.setVolume(0);
    }
    modeleLecteur.setTime(progression);
    udp.transmettreCommande(TypeCommande::PROGRESSION, progression);
}