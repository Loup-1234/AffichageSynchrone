#pragma once

#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include "Modele/M_SessionLecture.h"

/**
 * @class C_LecteurPhysiqueLocal
 * @brief Contrôleur orchestrant les interactions entre la vue (IHM) et le modèle (Logique).
 * * Ce contrôleur gère l'exécution asynchrone des tâches de rendu pour éviter
 * le gel de l'interface graphique.
 */
class C_LecteurPhysiqueLocal {
public:
    /**
     * @brief Constructeur du contrôleur.
     * @param ipMulticast IP réseau pour la synchronisation.
     * @param portCmd Port de contrôle.
     * @param portDec Port de découverte.
     * @param portRep Port de réponse.
     * @param chemin Chemin de destination du Master.
     */
    C_LecteurPhysiqueLocal(const string &ipMulticast, int portCmd, int portDec, int portRep, const string &chemin);
    ~C_LecteurPhysiqueLocal();

    /**
     * @brief Initialise et lance la génération des vidéos dans un thread séparé.
     * @param fichiers Liste des vidéos.
     * @param ips Liste des cibles réseau.
     */
    void initialiserSession(const vector<string> &fichiers, const vector<string> &ips);

    /** @brief Arrête proprement le processus de génération en cours. */
    void arreterSession();

    /** @brief Retourne l'état de la génération. */
    bool estEnCoursDeGeneration() const { return m_generationEnCours; }

private:
    M_SessionLecture m_session;           ///< Instance du modèle métier.
    thread m_threadWorker;                ///< Thread d'exécution des tâches de fond.
    atomic<bool> m_generationEnCours;     ///< Indicateur atomique de l'état du moteur.
    mutex m_mutexSession;                 ///< Protection des ressources partagées.

    string m_ipMulticast;                 ///< Paramètre réseau de diffusion.
    int m_portCmd, m_portDec, m_portRep;  ///< Ports de communication réseau.
    string m_cheminMaster;                ///< Chemin cible du Master local.
};