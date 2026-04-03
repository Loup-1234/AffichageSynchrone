
.. _program_listing_file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_AffichageSynchrone_AffichageSynchrone.h:

Program Listing for File AffichageSynchrone.h
=============================================

|exhale_lsh| :ref:`Return to documentation for file <file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_AffichageSynchrone_AffichageSynchrone.h>` (``C:\Users\ahumeaub\Desktop\CLionProject\TestAffichageSynchrone\include\AffichageSynchrone\AffichageSynchrone.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "raylib.h"
   #include <vlc/vlc.h>
   #include <string>
   #include <vector>
   #include <atomic>
   #include <mutex>
   #include <thread>
   
   // Forward declarations pour les callbacks C de VLC
   static void *verrouiller(void *donnees, void **p_pixels);
   static void deverrouiller(void *donnees);
   static void afficherPixels(const void *donnees);
   static unsigned configurerVideo(void **donnees, char *chroma, const unsigned *largeur, const unsigned *hauteur, unsigned *pas, unsigned *lignes);
   static void nettoyerVideo(void *donnees);
   
   class AffichageSynchrone {
   public:
       explicit AffichageSynchrone();
   
       ~AffichageSynchrone();
   
       void executer();
   
       void setVolume(float volume);
   
   private:
       // --- VLC & Gestion Vidéo ---
       libvlc_instance_t *instanceVLC = nullptr;
       libvlc_media_player_t *lecteurVLC = nullptr;
       Texture2D textureVideo{};
       std::vector<unsigned char> pixelsVideo;
       std::mutex mutexImage; 
       unsigned int largeurVideo = 0;
       unsigned int hauteurVideo = 0;
       std::string cheminVideoComplexe = "sortie_synchro.mp4";
       float duree{};
   
       // --- Callbacks VLC (Amis) ---
       friend void *verrouiller(void *donnees, void **p_pixels);
       friend void deverrouiller(void *donnees);
       friend void afficherPixels(const void *donnees);
       friend unsigned configurerVideo(void **donnees, char *chroma, const unsigned *largeur, const unsigned *hauteur, unsigned *pas, unsigned *lignes);
       friend void nettoyerVideo(void *donnees);
   
       // --- État de l'Interface Utilisateur ---
       float valeurSliderProgression = 0.0f;
       float valeurSliderSon = 100.0f;
       float valeurSliderSonPrecedent{};
       bool estMuet = false;
   
       // --- Gestion de la Génération ---
       std::atomic<bool> videoGeneree{false};
       std::atomic<bool> generationEnCours{false};
       std::atomic<bool> framePrete{false};
       std::atomic<bool> textureDoitEtreRedimensionnee{false};
       std::thread threadGeneration;
   
       // --- Gestion des Fichiers ---
       std::vector<std::string> fichiersVideo;
       std::vector<bool> videosSelectionnees;
       std::vector<int> ordreSelection;
       Vector2 positionDefilement = {0, 0};
   
       // --- Constantes UI ---
       const char *TEXTE_BOUTON_GENERER = "Générer";
       const char *TEXTE_BOUTON_OUVRIR_DOSSIER = "Ouvrir dossier";
   
       // --- Layout ---
       Rectangle zones[11]{};
   
       // --- Méthodes Internes ---
       void chargerListeVideos();
       void chargerVideo();
       void generer();
       static void ouvrirDossierVideos();
       void lecturePause() const;
       void son();
       void barreProgression(bool &enGlissement, bool &etaitEnLecture, float &delaiRecherche);
       void barreVolume();
       void afficherVideo();
       void afficherListeFichiers();
       void miseAJourDisposition();
   };
