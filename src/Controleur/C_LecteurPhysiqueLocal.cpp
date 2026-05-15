#include "Controleur/C_LecteurPhysiqueLocal.h"

#include <filesystem>
#include <iostream>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
using SocketType = SOCKET;
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/select.h>
using SocketType = int;
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#endif

using namespace std;

C_LecteurPhysiqueLocal::C_LecteurPhysiqueLocal(const string &ipMulticast, int portCommandes, int portDecouverte,
                                               int portReponse, const vector<vector<string> > &configLecteurs)
    : udp(ipMulticast, portCommandes), m_adresseMulticast(ipMulticast), m_portDecouverte(portDecouverte),
      m_portReponse(portReponse), configLecteurs(configLecteurs) {
    modeleLecteur.collecterInfosLocales();
    demarrerEcouteMulticast();

    if (filesystem::exists(CHEMIN_VIDEO)) {
        modeleLecteur.lireVideo(CHEMIN_VIDEO);
    }
}

C_LecteurPhysiqueLocal::~C_LecteurPhysiqueLocal() {
    arreterEcouteMulticast(); // Coupe le thread réseau proprement

    if (threadGeneration.joinable()) threadGeneration.join();
    if (threadRecherche.joinable()) threadRecherche.join();
    if (threadEcouteMulticast.joinable()) threadEcouteMulticast.join();
}

// Fonction d'origine restaurée
void C_LecteurPhysiqueLocal::arreterEcouteMulticast() {
    ecouteMulticastActive = false;
}

void C_LecteurPhysiqueLocal::demarrerEcouteMulticast() {
    ecouteMulticastActive = true;

    threadEcouteMulticast = thread([this]() {
        SocketType socketEcoute = socket(AF_INET, SOCK_DGRAM, 0);
        if (socketEcoute == INVALID_SOCKET) return;

        int reuse = 1;
        setsockopt(socketEcoute, SOL_SOCKET, SO_REUSEADDR, (const char *) &reuse, sizeof(reuse));

        sockaddr_in addrBind{};
        addrBind.sin_family = AF_INET;
        // CORRECTION : Utilisation de la variable membre m_portMulticast
        addrBind.sin_port = htons(m_portDecouverte);
        addrBind.sin_addr.s_addr = INADDR_ANY;

        if (bind(socketEcoute, reinterpret_cast<sockaddr *>(&addrBind), sizeof(addrBind)) == SOCKET_ERROR) {
#ifdef _WIN32
            closesocket(socketEcoute);
#else
            close(socketEcoute);
#endif
            return;
        }

        ip_mreq mreq{};
        // CORRECTION : Utilisation de la variable membre m_adresseMulticast (convertie en C-string)
        inet_pton(AF_INET, m_adresseMulticast.c_str(), &mreq.imr_multiaddr);
        mreq.imr_interface.s_addr = INADDR_ANY;
        setsockopt(socketEcoute, IPPROTO_IP, IP_ADD_MEMBERSHIP, (const char *) &mreq, sizeof(mreq));

        char buffer[512];
        sockaddr_in addrEmetteur{};
        socklen_t tailleAddr = sizeof(addrEmetteur);

        while (ecouteMulticastActive) {
            fd_set ensemble;
            FD_ZERO(&ensemble);
            FD_SET(socketEcoute, &ensemble);

            timeval tv{0, 500000};

            int pret = select(static_cast<int>(socketEcoute) + 1, &ensemble, nullptr, nullptr, &tv);
            if (pret <= 0) continue;

            int nbOctets = recvfrom(socketEcoute, buffer, sizeof(buffer) - 1, 0,
                                    reinterpret_cast<sockaddr *>(&addrEmetteur), &tailleAddr);

            if (nbOctets > 0) {
                string jsonInfos = modeleLecteur.versJson();

                sockaddr_in addrReponse{};
                addrReponse.sin_family = AF_INET;
                // CORRECTION : Utilisation de la variable membre m_portReponse
                addrReponse.sin_port = htons(m_portReponse);
                addrReponse.sin_addr = addrEmetteur.sin_addr;

                SocketType socketReponse = socket(AF_INET, SOCK_DGRAM, 0);
                if (socketReponse != INVALID_SOCKET) {
                    sendto(socketReponse, jsonInfos.c_str(), static_cast<int>(jsonInfos.size()), 0,
                           reinterpret_cast<sockaddr *>(&addrReponse), sizeof(addrReponse));
#ifdef _WIN32
                    closesocket(socketReponse);
#else
                    close(socketReponse);
#endif
                }
            }
        }

        setsockopt(socketEcoute, IPPROTO_IP, IP_DROP_MEMBERSHIP, (const char *) &mreq, sizeof(mreq));
#ifdef _WIN32
        closesocket(socketEcoute);
#else
        close(socketEcoute);
#endif
    });
}

