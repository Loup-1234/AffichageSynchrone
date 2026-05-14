#pragma once

#include <string>

using namespace std;

/** @brief Fréquence utilisée pour l'échantillonnage des pistes audio extraites. */
constexpr int FREQUENCE_ECHANTILLONNAGE = 44100;
/** @brief Préfixe utilisé pour les fichiers binaires audio temporaires. */
const string TEMP_AUDIO_PREFIX = "temp_audio_";

/**
 * @class M_VideoComplexe
 * @brief Module de traitement haute performance pour la synchronisation et la composition de vidéos.
 * * Cette classe permet de calculer les décalages temporels entre plusieurs vidéos par corrélation
 * croisée acoustique et de produire une vidéo mosaïque finale via FFmpeg.
 */
class M_VideoComplexe {
public:
    /**
     * @brief Orchestre la génération de la vidéo complexe synchronisée.
     * @param listeFichierEntree Tableau des chemins des vidéos sources.
     * @param nbVideos Nombre de vidéos à traiter.
     * @param fichierSortie Chemin du fichier MP4 final.
     */
    void genererVideoComplexe(const string* listeFichierEntree, size_t nbVideos, const string &fichierSortie);

private:
    /**
     * @brief Calcule les décalages (en secondes) de chaque vidéo par rapport à la première (référence).
     * @param audios Tableau de pointeurs vers les échantillons audio (float).
     * @param taillesAudios Tableau contenant le nombre d'échantillons par vidéo.
     * @param nbVideos Nombre de vidéos.
     * @param listeFichiers Chemins des fichiers (pour log et détection du type).
     * @return Un tableau dynamique de doubles contenant les décalages.
     */
    double* calculerDecalages(const float* const* audios, const size_t* taillesAudios, size_t nbVideos, const string* listeFichiers);

    /**
     * @brief Génère la ligne de commande FFmpeg complexe (filtres xstack, scale, sync).
     * @param listeFichierEntree Chemins des sources.
     * @param nbVideos Nombre de vidéos.
     * @param decalagesEnSecondes Valeurs de synchronisation calculées.
     * @param fichierSortie Destination du fichier.
     * @return La chaîne de caractères prête à être exécutée.
     */
    string construireCommandeFFmpeg(const string* listeFichierEntree, size_t nbVideos, const double* decalagesEnSecondes, const string &fichierSortie);

    /**
     * @brief Extrait les pistes audio des vidéos en parallèle et les charge en mémoire.
     * @param listeFichierEntree Chemins des sources.
     * @param nbVideos Nombre de vidéos.
     * @param taillesAudios [out] Tableau rempli avec les tailles des flux chargés.
     * @return Un tableau de pointeurs vers les données audio brutes.
     */
    float** extraireEtChargerAudios(const string* listeFichierEntree, size_t nbVideos, size_t* taillesAudios);

    /**
     * @brief Calcule la corrélation croisée entre deux signaux via FFT pour trouver le décalage optimal.
     * @param video1 Signal de référence.
     * @param taille1 Nombre d'échantillons signal 1.
     * @param video2 Signal à synchroniser.
     * @param taille2 Nombre d'échantillons signal 2.
     * @return Le décalage en nombre d'échantillons.
     */
    int xcorr(const float* video1, size_t taille1, const float* video2, size_t taille2);

    /**
     * @brief Supprime les fichiers binaires temporaires créés durant l'extraction.
     * @param nbVideos Nombre de fichiers à nettoyer.
     */
    void nettoyerTemporaires(int nbVideos);
};