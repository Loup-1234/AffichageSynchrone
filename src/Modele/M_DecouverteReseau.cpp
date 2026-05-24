#include "Modele/M_DecouverteReseau.h"
#ifdef _WIN32
#include "Modele/M_TFTP_W.h"
#endif // _WIN32
#include <iostream>
#include <chrono>
#include <filesystem>

M_DecouverteReseau::M_DecouverteReseau(const string& ipMulticast, int portMulticast, int portReponse)
    : m_expediteur(ipMulticast, portMulticast),
      m_receveur(portReponse, ipMulticast),
      m_portReponse(portReponse) {
}

M_DecouverteReseau::~M_DecouverteReseau() = default;

void M_DecouverteReseau::lancerDecouverte(int timeoutMs) {
    m_reponses.clear();
    m_expediteur.transmettreCommande(Expediteur::MASTER, TypeCommande::CONNECTION, Action::PLAY, 0.0f);
    cout << "[M_DecouverteReseau] Paquet de découverte envoyé." << endl;
    attendreReponses(timeoutMs);
}

void M_DecouverteReseau::attendreReponses(int timeoutMs) {
    char buffer[4096];
    string ipSrc;

    cout << "[M_DecouverteReseau] Ecoute des réponses..." << endl;

    auto heureDebut = chrono::steady_clock::now();

    while (true) {
        auto maintenant = chrono::steady_clock::now();
        const int tempsEcouleMs = chrono::duration_cast<chrono::milliseconds>(maintenant - heureDebut).count();
        const int tempsRestantMs = timeoutMs - tempsEcouleMs;

        if (tempsRestantMs <= 0) {
            break;
        }

        int nbOctets = m_receveur.recevoirAvecTimeout(buffer, sizeof(buffer) - 1, ipSrc, tempsRestantMs);

        if (nbOctets > 0) {
            buffer[nbOctets] = '\0';
            string nomFichierJson = string(buffer);
            string cheminFichierJson = "cmake-build-debug/" + nomFichierJson;
            #ifdef _WIN32
            M_TFTP_W tftp;
            if (tftp.recevoirFichierPousse(m_portReponse, cheminFichierJson)) {
                m_reponses.push_back(cheminFichierJson);
                cout << "[M_DecouverteReseau] Réponse JSON reçue de " << ipSrc << " et sauvegardée dans " << cheminFichierJson << endl;
            } else {
                cerr << "[M_DecouverteReseau] Erreur de réception TFTP du fichier " << nomFichierJson << " de " << ipSrc << endl;
            }
            #endif // _WIN32
        } else {
            break;
        }
    }
}

const vector<string> &M_DecouverteReseau::getReponsesBrutes() const {
    return m_reponses;
}

void M_DecouverteReseau::afficherResultats() const {
    cout << "--- " << m_reponses.size() << " réponses collectées ---" << endl;
    for (const auto &r: m_reponses) {
        cout << " > " << r << endl;
    }
}