#include "../../include/Modele/M_SessionLecture.h"

#include <fstream>
#include <iostream>

#include "../../include/Modele/M_ServeurTFTP_W.h"

M_SessionLecture::~M_SessionLecture() {
    delete[] idLecteurs;
    delete[] ipLecteurs;
    delete[] nbVideos;
}

void M_SessionLecture::preparerSessionLecture(const LecteurSpec* specLecteurs, size_t nbLecteurs) {
    delete[] idLecteurs;
    delete[] ipLecteurs;
    delete[] nbVideos;

    nbLecteursTotal = nbLecteurs;

    idLecteurs = new int[nbLecteursTotal];
    ipLecteurs = new string[nbLecteursTotal];
    nbVideos = new int[nbLecteursTotal];

    for (size_t i = 0; i < nbLecteursTotal; i++) {
        idLecteurs[i] = stoi(specLecteurs[i].id);
        ipLecteurs[i] = specLecteurs[i].ip;
        nbVideos[i] = stoi(specLecteurs[i].nbVideos);
    }
}

void M_SessionLecture::genererVideoComplexe(const string* listeFichiersEntree, size_t nbFichiers) {
    if (nbFichiers == 0 || nbLecteursTotal == 0) {
        return;
    }

    string** videosParLecteur = new string*[nbLecteursTotal];
    size_t* nbVideosAffectees = new size_t[nbLecteursTotal]();
    int placesDisponibles = 0;

    for (size_t i = 0; i < nbLecteursTotal; i++) {
        videosParLecteur[i] = new string[nbVideos[i]];

        if (nbVideos[i] > 0) {
            videosParLecteur[i][0] = listeFichiersEntree[0];
            nbVideosAffectees[i] = 1;
        }

        placesDisponibles += (nbVideos[i] - nbVideosAffectees[i]);
    }

    size_t indexVideo = 1;
    size_t lecteurActuel = 0;

    while (indexVideo < nbFichiers && placesDisponibles > 0) {
        if (nbVideosAffectees[lecteurActuel] < static_cast<size_t>(nbVideos[lecteurActuel])) {
            videosParLecteur[lecteurActuel][nbVideosAffectees[lecteurActuel]] = listeFichiersEntree[indexVideo];
            nbVideosAffectees[lecteurActuel]++;
            indexVideo++;
            placesDisponibles--;
        }

        lecteurActuel++;
        if (lecteurActuel >= nbLecteursTotal) {
            lecteurActuel = 0;
        }
    }

    for (size_t i = 0; i < nbLecteursTotal; i++) {
        if (nbVideosAffectees[i] > 0) {
            string nomFichierSortie = "videosComplexes/VideoComplexe_" + to_string(idLecteurs[i]) + ".mp4";
            instanceVideoComplexe.genererVideoComplexe(videosParLecteur[i], nbVideosAffectees[i], nomFichierSortie);
        }
    }

    for (size_t i = 0; i < nbLecteursTotal; ++i) {
        delete[] videosParLecteur[i];
    }
    delete[] videosParLecteur;
    delete[] nbVideosAffectees;
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

    for (size_t i = 0; i < nbLecteursTotal; i++) {
        if (nbVideos[i] > 0) {

            if (!premierElement) {
                fichierJson << ",\n";
            }
            premierElement = false;

            string pathVideo = "VideoComplexe_" + to_string(idLecteurs[i]) + ".mp4";
            string ipActuelle = ipLecteurs[i];

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
        M_ServeurTFTP_W serveurTftp(config, path);
        serveurTftp.runAllTransfers();
    }
    catch (const exception& e) {
        cerr << "\nUne erreur critique est survenue: " << e.what() << endl;
    }
}