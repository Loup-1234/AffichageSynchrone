#include "../../include/Modele/M_ConfigReseau.h"

// Constructeur de secours (Fallback)
M_ConfigReseau::M_ConfigReseau()
    : m_nom("ConfigDefaut"),
      m_adresseMulticast("239.0.0.1"),
      m_portMulticast(5000),
      m_portReponse(5001) {
}

// Constructeur principal pour l'injection depuis le main()
M_ConfigReseau::M_ConfigReseau(const std::string &nom,
                               const std::string &adresseMulticast,
                               int portMulticast,
                               int portReponse)
    : m_nom(nom),
      m_adresseMulticast(adresseMulticast),
      m_portMulticast(portMulticast),
      m_portReponse(portReponse) {
}

// --- Getters ---

const std::string &M_ConfigReseau::getNom() const {
    return m_nom;
}

const std::string &M_ConfigReseau::getAdresseMulticast() const {
    return m_adresseMulticast;
}

int M_ConfigReseau::getPortMulticast() const {
    return m_portMulticast;
}

int M_ConfigReseau::getPortReponse() const {
    return m_portReponse;
}

// --- Setters ---

void M_ConfigReseau::setNom(const std::string &nom) {
    m_nom = nom;
}

void M_ConfigReseau::setAdresseMulticast(const std::string &adresseMulticast) {
    m_adresseMulticast = adresseMulticast;
}

void M_ConfigReseau::setPortMulticast(int portMulticast) {
    m_portMulticast = portMulticast;
}

void M_ConfigReseau::setPortReponse(int portReponse) {
    m_portReponse = portReponse;
}