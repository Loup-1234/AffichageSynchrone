#pragma once

#include "M_VideoComplexe.h"
#include "M_ProtocoleReseau.h"
#include "M_BDD.h"
#include "M_configReseau.h"
#include <string>
#include <vector>
#include <map>

using namespace std;

class M_SessionLecture {
public:
    M_SessionLecture(const string &ip, int port);

    void configurerLecteurs(const vector<LecteurConfig>& configs);
    void genererVideoComplexe(const vector<string>& listeFichiersEntree, const string& dossierSortie);
    void uploaderVideoComplexe(const string& dossierSource) const;

    vector<map<string, string>> rechercherLecteurs();

private:
    M_VideoComplexe instanceVideoComplexe;
    vector<LecteurConfig> m_lecteurs;
    M_BDD bdd;
    M_configReseau config;
};