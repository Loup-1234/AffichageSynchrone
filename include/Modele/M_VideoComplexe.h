/**
 * @file M_VideoComplexe.h
 * @brief Déclaration de la classe M_VideoComplexe pour la synchronisation audio et la composition vidéo via FFmpeg.
 * @author Alain HUMEAU
 * @date 2026
 */

#pragma once

#include <string>
#include <vector>

using namespace std;

/** @brief Fréquence utilisée pour l'échantillonnage des pistes audio extraites (16 kHz standard pour l'analyse). */
constexpr int FREQUENCE_ECHANTILLONNAGE = 16000;

/** @brief Préfixe utilisé pour nommer les fichiers binaires audio temporaires sur le disque. */
const string TEMP_AUDIO_PREFIX = "temp_audio_";

/**
 * @class M_VideoComplexe
 * @brief Module de traitement haute performance pour la synchronisation et la composition de vidéos.
 * * Cette classe extrait les bandes sonores des flux vidéos, calcule leurs décalages temporels
 * par corrélation croisée (analyse de signal) et génère des commandes de filtrage FFmpeg complexes
 * (mosaïques avec xstack, redimensionnement et décalages temporels).
 */
class M_VideoComplexe {
public:
    /**
     * @brief Orchestre la génération de la vidéo complexe synchronisée.
     * * Analyse les fichiers, extrait l'audio, calcule la matrice de synchronisation,
     * assemble le script FFmpeg complexe et l'exécute pour générer le rendu final.
     * @param listeFichiersEntree Pointeur vers un tableau de chaînes de caractères contenant les chemins des vidéos sources.
     * @param nbVideos Nombre total de vidéos présentes dans le tableau d'entrée.
     * @param fichierSortie Chemin complet ou relatif du fichier MP4 final à générer.
     * @param masquerReference Si vrai, la première vidéo (référence de synchro) ne sera pas affichée dans la mosaïque.
     * @param idLecteur Identifiant du lecteur appelant (utilisé pour isoler le nom des fichiers temporaires).
     */
    void genererVideoComplexe(const string *listeFichiersEntree, size_t nbVideos, const string &fichierSortie,
                              bool masquerReference = false, int idLecteur = 0);

private:
    /**
     * @brief Calcule les décalages (en secondes) de chaque vidéo par rapport à la première (considérée comme la référence).
     * @param audios Matrice contenant les amplitudes des signaux audio de chaque vidéo chargés en mémoire.
     * @param nbVideos Nombre de vidéos à analyser.
     * @param listeFichiers Tableau des noms de fichiers sources (essentiellement pour la traçabilité des logs).
     * @param idLecteur Identifiant unique du lecteur pour le suivi du contexte.
     * @return Un vecteur de doubles représentant le décalage temporel précis (en secondes) pour chaque vidéo.
     */
    vector<double> calculerDecalages(const vector<vector<float>> &audios, size_t nbVideos, const string *listeFichiers,
                                     int idLecteur);

    /**
     * @brief Génère la ligne de commande FFmpeg complexe (filtres xstack, scale, sync).
     * * Construit la chaîne d'arguments de filtrage `-filter_complex` en y appliquant les décalages
     * calculés via les options `setpts` et `asetpts` de FFmpeg.
     * @param listeFichiersEntree Tableau des chemins des fichiers vidéos d'entrée.
     * @param nbVideos Nombre de vidéos à intégrer dans la composition.
     * @param decalagesEnSecondes Tableau des décalages temporels calculés pour chaque flux.
     * @param fichierSortie Chemin du fichier de destination.
     * @param masquerReference Indicateur d'exclusion de la vidéo de référence dans le rendu visuel.
     * @param idLecteur Identifiant du lecteur.
     * @return La chaîne de caractères contenant la commande système FFmpeg prête à l'emploi.
     */
    string construireCommandeFFmpeg(const string *listeFichiersEntree, size_t nbVideos,
                                    const double *decalagesEnSecondes, const string &fichierSortie,
                                    bool masquerReference, int idLecteur);

    /**
     * @brief Extrait les pistes audio des vidéos en parallèle et les charge en mémoire.
     * * Appelle FFmpeg en arrière-plan pour convertir le flux audio en PCM brut (brut de flottants),
     * puis lit ces fichiers pour alimenter la matrice mémoire.
     * @param listeFichiersEntree Tableau des chemins des vidéos sources.
     * @param nbVideos Nombre de vidéos à traiter.
     * @param idLecteur Identifiant du lecteur pour la création des fichiers temporaires de travail.
     * @return Une matrice de flottants (vecteur de vecteurs) contenant l'historique d'amplitude de chaque signal.
     */
    vector<vector<float>> extraireEtChargerAudios(const string *listeFichiersEntree, size_t nbVideos, int idLecteur);

    /**
     * @brief Calcule la corrélation croisée entre deux signaux numériques pour identifier le décalage optimal.
     * * Détermine à quel échantillon précis le second signal se superpose le mieux avec le premier.
     * @param sig1 Pointeur vers le tableau de données du premier signal (le signal de référence).
     * @param taille1 Nombre d'échantillons du premier signal.
     * @param sig2 Pointeur vers le tableau de données du second signal (le signal à synchroniser).
     * @param taille2 Nombre d'échantillons du second signal.
     * @return L'indice de décalage (en nombre d'échantillons) maximisant la ressemblance entre les deux pistes.
     */
    int xcorr(const float *sig1, size_t taille1, const float *sig2, size_t taille2);

    /**
     * @brief Supprime l'ensemble des fichiers binaires temporaires audio créés sur le disque durant l'extraction.
     * @param nbVideos Nombre de vidéos dont il faut nettoyer les fichiers résiduels.
     * @param idLecteur Identifiant du lecteur permettant de cibler précisément les fichiers du contexte de session.
     */
    void nettoyerTemporaires(int nbVideos, int idLecteur);
};