
.. _program_listing_file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_SynchroniseurMultiVideo_SynchroniseurMultiVideo.h:

Program Listing for File SynchroniseurMultiVideo.h
==================================================

|exhale_lsh| :ref:`Return to documentation for file <file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_SynchroniseurMultiVideo_SynchroniseurMultiVideo.h>` (``C:\Users\ahumeaub\Desktop\CLionProject\TestAffichageSynchrone\include\SynchroniseurMultiVideo\SynchroniseurMultiVideo.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include <string>
   #include <vector>
   
   using namespace std;
   
   class SynchroniseurMultiVideo {
       const int FREQUENCE_ECHANTILLONNAGE = 44100;
   
       double dureeAnalyse = 60.0;
   
       double plageRechercheMax = 30.0;
   
       int pasDePrecision = 100;
   
       const int HAUTEUR_CIBLE = 480;
   
       const int LARGEUR_CIBLE = 854;
   
       const string TEMP_AUDIO_REF = "temp_ref.raw";
   
       const string TEMP_AUDIO_CIBLE = "temp_cible.raw";
   
       struct InfoVideo {
           string chemin; 
           double retardSecondes; 
       };
   
       void extraireAudio(const string &fichierVideo, const string &fichierAudioSortie) const;
   
       static vector<float> chargerAudioBrut(const string &nomFichier);
   
       double calculerDecalage(const vector<float> &ref, const vector<float> &cible) const;
   
       bool genererVideo(const vector<InfoVideo> &listeVideos, const string &fichierSortie,
                         const string &fichierAudioRef = "") const;
   
   public:
       ~SynchroniseurMultiVideo();
   
       void configurerAnalyse(double duree, double plage, int pas);
   
       bool genererVideoSynchronisee(const vector<string> &fichiersEntree, const string &fichierSortie) const;
   
       bool genererVideoSynchronisee(const string &fichierAudioRef, const vector<string> &fichiersVideo,
                                     const string &fichierSortie) const;
   };
