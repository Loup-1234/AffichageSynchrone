#pragma once

#include "Modele/M_ConfigReseau.h"
#include "Modele/M_ExpediteurUDP.h"
#include "Modele/M_ReceveurUDP.h"
#include "Modele/M_TFTP_W.h"
#include <vector>
#include <string>

/**
 * @class M_DecouverteReseau
 * @brief Gère la recherche et la découverte des lecteurs sur le réseau local.
 *
 * Cette classe orchestre l'envoi d'une requête de découverte en Multicast/Broadcast
 * et écoute les réponses renvoyées par les autres instances du programme pendant
 * un temps donné.
 */
class M_DecouverteReseau {
public:
    /**
     * @brief Constructeur de la classe M_DecouverteReseau.
     * @param config Objet contenant les paramètres réseau (IP Multicast, ports d'écoute et d'envoi).
     */
    explicit M_DecouverteReseau(const M_ConfigReseau& config);

    /**
     * @brief Destructeur de la classe.
     * Nettoie les ressources réseau allouées.
     */
    ~M_DecouverteReseau();

    /**
     * @brief Lance une session de découverte sur le réseau.
     *
     * Envoie une commande UDP aux autres machines pour signaler sa présence
     * et attend leurs réponses en bloquant l'exécution pendant la durée du timeout.
     *
     * @param timeoutMs Temps d'attente maximum pour collecter toutes les réponses, en millisecondes.
     */
    void lancerDecouverte(int timeoutMs = 2000);

    /**
     * @brief Récupère la liste des réponses obtenues lors de la dernière découverte.
     * @return Une référence constante vers un vecteur de chaînes de caractères (généralement du format JSON).
     */
    const std::vector<std::string>& getReponsesBrutes() const;

    /**
     * @brief Affiche le contenu brut des réponses collectées dans la console standard.
     * Utile principalement pour le débogage.
     */
    void afficherResultats() const;

private:
    /**
     * @brief Boucle d'écoute interne pour réceptionner les paquets réseau entrants.
     * @param timeoutMs Le temps restant imparti pour l'écoute, en millisecondes.
     */
    void attendreReponses(int timeoutMs);

    M_ConfigReseau m_config;               ///< Configuration réseau utilisée pour les ports et adresses.
    M_ExpediteurUDP m_expediteur;          ///< Composant responsable de l'envoi de la requête de découverte.
    M_ReceveurUDP m_receveur;              ///< Composant responsable de la réception des réponses (avec timeout).
    std::vector<std::string> m_reponses;   ///< Liste stockant les réponses brutes reçues des lecteurs distants.
};