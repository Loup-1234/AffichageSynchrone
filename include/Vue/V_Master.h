#pragma once

#include <string>
#include <vector>
#include <memory>
#include "Controleur/C_LecteurPhysiqueLocal.h"

/**
 * @class V_Master
 * @brief Classe principale gérant l'affichage graphique et les événements de saisie.
 */
class V_Master {
public:
    /**
     * @brief Constructeur de l'IHM.
     * @param ipMulticast Paramètres réseau pour le contrôleur.
     * @param pCmd Ports de communication.
     * @param pDec Ports de découverte.
     * @param pRep Ports de réponse.
     * @param dossier Dossier contenant les médias.
     * @param cheminMaster Chemin du Master local.
     */
    V_Master(const string &ipMulticast, int pCmd, int pDec, int pRep, const string &dossier, const string &cheminMaster);
    ~V_Master() = default;

    /** @brief Boucle principale de l'application (Main Loop). */
    void executer();

    /**
     * @brief Retourne les adresses IP sélectionnées dans l'interface.
     * @return Vecteur d'IP.
     */
    vector<string> getLecteursSelectionnes() const;

    /**
     * @brief Retourne les chemins des vidéos sélectionnées.
     * @return Vecteur de chemins fichiers.
     */
    vector<string> getVideosSelectionnees() const;

private:
    unique_ptr<C_LecteurPhysiqueLocal> m_controleur; ///< Contrôleur géré par pointeur intelligent.
    string m_dossierVideos;                          ///< Répertoire source.
    bool m_fenetreOuverte;                           ///< État de la fenêtre.

    void dessinerInterface();                        ///< Rendu graphique des éléments Raylib.
    void gererEntreesClavier();                      ///< Gestion des raccourcis et entrées utilisateur.
};