#pragma once

#include <string>

using namespace std;

constexpr int FREQUENCE_ECHANTILLONNAGE = 44100;
const string TEMP_AUDIO_PREFIX = "temp_audio_";

class M_VideoComplexe {
public:
    void genererVideoComplexe(const string* listeFichierEntree, size_t nbVideos, const string &fichierSortie);

private:
    double* calculerDecalages(const float* const* audios, const size_t* taillesAudios, size_t nbVideos, const string* listeFichiers);

    string construireCommandeFFmpeg(const string* listeFichierEntree, size_t nbVideos, const double* decalagesEnSecondes, const string &fichierSortie);

    float** extraireEtChargerAudios(const string* listeFichierEntree, size_t nbVideos, size_t* taillesAudios);

    int xcorr(const float* video1, size_t taille1, const float* video2, size_t taille2);

    void nettoyerTemporaires(int nbVideos);
};