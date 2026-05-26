#pragma once

#include <string>
#include <vector>

using namespace std;

/**
 * @class M_VideoComplexe
 * @brief Moteur de rendu multimédia exploitant FFmpeg pour la composition de flux.
 * * Cette classe encapsule la logique système de traitement vidéo. Elle assure l'isolation
 * des processus de rendu et la gestion sécurisée des ressources (fichiers temporaires, threads).
 */
class M_VideoComplexe {
public:
    /** @brief Constructeur par défaut. */
    M_VideoComplexe() = default;
    /** @brief Destructeur par défaut. */
    ~M_VideoComplexe() = default;

    // Suppression de la copie pour garantir l'unicité du moteur de rendu
    M_VideoComplexe(const M_VideoComplexe&) = delete;
    M_VideoComplexe& operator=(const M_VideoComplexe&) = delete;

    /**
     * @brief Génère une vidéo composite à partir de plusieurs flux sources.
     * @param listeFichiers Pointeur vers le tableau des chemins d'accès des vidéos.
     * @param nbVideos Nombre total de vidéos à composer.
     * @param cheminSortie Chemin complet du fichier MP4 final généré.
     */
    void genererVideoComplexe(const string *listeFichiers, size_t nbVideos, const string &cheminSortie);

private:
    /**
     * @brief Construit la ligne de commande système pour l'appel de FFmpeg.
     * @param listeFichiers Tableau des chemins des vidéos.
     * @param nbVideos Nombre de vidéos.
     * @param decalages Pointeur vers les décalages de synchronisation temporelle.
     * @param fichierSortie Chemin du fichier de destination.
     * @return La commande shell complète formatée pour l'encodage H.264.
     */
    string construireCommandeFFmpeg(const string *listeFichiers, size_t nbVideos,
                                     const double *decalages, const string &fichierSortie);

    static constexpr int FPS = 30;                 ///< Fréquence d'image cible pour le rendu.
    static constexpr int LARGEUR_BASE = 640;       ///< Largeur standard d'un flux vidéo dans la mosaïque.
    static constexpr int HAUTEUR_BASE = 360;       ///< Hauteur standard d'un flux vidéo dans la mosaïque.
};