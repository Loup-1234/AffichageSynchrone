#pragma once

#include <vector>
#include <string>

using namespace std;

constexpr int FREQUENCE_ECHANTILLONNAGE = 44100;
const string TEMP_AUDIO_PREFIX = "temp_audio_";

class M_VideoComplexe {
public:
    void genererVideoComplexe(const vector<string> &listeFichierEntree, const string &fichierSortie);

private:

    vector<double> calculerDecalages(const vector<vector<float> > &audios, const vector<string>& listeFichiers);

    string construireCommandeFFmpeg(const vector<string> &listeFichierEntree, const vector<double> &decalagesEnSecondes,
                                    const string &fichierSortie);

    vector<vector<float> > extraireEtChargerAudios(const vector<string> &listeFichierEntree);

    int xcorr(const vector<float> &sig1, const vector<float> &sig2);

    void nettoyerTemporaires(int nbVideos);
};
