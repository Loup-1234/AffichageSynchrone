#include "Controleur/C_LecteurPhysiqueLocal.h"
#include <filesystem>
#include <iostream>

using namespace std;

C_LecteurPhysiqueLocal::C_LecteurPhysiqueLocal(const string &ipGroupe, int port, const vector<vector<string>> &configLecteurs)
    : udp(ipGroupe, port), configLecteurs(configLecteurs) {
    if (filesystem::exists(CHEMIN_VIDEO)) modeleLecteur.lireVideoComplexe(CHEMIN_VIDEO);
}

C_LecteurPhysiqueLocal::~C_LecteurPhysiqueLocal() {
    if (threadGeneration.joinable()) threadGeneration.join();
}

void C_LecteurPhysiqueLocal::initialiserSession(const vector<string>& fichiers) {
    if (generationEnCours || fichiers.size() < 2) return;

    modeleLecteur.pause();
    generationEnCours = true;

    if (threadGeneration.joinable()) threadGeneration.join();

    threadGeneration = thread([this, fichiers]() {
        try {
            if (!filesystem::exists("videosComplexes")) filesystem::create_directory("videosComplexes");
            session.preparerSessionLecture(configLecteurs);
            session.genererVideoComplexe(fichiers);
            session.uploaderVideoComplexe();
            videoGeneree = true;
        } catch (const exception& e) {
            cerr << "Erreur Génération: " << e.what() << '\n';
        }
        generationEnCours = false;
    });
}

void C_LecteurPhysiqueLocal::mettreAJour() {
    if (videoGeneree) {
        modeleLecteur.lireVideoComplexe(CHEMIN_VIDEO);
        videoGeneree = false;
    }
}

void C_LecteurPhysiqueLocal::basculerPlayPause() {
    const bool enLecture = modeleLecteur.estEnLecture();
    enLecture ? modeleLecteur.pause() : modeleLecteur.play();
    udp.transmettreCommande(TypeCommande::LECTURE_PAUSE, enLecture ? 0.0f : 1.0f);
}

void C_LecteurPhysiqueLocal::modifierVolume(const float volume, const bool muet) {
    volumeCourant = muet ? 0.0f : volume;
    modeleLecteur.setVolume(static_cast<int>(volumeCourant));
    udp.transmettreCommande(TypeCommande::VOLUME, volumeCourant);
}

void C_LecteurPhysiqueLocal::modifierProgression(const float progression, const bool enGlissement) {
    modeleLecteur.setTime(progression);
    udp.transmettreCommande(TypeCommande::PROGRESSION, progression);

    if (enGlissement) {
        modeleLecteur.pause();
        modeleLecteur.setVolume(0);
    } else {
        modeleLecteur.setVolume(static_cast<int>(volumeCourant));
        modeleLecteur.play();
        udp.transmettreCommande(TypeCommande::LECTURE_PAUSE, 0.0f);
    }
}