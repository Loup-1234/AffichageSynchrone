#include "Modele/M_DecouverteReseau.h"
#include <iostream>
#include <chrono>

M_DecouverteReseau::M_DecouverteReseau(const std::string& ipMulticast, int portMulticast, int portReponse)
    : m_expediteur(ipMulticast, portMulticast),
      m_receveur(portReponse, ipMulticast) {
}

M_DecouverteReseau::~M_DecouverteReseau() = default;

void M_DecouverteReseau::lancerDecouverte(int timeoutMs) {
    m_reponses.clear();

    // Envoi de la requête de découverte sur le réseau
    m_expediteur.transmettreCommande(Expediteur::MASTER, TypeCommande::DECOUVERTE, Action::RECHERCHE, 0.0f);

    std::cout << "[M_DecouverteReseau] Paquet de découverte envoyé." << std::endl;

    attendreReponses(timeoutMs);
}

void M_DecouverteReseau::attendreReponses(int timeoutMs) {
    char buffer[4096];
    std::string ipSrc;

    std::cout << "[M_DecouverteReseau] Ecoute des réponses..." << std::endl;

    // On mémorise l'heure de début pour ne pas dépasser le temps alloué
    auto heureDebut = std::chrono::steady_clock::now();

    while (true) {
        // Calcul du temps restant
        auto maintenant = std::chrono::steady_clock::now();
        int tempsEcouleMs = std::chrono::duration_cast<std::chrono::milliseconds>(maintenant - heureDebut).count();
        int tempsRestantMs = timeoutMs - tempsEcouleMs;

        // Si on a dépassé le délai global de recherche, on arrête d'écouter
        if (tempsRestantMs <= 0) {
            break;
        }

        // On écoute le réseau uniquement pour le temps qu'il nous reste
        int nbOctets = m_receveur.recevoirAvecTimeout(buffer, sizeof(buffer) - 1, ipSrc, tempsRestantMs);

        if (nbOctets > 0) {
            buffer[nbOctets] = '\0';
            m_reponses.push_back(std::string(buffer));
            std::cout << "[M_DecouverteReseau] Réponse JSON reçue de " << ipSrc << std::endl;
        } else if (nbOctets == 0) {
            // nbOctets = 0 signifie que le timeout a été atteint sans recevoir de paquet
            break;
        } else {
            // Une erreur réseau s'est produite
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