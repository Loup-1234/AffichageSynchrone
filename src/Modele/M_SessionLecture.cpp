#include "../../include/Modele/M_SessionLecture.h"
#include <fstream>
#include <iostream>
#include <vector> // Nécessaire pour TransferInfo

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
    const string dossierSource = "videosComplexes";

    // 1. Préparation du vecteur pour le serveur TFTP
    vector<TransferInfo> listeTransferts;

    for (size_t i = 0; i < nbLecteursTotal; i++) {
        if (nbVideos[i] > 0) {
            TransferInfo info;
            info.ip = ipLecteurs[i];
            info.path = "VideoComplexe_" + to_string(idLecteurs[i]) + ".mp4";
            listeTransferts.push_back(info);
        }
    }

    // 2. Generation du fichier JSON (pour archive ou autre usage)
    ofstream fichierJson("listeLecteurs.json");
    if (fichierJson.is_open()) {
        fichierJson << "[\n";
        for (size_t i = 0; i < listeTransferts.size(); i++) {
            fichierJson << "  {\n";
            fichierJson << "    \"ip\": \"" << listeTransferts[i].ip << "\",\n";
            fichierJson << "    \"path\": \"" << listeTransferts[i].path << "\"\n";
            fichierJson << "  }" << (i == listeTransferts.size() - 1 ? "" : ",");
            fichierJson << "\n";
        }
        fichierJson << "]\n";
        fichierJson.close();
    }

    // 3. Lancement du transfert
    if (listeTransferts.empty()) {
        cout << "[SESSION] Aucun transfert a effectuer." << endl;
        return;
    }

    cout << "\nLancement du streaming TFTP (Mode sans ACK)..." << endl;

    try {
        // On passe maintenant le VECTEUR et le DOSSIER SOURCE
        M_ServeurTFTP_W serveurTftp(listeTransferts, dossierSource);
        serveurTftp.runAllTransfers();
    }
    catch (const exception& e) {
        cerr << "\nErreur lors de l'upload: " << e.what() << endl;
    }
}