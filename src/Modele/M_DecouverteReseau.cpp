#include "../../include/Modele/M_DecouverteReseau.h"
#include <cstring>
#include <iostream>
#include <stdexcept>

M_DecouverteReseau::M_DecouverteReseau(const M_ConfigReseau &config)
    : m_config(config),
      m_expediteur(config.getAdresseMulticast(), config.getPortMulticast()),
      m_socketReception(INVALID_SOCKET) {
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        throw std::runtime_error("[M_DecouverteReseau] Echec WSAStartup");
    }
#endif

    m_socketReception = socket(AF_INET, SOCK_DGRAM, 0);
    if (m_socketReception == INVALID_SOCKET) {
        throw std::runtime_error("[M_DecouverteReseau] Erreur création socket réception");
    }

    int reuse = 1;
    setsockopt(m_socketReception, SOL_SOCKET, SO_REUSEADDR, (const char *) &reuse, sizeof(reuse));

    sockaddr_in addrReception{};
    addrReception.sin_family = AF_INET;
    addrReception.sin_port = htons(m_config.getPortReponse());
    addrReception.sin_addr.s_addr = INADDR_ANY;

    if (bind(m_socketReception, reinterpret_cast<sockaddr *>(&addrReception), sizeof(addrReception)) == SOCKET_ERROR) {
#ifdef _WIN32
        closesocket(m_socketReception);
#else
        close(m_socketReception);
#endif
        throw std::runtime_error("[M_DecouverteReseau] Impossible de lier le socket de réception");
    }
}

M_DecouverteReseau::~M_DecouverteReseau() {
    if (m_socketReception != INVALID_SOCKET) {
#ifdef _WIN32
        closesocket(m_socketReception);
        WSACleanup();
#else
        close(m_socketReception);
#endif
    }
}

void M_DecouverteReseau::lancerDecouverte(int timeoutMs) {
    m_reponses.clear();

    // Envoi de la requête de découverte avec la structure standardisée
    m_expediteur.transmettreCommande(Expediteur::MASTER, TypeCommande::DECOUVERTE, Action::RECHERCHE, 0.0f);

    std::cout << "[M_DecouverteReseau] Paquet de découverte envoyé sur "
            << m_config.getAdresseMulticast() << ":" << m_config.getPortMulticast() << std::endl;

    attendreReponses(timeoutMs);
}

void M_DecouverteReseau::attendreReponses(int timeoutMs) {
    char buffer[4096];
    sockaddr_in addrEmetteur{};
    socklen_t tailleAddr = sizeof(addrEmetteur);

    timeval tv{};
    tv.tv_sec = timeoutMs / 1000;
    tv.tv_usec = (timeoutMs % 1000) * 1000;

    std::cout << "[M_DecouverteReseau] Ecoute des réponses..." << std::endl;

    while (true) {
        fd_set ensemble;
        FD_ZERO(&ensemble);
        FD_SET(m_socketReception, &ensemble);

        timeval tvCopie = tv;
        int pret = select(m_socketReception + 1, &ensemble, nullptr, nullptr, &tvCopie);

        if (pret <= 0) {
            break; // Timeout ou erreur
        }

        if (FD_ISSET(m_socketReception, &ensemble)) {
            int nbOctets = recvfrom(m_socketReception, buffer, sizeof(buffer) - 1, 0,
                                    reinterpret_cast<sockaddr *>(&addrEmetteur), &tailleAddr);

            if (nbOctets > 0) {
                buffer[nbOctets] = '\0';
                m_reponses.push_back(std::string(buffer));

                char ipSrc[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &addrEmetteur.sin_addr, ipSrc, sizeof(ipSrc));
                std::cout << "[M_DecouverteReseau] Réponse JSON reçue de " << ipSrc << std::endl;
            }
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