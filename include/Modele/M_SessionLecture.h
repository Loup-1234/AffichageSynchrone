#pragma once

#include "M_VideoComplexe.h"
#include "M_ProtocoleReseau.h"
#include "M_BDD.h"
#include "M_ConfigReseau.h"
#include <string>
#include <vector>
#include <map>

using namespace std;

/**
 * @class M_SessionLecture
 * @brief Gestionnaire de session supervisant le cycle de distribution, de generation et de transfert des videos.
 * * Cette classe fait le lien entre l'interface utilisateur (choix des ecrans et des medias),
 * la base de donnees (resolutions materielles) et les moteurs d'execution (FFmpeg et TFTP).
 */
class M_SessionLecture {
public:
    /**
     * @brief Constructeur de la classe M_SessionLecture.
     * @param ip Adresse IP utilisee pour l'initialisation de la configuration reseau.
     * @param port Port reseau associe aux communications de la session.
     */
    M_SessionLecture(const string &ip, int port);

    /**
     * @brief Definit manuellement la liste des configurations de lecteurs.
     * @param configs Vecteur contenant les structures de configuration cibles.
     */
    void configurerLecteurs(const vector<LecteurConfig>& configs);

    /**
     * @brief Orchestre la generation des mosaiques videos adaptees a chaque lecteur actif.
     * * Cette methode reconstruit la liste des lecteurs, interroge la base de donnees pour connaitre
     * leurs dimensions d'ecran, repartit les fichiers sources et lance le rendu FFmpeg.
     * * @param listeFichiersEntree Liste des chemins des videos selectionnees par l'utilisateur.
     * @param dossierSortie Repertoire de destination pour l'enregistrement des fichiers MP4 generes.
     * @param ipsSelectionnees Liste des adresses IP cochees dans l'interface graphique.
     */
    void genererVideoComplexe(const vector<string>& listeFichiersEntree, const string& dossierSortie, const vector<string>& ipsSelectionnees);

    /**
     * @brief Transmet les fichiers videos generes aux lecteurs distants via le protocole TFTP.
     * @param dossierSource Repertoire contenant les fichiers "VideoComplexe_X.mp4" a envoyer.
     */
    void uploaderVideoComplexe(const string& dossierSource) const;

    /**
     * @brief Effectue une recherche reseau pour detecter et lister les lecteurs physiques connectes.
     * @return Un vecteur de dictionnaires contenant les attributs de chaque lecteur detecte (IP, MAC, etc.).
     */
    vector<map<string, string>> rechercherLecteurs();

private:
    /**
     * @brief Calcule le quota de videos attribue a chaque ecran proportionnellement a sa surface en pixels.
     * * Si un seul lecteur est present (le Master local), il se voit attribuer la totalite du catalogue.
     * En cas d'incoherence ou d'absence de donnees en BDD, une repartition de secours est appliquee.
     * * @param nombreTotalVideos Nombre total de flux videos a distribuer au sein de la session.
     */
    void calculerCapacitesVideo(int nombreTotalVideos);

    M_VideoComplexe instanceVideoComplexe; ///< Instance du moteur de composition video FFmpeg.
    vector<LecteurConfig> m_lecteurs;      ///< Tableau dynamique des lecteurs selectionnes pour la session courante.
    M_BDD bdd;                             ///< Instance d'acces et de requetage a la base de donnees SQLite.
    M_ConfigReseau config;                 ///< Gestionnaire des topologies et de la decouverte reseau.
};