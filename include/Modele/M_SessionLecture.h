#pragma once

#include "M_VideoComplexe.h"
#include "M_ConfigReseau.h"
#include <string>
#include <vector>
#include <map>

using namespace std;

/**
 * @struct LecteurConfig
 * @brief Configuration propre, typée et reliée au cache pour un lecteur cible.
 */
struct LecteurConfig {
    int id;                      ///< Identifiant unique du lecteur (0 pour le Master).
    string mac;                  ///< Adresse MAC du lecteur.
    string ip;                   ///< Adresse IP du lecteur.
    int nbVideosCapacite;        ///< Nombre maximal de vidéos que l'écran peut accueillir.
};

/**
 * @class M_SessionLecture
 * @brief Gestionnaire de session supervisant le cycle de distribution, de génération et de transfert des vidéos.
 *
 * Cette classe fait le lien entre l'interface utilisateur (choix des écrans et des médias),
 * le cache de configuration réseau (résolutions matérielles) et les moteurs d'exécution (FFmpeg et TFTP).
 */
class M_SessionLecture {
public:
    /**
     * @brief Constructeur de la classe M_SessionLecture.
     * @param ip Adresse IP utilisée pour l'initialisation de la configuration réseau.
     * @param port Port réseau associé aux communications de la session.
     */
    M_SessionLecture(const string &ip, int port);

    /**
     * @brief Définit manuellement la liste des configurations de lecteurs.
     * @param configs Vecteur contenant les structures de configuration cibles.
     */
    void configurerLecteurs(const vector<LecteurConfig>& configs);

    /**
     * @brief Orchestre la génération des mosaïques vidéos adaptées à chaque lecteur actif.
     *
     * Cette méthode reconstruit la liste des lecteurs, utilise le cache pour connaître
     * leurs dimensions d'écran, répartit les fichiers sources et lance le rendu FFmpeg.
     * @param listeFichiersEntree Liste des chemins des vidéos sélectionnées par l'utilisateur.
     * @param dossierSortie Répertoire de destination pour l'enregistrement des fichiers MP4 générés.
     * @param ipsSelectionnees Liste des adresses IP cochées dans l'interface graphique.
     */
    void genererVideoComplexe(const vector<string>& listeFichiersEntree, const string& dossierSortie, const vector<string>& ipsSelectionnees);

    /**
     * @brief Transmet les fichiers vidéos générés aux lecteurs distants via le protocole TFTP.
     * @param dossierSource Répertoire contenant les fichiers "VideoComplexe_X.mp4" à envoyer.
     */
    void uploaderVideoComplexe(const string& dossierSource) const;

    /**
     * @brief Effectue une recherche réseau pour détecter, lister les lecteurs physiques et synchroniser le cache.
     * @return Un vecteur de dictionnaires contenant les attributs de chaque lecteur détecté (IP, MAC, etc.).
     */
    vector<map<string, string>> rechercherLecteurs();

    /**
     * @brief Récupère la configuration brute actuelle depuis le module réseau.
     * @return Tableau 2D de chaînes représentant la configuration réseau.
     */
    vector<vector<string>> getConfig() { return config.getConfigReseau(); }

    /**
     * @brief Demande la persistance de la configuration actuelle vers la base de données.
     */
    void sauvegarderConfig() { config.sauvegarderConfigActuelle(); }

    /**
     * @brief Charge la configuration depuis la base de données et met à jour le cache local.
     */
    void chargerConfig() {
        config.visualiserLecteurPhysique();
        m_cacheConfigReseau = config.getConfigReseau();
    }

private:
    /**
     * @brief Calcule le quota de vidéos attribué à chaque écran proportionnellement à sa surface en pixels depuis le cache.
     * @param nombreTotalVideos Nombre total de flux vidéos à distribuer au sein de la session.
     */
    void calculerCapacitesVideo(int nombreTotalVideos);

    M_VideoComplexe instanceVideoComplexe;       ///< Instance du moteur de composition vidéo FFmpeg.
    vector<LecteurConfig> m_lecteurs;            ///< Tableau dynamique des lecteurs sélectionnés pour la session courante.
    M_ConfigReseau config;                       ///< Gestionnaire des topologies et de la découverte réseau.
    vector<vector<string>> m_cacheConfigReseau;   ///< Variable cache mise à jour lors d'une recherche ou d'un chargement.
};