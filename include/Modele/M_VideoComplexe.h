#pragma once

#include <string>
#include <vector>
#include <future>

using namespace std;

/**
 * @class M_VideoComplexe
 * @brief Moteur de traitement multimédia exploitant FFmpeg pour extraire, analyser et fusionner les flux vidéo.
 */
class M_VideoComplexe {
public:
    /**
     * @brief Point d'entrée pour la génération de la vidéo mosaïque ou du flux unique calibré.
     * @param listeFichiers Tableau contenant les chemins d'accès vers les vidéos sources.
     * @param nbVideos Nombre de vidéos présentes au sein du tableau.
     * @param cheminSortie Chemin d'enregistrement complet du fichier MP4 composite généré.
     */
    void genererVideoComplexe(const string *listeFichiers, size_t nbVideos, const string &cheminSortie);

private:
    /**
     * @brief Extrait de manière asynchrone et parallèle les pistes audio brutes des fichiers sources.
     * @param listeFichiersEntree Tableau des chemins d'accès des fichiers vidéo.
     * @param nbVideos Nombre total de vidéos à traiter en parallèle.
     * @param taillesAudios Tableau complété par la méthode pour stocker le nombre d'échantillons de chaque flux.
     * @return Un tableau de pointeurs vers les segments de données audio flottantes (float*).
     */
    float **extraireEtChargerAudios(const string *listeFichiersEntree, size_t nbVideos, size_t *taillesAudios);

    /**
     * @brief Construit la syntaxe de la commande shell FFmpeg adaptée selon le nombre de vidéos à traiter.
     * @param listeFichiersEntree Tableau des chemins d'accès des fichiers vidéo.
     * @param nbVideos Nombre total de vidéos associées au lecteur.
     * @param decalagesEnSecondes Tableau des décalages temporels de synchronisation calculés pour chaque flux.
     * @param fichierSortie Chemin cible du fichier MP4 de sortie.
     * @return La chaîne de caractères de la commande FFmpeg prête à l'exécution système.
     */
    string construireCommandeFFmpeg(const string *listeFichiersEntree, size_t nbVideos, const double *decalagesEnSecondes, const string &fichierSortie);

    static constexpr int FREQUENCE_ECHANTILLONNAGE = 44100; ///< Fréquence de référence utilisée pour l'analyse audio.
    const string TEMP_AUDIO_PREFIX = "temp_audio_";        ///< Préfixe des fichiers binaires temporaires d'extraction.
};