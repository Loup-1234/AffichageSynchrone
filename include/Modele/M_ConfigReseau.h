#pragma once

#include <string>

/**
 * @class M_ConfigReseau
 * @brief Représente une configuration réseau du système SG01.2.
 * * Contient les paramètres nécessaires à la découverte multicast et à l'écoute des réponses.
 * * Conçue pour être instanciée dynamiquement via les paramètres du main() ou relue depuis la BDD (UC5).
 */
class M_ConfigReseau {
public:
    /**
     * @brief Constructeur par défaut (Valeurs de secours).
     * Utilise les valeurs : "ConfigDefaut", IP "239.0.0.1", Port Multicast 5000, Port Réponse 5001.
     * @note À n'utiliser que si l'injection depuis le main() échoue ou pour les tests.
     */
    M_ConfigReseau();

    /**
     * @brief Constructeur complet (Recommandé pour l'injection de dépendances).
     * @param nom Nom identifiant cette configuration (ex: "SessionActuelle").
     * @param adresseMulticast Adresse IP de multidiffusion cible (ex: "239.0.0.1").
     * @param portMulticast Port d'envoi pour la requête de découverte (ex: 5000).
     * @param portReponse Port d'écoute pour les réponses des lecteurs distants (ex: 5001).
     */
    M_ConfigReseau(const std::string& nom,
                   const std::string& adresseMulticast,
                   int portMulticast,
                   int portReponse);

    // ==========================================
    // --- GETTERS ---
    // ==========================================

    /** @return Le nom de la configuration. */
    const std::string& getNom() const;
    /** @return L'adresse IP Multicast configurée. */
    const std::string& getAdresseMulticast() const;
    /** @return Le port utilisé pour envoyer la découverte. */
    int getPortMulticast() const;
    /** @return Le port utilisé pour écouter les réponses JSON. */
    int getPortReponse() const;

    // ==========================================
    // --- SETTERS ---
    // ==========================================

    /** @param nom Le nouveau nom de configuration. */
    void setNom(const std::string& nom);
    /** @param adresseMulticast La nouvelle adresse IP Multicast. */
    void setAdresseMulticast(const std::string& adresseMulticast);
    /** @param portMulticast Le nouveau port d'envoi. */
    void setPortMulticast(int portMulticast);
    /** @param portReponse Le nouveau port de réception. */
    void setPortReponse(int portReponse);

private:
    std::string m_nom;              ///< Nom identifiant la configuration.
    std::string m_adresseMulticast; ///< Adresse IP Multicast (ex: 239.0.0.1).
    int         m_portMulticast;    ///< Port de destination pour le Multicast.
    int         m_portReponse;      ///< Port d'écoute local pour l'Unicast entrant.
};