#pragma once

#include <string>
#include <vector>
#include "Controleur/C_LecteurPhysiqueLocal.h"

using namespace std;

/**
 * @class V_Master
 * @brief Composant de l'interface graphique (IHM) pilotant la grappe d'écrans.
 */
class V_Master {
public:
    /**
     * @brief Constructeur de V_Master.
     * @param ipMulticast Adresse IP Multicast pour la synchronisation.
     * @param portCommandes Port UDP pour les ordres de lecture.
     * @param portDecouverte Port d'écoute pour la découverte des nœuds.
     * @param portReponse Port de retour pour l'identification.
     * @param dossierSourceVideos Répertoire contenant les fichiers multimédias.
     * @param cheminVideoMaster Chemin de destination pour la composition locale.
     */
    V_Master(const string &ipMulticast, int portCommandes, int portDecouverte, int portReponse,
             const string &dossierSourceVideos, const string &cheminVideoMaster);

    /**
     * @brief Lance la boucle principale de rendu de l'interface utilisateur.
     */
    void executer();

    /**
     * @brief Récupère la liste des adresses IP sélectionnées par l'utilisateur.
     * @return Vecteur contenant les adresses IP actives.
     */
    vector<string> getLecteursSelectionnes() const;

    /**
     * @brief Récupère les chemins des vidéos cochées dans la liste.
     * @return Vecteur des chemins complets vers les vidéos.
     */
    vector<string> getVideosSelectionnees() const;

private:
    C_LecteurPhysiqueLocal controleur; ///< Instance du contrôleur associé.
    string m_dossierVideos;            ///< Répertoire source des fichiers.
    void chargerListeVideos();         ///< Charge la liste depuis le répertoire.
    void miseAJourDisposition();       ///< Actualise la grille de l'IHM.
};