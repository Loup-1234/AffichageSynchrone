#pragma once

#include "M_VideoComplexe.h"
#include "M_ServeurTFTP_W.h"

#include <string>

using namespace std;

struct LecteurSpec {
    string id;
    string ip;
    string nbVideos;
};

class M_SessionLecture {

    M_VideoComplexe instanceVideoComplexe;

    int* idLecteurs = nullptr;
    string* ipLecteurs = nullptr;
    int* nbVideos = nullptr;
    size_t nbLecteursTotal = 0;

public:
    M_SessionLecture() = default;
    ~M_SessionLecture();

    void preparerSessionLecture(const LecteurSpec* specLecteurs, size_t nbLecteurs);
    void genererVideoComplexe(const string* listeFichiersEntree, size_t nbFichiers);
    void uploaderVideoComplexe();
};