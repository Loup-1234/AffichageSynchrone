#pragma once

#include <string>
#include <vector>
#include "Controleur/C_LecteurPhysiqueLocal.h"

using namespace std;

/**
 * @class V_Master
 * @brief Composant de l'interface graphique utilisateur (IHM réalisée en Raylib) pilotant la grappe d'écrans.
 */
class V_Master {
public:
    /**
     * @brief Constructeur de la classe V_Master.
     * @param ipMulticast Adresse IP de groupe Multicast pour le pilotage synchrone.
     * @param portCommandes Port d'envoi UDP des structures d'ordres de contrôle.
     * @param portDecouverte Port réseau d'écoute associé au protocole de découverte.
     * @param portReponse Port réseau de collecte des retours d'identification.
     * @param dossierSourceVideos Répertoire local contenant les médias indexables dans la liste.
     * @param cheminVideoMaster Emplacement disque où enregistrer la composition vidéo du Master.
     */
    V_Master(const string &ipMulticast, int portCommandes, int portDecouverte, int portReponse, const string &dossierSourceVideos, const string &cheminVideoMaster);

    /**
     * @brief Démarre la fenêtre graphique et gère la boucle principale événementielle de rendu à 60 FPS.
     */
    void executer();

    /**
     * @brief Interroge l'état graphique des cases à cocher pour lister les terminaux validés.
     * @return Un vecteur de chaînes de caractères listant les adresses IP sélectionnées.
     */
    vector<string> getLecteursSelectionnes() const;

    /**
     * @brief Interroge l'état graphique des éléments de liste pour collecter les médias validés.
     * @return Un vecteur de chaînes de caractères listant les chemins des vidéos sélectionnées.
     */
    vector<string> getVideosSelectionnees() const;
};