#include "Modele/M_DecouverteReseau.h"
#include <iostream>
#include <chrono>

M_DecouverteReseau::M_DecouverteReseau(const M_ConfigReseau &config)
    : m_config(config),
      m_expediteur(config.getAdresseMulticast(), config.getPortMulticast()),
      m_receveur(config.getPortReponse(), config.getAdresseMulticast()) {
}

M_DecouverteReseau::~M_DecouverteReseau() = default;

void M_DecouverteReseau::lancerDecouverte(int timeoutMs) {
    m_reponses.clear();

    // 1. Envoi du ping Multicast (Port 5000)
    m_expediteur.transmettreCommande(Expediteur::MASTER, TypeCommande::DECOUVERTE, Action::RECHERCHE, 0.0f);

    std::cout << "[M_DecouverteReseau] Ping Multicast envoyé. Attente de la réponse TFTP..." << std::endl;

    // 2. On bascule sur l'attente TFTP
    attendreReponses(timeoutMs);
}

void M_DecouverteReseau::attendreReponses(int timeoutMs) {
    // On instancie un récepteur TFTP pour le Master
    M_TFTP_W serveurTFTPMaster;
    std::string nomFichierRecu = "config_lecteur_recue.json";

    std::cout << "[M_DecouverteReseau] Serveur TFTP du Master ouvert (Port: " << m_config.getPortReponse() << ")..." << std::endl;

    // On utilise ta méthode TFTP pour attendre le fichier poussé par le lecteur
    // Attention : le lecteur doit envoyer sur le port m_config.getPortReponse() !
    if (serveurTFTPMaster.recevoirFichierPousse(m_config.getPortReponse(), nomFichierRecu)) {

        std::cout << "[M_DecouverteReseau] Fichier JSON reçu par TFTP avec succès !" << std::endl;

        // On lit le fichier qui vient d'être téléchargé sur le disque dur
        std::ifstream fichier(nomFichierRecu);
        if (fichier) {
            std::stringstream buffer;
            buffer << fichier.rdbuf();

            // On ajoute le contenu du JSON dans notre liste de réponses
            m_reponses.push_back(buffer.str());
            std::cout << "[M_DecouverteReseau] Configuration JSON extraite et sauvegardée en mémoire." << std::endl;
        } else {
            std::cerr << "[Erreur] Impossible de lire le fichier JSON reçu sur le disque." << std::endl;
        }

    } else {
        std::cout << "[M_DecouverteReseau] Timeout ou erreur lors de la réception TFTP." << std::endl;
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