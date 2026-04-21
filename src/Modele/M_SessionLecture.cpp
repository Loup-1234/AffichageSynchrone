#include "../../include/Modele/M_SessionLecture.h"

#include <fstream>
#include <iostream>

void M_SessionLecture::preparerSessionLecture(const vector<vector<string> > &specLecteurs) {
    IdLecteurs.clear();
    IpLecteurs.clear();
    NbVideos.clear();

    for (size_t i = 0; i < specLecteurs.size(); i++) {
        IdLecteurs.push_back(stoi(specLecteurs[i][0]));
        IpLecteurs.push_back(specLecteurs[i][1]);
        NbVideos.push_back(stoi(specLecteurs[i][2]));
    }
}

void M_SessionLecture::genererVideoComplexe(const vector<string> &listeFichierEntree) {
    if (listeFichierEntree.empty()) {
        return;
    }

    vector<vector<string> > videosParLecteur(NbVideos.size());
    int placesDisponibles = 0;

    for (size_t i = 0; i < NbVideos.size(); i++) {
        if (NbVideos[i] > 0) {
            videosParLecteur[i].push_back(listeFichierEntree[0]); // La première vidéo (audio) pour tous
        }
        placesDisponibles += (NbVideos[i] - videosParLecteur[i].size());
    }

    size_t indexVideo = 1;
    size_t lecteurActuel = 0;

    while (indexVideo < listeFichierEntree.size() && placesDisponibles > 0) {
        if (videosParLecteur[lecteurActuel].size() < NbVideos[lecteurActuel]) {
            videosParLecteur[lecteurActuel].push_back(listeFichierEntree[indexVideo]);
            indexVideo++;
            placesDisponibles--;
        }

        lecteurActuel++;
        if (lecteurActuel >= NbVideos.size()) {
            lecteurActuel = 0;
        }
    }

    for (size_t i = 0; i < videosParLecteur.size(); i++) {
        vector<string> mesVideos = videosParLecteur[i];

        if (!mesVideos.empty()) {
            string nomFichierSortie = "videosComplexes/VideoComplexe_" + to_string(IdLecteurs[i]) + ".mp4";
            M_VideoComplexe.genererVideoComplexe(mesVideos, nomFichierSortie);
        }
    }
}

void M_SessionLecture::uploaderVideoComplexe() {
    const string config = "listeLecteurs.json";
    const string path = "videosComplexes";

    ofstream fichierJson(config);

    if (!fichierJson.is_open()) {
        cerr << "Erreur: Impossible de créer le fichier JSON de configuration." << endl;
        return;
    }

    fichierJson << "[\n";
    bool premierElement = true;

    for (size_t i = 0; i < IdLecteurs.size(); i++) {
        if (NbVideos[i] > 0) {
            if (!premierElement) {
                fichierJson << ",\n";
            }
            premierElement = false;

            string pathVideo = "VideoComplexe_" + to_string(IdLecteurs[i]) + ".mp4";
            string ipActuelle = IpLecteurs[i];

            fichierJson << "  {\n";
            fichierJson << "    \"ip\": \"" << ipActuelle << "\",\n";
            fichierJson << "    \"path\": \"" << pathVideo << "\"\n";
            fichierJson << "  }";
        }
    }

    fichierJson << "\n]\n";
    fichierJson.close();

    cout << "\nFichier JSON genere. Lancement du transfert TFTP...\n" << endl;

    try {
        M_ServeurTFTP_W tftpServer(config, path);
        tftpServer.runAllTransfers();
    } catch (const exception &e) {
        cerr << "\nUne erreur critique est survenue: " << e.what() << endl;
    }
}