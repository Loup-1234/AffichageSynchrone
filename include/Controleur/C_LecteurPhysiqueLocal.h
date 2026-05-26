#pragma once

#include <string>
#include <vector>
#include <thread>
#include <filesystem>
#include "Modele/M_SessionLecture.h"

using namespace std;

/**
 * @class C_LecteurPhysiqueLocal
 * @brief Contrôleur orchestrant les flux entre l'interface et le modèle de session.
 */
class C_LecteurPhysiqueLocal {
public:
    /**
     * @brief Constructeur du contrôleur.
     */
    C_LecteurPhysiqueLocal(const string &ipMulticast, int portCommandes, int portDecouverte, int portReponse,
                           const string &cheminVideoMaster);

    /**
     * @brief Initialise et lance la génération asynchrone des vidéos.
     * @param fichiers Liste des vidéos à traiter.
     * @param lecteursSelectionnes Liste des cibles réseau.
     */
    void initialiserSession(const vector<string> &fichiers, const vector<string> &lecteursSelectionnes);

    /**
     * @brief Lance la détection des lecteurs sur le réseau.
     */
    void lancerRechercheLecteurs();

private:
    M_SessionLecture session; ///< Modèle de session lecture.
    thread threadGeneration;  ///< Thread pour ne pas bloquer l'IHM.
    string m_dossierSortie = "videosComplexes";
    bool generationEnCours = false;
};