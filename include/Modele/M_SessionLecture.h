#pragma once

#include "M_VideoComplexe.h"
#include "M_ProtocoleReseau.h"
#include <string>
#include <vector>
#include <map>

using namespace std;

class M_SessionLecture {
public:
    M_SessionLecture() = default;

    void configurerLecteurs(const vector<LecteurConfig>& configs);
    void genererVideoComplexe(const vector<string>& listeFichiersEntree, const string& dossierSortie);
    void uploaderVideoComplexe(const string& dossierSource) const;

    vector<map<string, string>> rechercherLecteurs(
        const string& ipMulticast, int portDecouverte, int portReponse);

private:
    M_VideoComplexe instanceVideoComplexe;
    vector<LecteurConfig> m_lecteurs;
};