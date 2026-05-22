#include "Modele/M_SessionLecture.h"
#include "Modele/M_TFTP_W.h"
#include "Modele/M_JsonUtil.h"
#include "Modele/M_DecouverteReseau.h"

#include <fstream>
#include <iostream>
#include <vector>
#include <string>

M_SessionLecture::~M_SessionLecture() {
    // Libération de la mémoire allouée dynamiquement dans preparerSessionLecture
    delete[] idLecteurs;
    delete[] ipLecteurs;
    delete[] nbVideos;
}

void M_SessionLecture::preparerSessionLecture(const LecteurSpec *specLecteurs, size_t nbLecteurs) {
    // Nettoyage d'une éventuelle session précédente
    delete[] idLecteurs;
    delete[] ipLecteurs;
    delete[] nbVideos;

    nbLecteursTotal = nbLecteurs;

    idLecteurs = new int[nbLecteursTotal];
    ipLecteurs = new string[nbLecteursTotal];
    nbVideos = new int[nbLecteursTotal];

    // Conversion des chaînes de caractères en types numériques exploitables
    for (size_t i = 0; i < nbLecteursTotal; i++) {
        idLecteurs[i] = stoi(specLecteurs[i].id);
        ipLecteurs[i] = specLecteurs[i].ip;
        nbVideos[i] = stoi(specLecteurs[i].nbVideos);
    }
}

void M_SessionLecture::genererVideoComplexe(const string *listeFichiersEntree, size_t nbFichiers) {
    if (nbFichiers == 0 || nbLecteursTotal == 0) {
        return;
    }

    // Allocation d'un tableau de tableaux pour stocker les listes de fichiers par lecteur
    string **videosParLecteur = new string *[nbLecteursTotal];
    size_t *nbVideosAffectees = new size_t[nbLecteursTotal]();
    int placesDisponibles = 0;

    for (size_t i = 0; i < nbLecteursTotal; i++) {
        videosParLecteur[i] = new string[nbVideos[i]];

        // Chaque lecteur reçoit par défaut la première vidéo (souvent un fond ou une vidéo maître)
        if (nbVideos[i] > 0) {
            videosParLecteur[i][0] = listeFichiersEntree[0];
            nbVideosAffectees[i] = 1;
        }

        placesDisponibles += (nbVideos[i] - nbVideosAffectees[i]);
    }

    // Algorithme de répartition : on distribue les vidéos restantes une par une aux lecteurs
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
            lecteurActuel = 0; // Retour au premier lecteur (Round-Robin)
        }
    }

    // Lancement effectif de la génération FFmpeg pour chaque lecteur
    for (size_t i = 0; i < nbLecteursTotal; i++) {
        if (nbVideosAffectees[i] > 0) {
            string nomFichierSortie = "videosComplexes/VideoComplexe_" + to_string(idLecteurs[i]) + ".mp4";
            instanceVideoComplexe.genererVideoComplexe(videosParLecteur[i], nbVideosAffectees[i], nomFichierSortie);
        }
    }

    // Nettoyage de la mémoire temporaire de répartition
    for (size_t i = 0; i < nbLecteursTotal; ++i) {
        delete[] videosParLecteur[i];
    }
    delete[] videosParLecteur;
    delete[] nbVideosAffectees;
}

void M_SessionLecture::uploaderVideoComplexe() const {
    const string dossierSource = "videosComplexes";

    // 1. Détermination des fichiers à transférer
    struct TransferInfo {
        string ip;
        string path;
    };
    vector<TransferInfo> listeTransferts;

    // Note : On commence à i=1 pour ne pas s'envoyer la vidéo à soi-même
    for (size_t i = 1; i < nbLecteursTotal; i++) {
        if (nbVideos[i] > 0) {
            TransferInfo info;
            info.ip = ipLecteurs[i];
            info.path = "VideoComplexe_" + to_string(idLecteurs[i]) + ".mp4";
            listeTransferts.push_back(info);
        }
    }

    if (listeTransferts.empty()) {
        cout << "[SESSION] Aucun fichier à uploader." << endl;
        return;
    }

    // 2. Génération d'un fichier de log JSON en utilisant M_JsonUtil
    ofstream fichierJson("listeLecteurs.json");
    if (fichierJson.is_open()) {
        fichierJson << "[\n";
        for (size_t i = 0; i < listeTransferts.size(); i++) {
            // Création de la map pour l'objet plat (Exigence RFC 8259 via JsonUtil)
            map<string, string> champs;
            champs["ip"] = listeTransferts[i].ip;
            champs["path"] = listeTransferts[i].path;

            // M_JsonUtil::construire génère la chaîne '{"ip":"...","path":"..."}'
            fichierJson << "  " << M_JsonUtil::construire(champs);

            // Gestion de la virgule entre les éléments du tableau
            if (i < listeTransferts.size() - 1) {
                fichierJson << ",\n";
            } else {
                fichierJson << "\n";
            }
        }
        fichierJson << "]\n";
        fichierJson.close();
    }

    // 3. Exécution des transferts via le serveur TFTP
    cout << "\n[SESSION] Initialisation du client TFTP (M_TFTP_W)..." << endl;

    try {
        M_TFTP_W tftp;
        for (const auto& transfer : listeTransferts) {
            string fullPath = dossierSource + "/" + transfer.path;
            tftp.envoyer(transfer.ip, fullPath);
        }
        cout << "[SESSION] Fin de la session d'upload." << endl;
    } catch (const exception &e) {
        cerr << "[ERREUR CRITIQUE] Echec durant l'upload TFTP : " << e.what() << endl;
    }
}

vector<map<string, string> > M_SessionLecture::rechercherLecteurs(
    const string &ipMulticast, int portDecouverte, int portReponse) {

    // 2. Initialisation du moteur de découverte avec cette configuration
    M_DecouverteReseau moteurDecouverte(ipMulticast, portDecouverte, portReponse);

    // 3. Lancement de la recherche (Envoi du paquet binaire, attente 2s)
    moteurDecouverte.lancerDecouverte(2000);

    vector<map<string, string> > lecteursDetectes;
    const vector<string> &reponsesBrutes = moteurDecouverte.getReponsesBrutes();

    for (const string &json: reponsesBrutes) {
        try {
            auto infos = M_JsonUtil::parser(json);
            if (!infos.empty()) {
                lecteursDetectes.push_back(infos);
                cout << "[SESSION] Lecteur détecté : " << infos.at("ip") << ")" << endl;
            }
        } catch (const std::exception &e) {
            std::cerr << "[M_SessionLecture] Erreur parsing JSON : " << e.what() << std::endl;
        }
    }
    return lecteursDetectes;
}