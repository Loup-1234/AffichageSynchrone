#pragma once

#include "M_VideoComplexe.h"

#include <string>
#include <vector>
#include <map>

using namespace std;

/**
 * @struct LecteurSpec
 * @brief Structure de données brute décrivant un lecteur cible.
 */
struct LecteurSpec {
    string id;       ///< Identifiant numérique (sous forme de chaîne).
    string ip;       ///< Adresse IP réseau.
    string nbVideos; ///< Nombre de vidéos allouées à ce lecteur (sous forme de chaîne).
};

/**
 * @class M_SessionLecture
 * @brief Chef d'orchestre gérant la répartition, la génération et l'envoi des vidéos aux lecteurs.
 */
class M_SessionLecture {

    M_VideoComplexe instanceVideoComplexe; ///< Module de synthèse vidéo (FFmpeg).

    int* idLecteurs = nullptr;    ///< Tableau dynamique des identifiants (convertis).
    string* ipLecteurs = nullptr; ///< Tableau dynamique des adresses IP.
    int* nbVideos = nullptr;      ///< Tableau dynamique des capacités de stockage/lecture.
    size_t nbLecteursTotal = 0;   ///< Nombre total de lecteurs configurés.

public:
    /**
     * @brief Constructeur par défaut.
     */
    M_SessionLecture() = default;

    /**
     * @brief Destructeur gérant la libération des tableaux dynamiques.
     */
    ~M_SessionLecture();

    /**
     * @brief Alloue et configure les données de la session à partir des spécifications.
     * @param specLecteurs Pointeur vers le tableau de spécifications.
     * @param nbLecteurs Nombre d'éléments dans le tableau.
     */
    void preparerSessionLecture(const LecteurSpec* specLecteurs, size_t nbLecteurs);

    /**
     * @brief Répartit les fichiers d'entrée entre les lecteurs et lance la génération des fichiers MP4.
     * @param listeFichiersEntree Tableau des chemins de fichiers sources.
     * @param nbFichiers Nombre total de fichiers sources.
     */
    void genererVideoComplexe(const string* listeFichiersEntree, size_t nbFichiers);

    /**
     * @brief Prépare les tâches de transfert et lance le serveur TFTP pour l'envoi aux clients.
     */
    void uploaderVideoComplexe() const;

    /**
         * @brief [UC4/UC3] Recherche les lecteurs sur le réseau avec une configuration spécifique.
         * @param ipMulticast Adresse IP Multicast de découverte.
         * @param portDecouverte Port d'envoi UDP pour la requête de recherche.
         * @param portReponse Port de réception UDP local pour les réponses JSON.
         * @return Un vecteur de dictionnaires (Clé -> Valeur) représentant chaque lecteur détecté.
         */
    vector<map<string, string>> rechercherLecteurs(const string& ipMulticast, int portDecouverte, int portReponse);
};