#pragma once

#include <string>
#include <vector>

using namespace std;

/** @brief Fréquence utilisée pour l'échantillonnage des pistes audio extraites. */
constexpr int FREQUENCE_ECHANTILLONNAGE = 16000;
/** @brief Préfixe utilisé pour les fichiers binaires audio temporaires. */
const string TEMP_AUDIO_PREFIX = "temp_audio_";

/**
 * @class M_VideoComplexe
 * @brief Module de traitement haute performance pour la synchronisation et la composition de vidéos.
 */
class M_VideoComplexe {
public:
    /**
     * @brief Orchestre la génération de la vidéo complexe synchronisée.
     */
    void genererVideoComplexe(const string* listeFichiersEntree, size_t nbVideos, const string &fichierSortie, bool masquerReference = false, int idLecteur = 0);

private:
    /**
     * @brief Calcule les décalages (en secondes) de chaque vidéo par rapport à la première (référence).
     */
    vector<double> calculerDecalages(const vector<vector<float>>& audios, size_t nbVideos, const string* listeFichiers, int idLecteur);

    /**
     * @brief Génère la ligne de commande FFmpeg complexe (filtres xstack, scale, sync).
     */
    string construireCommandeFFmpeg(const string *listeFichiersEntree, size_t nbVideos, const double *decalagesEnSecondes, const string &fichierSortie, bool masquerReference, int idLecteur);

    /**
     * @brief Extrait les pistes audio des vidéos en parallèle et les charge en mémoire.
     */
    vector<vector<float>> extraireEtChargerAudios(const string* listeFichiersEntree, size_t nbVideos, int idLecteur);

    /**
     * @brief Calcule la corrélation croisée entre deux signaux via FFT pour trouver le décalage optimal.
     */
    int xcorr(const float* sig1, size_t taille1, const float* sig2, size_t taille2);

    /**
     * @brief Supprime les fichiers binaires temporaires créés durant l'extraction.
     */
    void nettoyerTemporaires(int nbVideos, int idLecteur);
};