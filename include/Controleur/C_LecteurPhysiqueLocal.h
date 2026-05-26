#pragma once

#include <string>
#include <vector>
#include <thread>
#include "Modele/M_SessionLecture.h"

using namespace std;

/**
 * @class C_LecteurPhysiqueLocal
 * @brief Contrôleur d'interface orchestrant les échanges entre la vue graphique et le modèle de session.
 */
class C_LecteurPhysiqueLocal {
public:
    /**
     * @brief Constructeur de la classe C_LecteurPhysiqueLocal.
     * @param ipMulticast Adresse de diffusion IP Multicast pour les trames de synchronisation.
     * @param portCommandes Port de destination des paquets réseau d'ordres de lecture.
     * @param portDecouverte Port réseau d'écoute dédié au mécanisme de détection automatique des nœuds.
     * @param portReponse Port réseau d'émission des réponses de configuration matérielle.
     * @param cheminVideoMaster Chemin d'accès vers la vidéo finale synthétisée pour l'écran maître.
     */
    C_LecteurPhysiqueLocal(const string &ipMulticast, int portCommandes, int portDecouverte, int portReponse, const string &cheminVideoMaster);

    /**
     * @brief Initialise et exécute asynchroniquement au sein d'un thread dédié le processus complet de rendu et d'envoi.
     * @param fichiers Liste des chemins d'accès des vidéos sélectionnées dans l'IHM.
     * @param lecteursSelectionnes Liste des adresses IP des écrans clients cochés dans l'IHM.
     */
    void initialiserSession(const vector<string> &fichiers, const vector<string> &lecteursSelectionnes);
};