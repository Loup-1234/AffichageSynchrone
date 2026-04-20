#pragma once

#include "../VideoComplexe/VideoComplexe.h"
#include "../ServeurTFTP_W/ServeurTFTP_W.h"

#include <vector>
#include <string>

using namespace std;

class SessionLecture {
private:
    VideoComplexe videoComplexe;

    vector<int> IdLecteurs;
    vector<string> IpLecteurs;
    vector<int> NbVideos;

public:
    void preparerSessionLecture(const vector<vector<string>> &specLecteurs);
    void genererVideoComplexe(const vector<string> &listeFichierEntree);
    void uploaderVideoComplexe();
};