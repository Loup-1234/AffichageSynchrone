#include "../../include/Modele/M_SessionLecture.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <string>

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
            // Dossier "videosComplexes" doit exister
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

    // 1. Préparation du vecteur TransferInfo pour le serveur TFTP
    // On convertit tes tableaux internes en structure compréhensible par le serveur
    vector<TransferInfo> listeTransferts;

    for (size_t i = 1; i < nbLecteursTotal; i++) {
        if (nbVideos[i] > 0) {
            TransferInfo info;
            info.ip = ipLecteurs[i];
            // Le nom du fichier doit correspondre à celui généré dans genererVideoComplexe
            info.path = "VideoComplexe_" + to_string(idLecteurs[i]) + ".mp4";
            listeTransferts.push_back(info);
        }
    }

    // 2. Vérification si on a quelque chose à envoyer
    if (listeTransferts.empty()) {
        cout << "[SESSION] Aucun fichier à uploader." << endl;
        return;
    }

    // 3. Optionnel : Génération du JSON pour archive
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

    // 4. Lancement du serveur TFTP avec gestion des ACK
    cout << "\n[SESSION] Initialisation du serveur TFTP (Mode Fiable avec ACK)..." << endl;

    try {
        // On instancie le serveur avec la liste de transferts et le dossier racine
        M_ServeurTFTP_W serveurTftp(listeTransferts, dossierSource);

        // Lance les transferts en parallèle (multithreadé via async)
        serveurTftp.runAllTransfers();

        cout << "[SESSION] Fin de la session d'upload." << endl;
    }
    catch (const exception& e) {
        cerr << "[ERREUR CRITIQUE] Echec durant l'upload TFTP : " << e.what() << endl;
    }
}