void C_LecteurPhysiqueLocal::lancerRechercheLecteurs() {
    if (rechercheEnCours) return;

    rechercheEnCours = true;
    resultatsRecherchePrets = false;

    if (threadRecherche.joinable()) threadRecherche.join();

    threadRecherche = thread([this]() {
        try {
            // CORRECTION : On passe bien les paramètres injectés à la session
            cacheLecteurs = session.rechercherLecteursComplets(m_adresseMulticast, m_portDecouverte, m_portReponse);
            resultatsRecherchePrets = true;
        } catch (const exception &e) {
            cerr << "Erreur Recherche Réseau: " << e.what() << '\n';
        }
        rechercheEnCours = false;
    });
}

vector<map<string, string> > C_LecteurPhysiqueLocal::getDerniersLecteursTrouves() {
    resultatsRecherchePrets = false;
    return cacheLecteurs;
}

void C_LecteurPhysiqueLocal::initialiserSession(const vector<string> &fichiers) {
    if (generationEnCours || fichiers.size() < 2) return;

    modeleLecteur.pause();
    generationEnCours = true;

    if (threadGeneration.joinable()) threadGeneration.join();

    threadGeneration = thread([this, fichiers]() {
        try {
            if (!filesystem::exists("videosComplexes")) filesystem::create_directory("videosComplexes");

            size_t nbLecteurs = configLecteurs.size();
            LecteurSpec *specs = new LecteurSpec[nbLecteurs];

            for (size_t i = 0; i < nbLecteurs; ++i) {
                if (configLecteurs[i].size() >= 3) {
                    specs[i].id = configLecteurs[i][0];
                    specs[i].ip = configLecteurs[i][1];
                    specs[i].nbVideos = configLecteurs[i][2];
                }
            }

            session.preparerSessionLecture(specs, nbLecteurs);
            delete[] specs;

            session.genererVideoComplexe(fichiers.data(), fichiers.size());

            transfertEnCours = true;
            session.uploaderVideoComplexe();
            transfertEnCours = false;

            videoGeneree = true;
        } catch (const exception &e) {
            cerr << "Erreur Génération: " << e.what() << '\n';
        }
        generationEnCours = false;
    });
}

void C_LecteurPhysiqueLocal::basculerPlayPause() {
    const bool enLecture = modeleLecteur.estEnLecture();
    if (enLecture) {
        modeleLecteur.pause();
        udp.transmettreCommande(Expediteur::MASTER, TypeCommande::ORDRE, Action::PAUSE, 0.0f);
    } else {
        if (modeleLecteur.estTermine()) {
            modeleLecteur.stop();
            modeleLecteur.demarrer();
            udp.transmettreCommande(Expediteur::MASTER, TypeCommande::ORDRE, Action::PROGRESSION, 0.0f);
        } else {
            modeleLecteur.play();
            udp.transmettreCommande(Expediteur::MASTER, TypeCommande::ORDRE, Action::PLAY, 0.0f);
        }
    }
}

void C_LecteurPhysiqueLocal::modifierVolume(const float volume, const bool muet) {
    volumeCourant = muet ? 0.0f : volume;
    modeleLecteur.setVolume(static_cast<int>(volumeCourant));
    udp.transmettreCommande(Expediteur::MASTER, TypeCommande::ORDRE, Action::VOLUME, volumeCourant);
}

void C_LecteurPhysiqueLocal::modifierProgression(const float progression, const bool enGlissement,
                                                 const bool restaurerLecture) {
    modeleLecteur.setTime(progression);
    udp.transmettreCommande(Expediteur::MASTER, TypeCommande::ORDRE, Action::PROGRESSION, progression);

    if (enGlissement) {
        modeleLecteur.pause();
        modeleLecteur.setVolume(0);
    } else {
        modeleLecteur.setVolume(static_cast<int>(volumeCourant));
        if (restaurerLecture) modeleLecteur.play();
        else modeleLecteur.pause();
    }
}

void C_LecteurPhysiqueLocal::modifierVitesse(const float vitesse) {
    modeleLecteur.setVitesse(vitesse);
    udp.transmettreCommande(Expediteur::MASTER, TypeCommande::ORDRE, Action::VITESSE, vitesse);
}

void C_LecteurPhysiqueLocal::mettreAJour() {
    if (videoGeneree) {
        modeleLecteur.lireVideo(CHEMIN_VIDEO);
        videoGeneree = false;
    }
}

void C_LecteurPhysiqueLocal::stopper() {
    modeleLecteur.stop();
    udp.transmettreCommande(Expediteur::MASTER, TypeCommande::ORDRE, Action::STOP, 0.0f);
}