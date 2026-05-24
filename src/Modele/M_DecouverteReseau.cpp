#include "Modele/M_DecouverteReseau.h"
#ifdef _WIN32
#include "Modele/M_TFTP_W.h"
#endif // _WIN32
#include <iostream>
#include <chrono>
#include <filesystem>

M_DecouverteReseau::M_DecouverteReseau(const std::string& ipMulticast, int portMulticast, int portReponse)
    : m_expediteur(ipMulticast, portMulticast),
      m_receveur(portReponse, ipMulticast),
      m_portReponse(portReponse) {
}

M_DecouverteReseau::~M_DecouverteReseau() = default;

void M_DecouverteReseau::lancerDecouverte(int timeoutMs) {
    m_reponses.clear();
    m_expediteur.transmettreCommande(Expediteur::MASTER, TypeCommande::CONNECTION, Action::PLAY, 0.0f);
    std::cout << "[M_DecouverteReseau] Paquet de découverte envoyé." << std::endl;
    attendreReponses(timeoutMs);
}

void M_DecouverteReseau::attendreReponses(int timeoutMs) {
    char buffer[4096];
    std::string ipSrc;

    std::cout << "[M_DecouverteReseau] Ecoute des réponses..." << std::endl;

    auto heureDebut = std::chrono::steady_clock::now();

    while (true) {
        auto maintenant = std::chrono::steady_clock::now();
        const int tempsEcouleMs = std::chrono::duration_cast<std::chrono::milliseconds>(maintenant - heureDebut).count();
        const int tempsRestantMs = timeoutMs - tempsEcouleMs;

        if (tempsRestantMs <= 0) {
            break;
        }

        int nbOctets = m_receveur.recevoirAvecTimeout(buffer, sizeof(buffer) - 1, ipSrc, tempsRestantMs);

        if (nbOctets > 0) {
            buffer[nbOctets] = '\0';
            std::string nomFichierJson = std::string(buffer);
            std::string cheminFichierJson = "cmake-build-debug/" + nomFichierJson;
            #ifdef _WIN32
            M_TFTP_W tftp;
            if (tftp.recevoirFichierPousse(m_portReponse, cheminFichierJson)) {
                m_reponses.push_back(cheminFichierJson);
                std::cout << "[M_DecouverteReseau] Réponse JSON reçue de " << ipSrc << " et sauvegardée dans " << cheminFichierJson << std::endl;
            } else {
                std::cerr << "[M_DecouverteReseau] Erreur de réception TFTP du fichier " << nomFichierJson << " de " << ipSrc << std::endl;
            }
            #endif // _WIN32
        } else {
            break;
        }
    }
}

const std::vector<std::string> &M_DecouverteReseau::getReponsesBrutes() const {
    return m_reponses;
}

void M_DecouverteReseau::afficherResultats() const {
    std::cout << "--- " << m_reponses.size() << " réponses collectées ---" << std::endl;
    for (const auto &r: m_reponses) {
        std::cout << " > " << r << std::endl;
    }
}