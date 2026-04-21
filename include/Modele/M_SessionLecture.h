#pragma once

#include "M_VideoComplexe.h"
#include "../Modele/M_ServeurTFTP_W.h"

#include <vector>
#include <string>

using namespace std;

class M_SessionLecture {
private:
    M_VideoComplexe M_VideoComplexe;

    vector<int> IdLecteurs;
    vector<string> IpLecteurs;
    vector<int> NbVideos;

public:
    void preparerSessionLecture(const vector<vector<string>> &specLecteurs);
    void genererVideoComplexe(const vector<string> &listeFichierEntree);
    void uploaderVideoComplexe();
};