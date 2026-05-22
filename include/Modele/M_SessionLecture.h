#pragma once

#include "M_VideoComplexe.h"
#include "M_ProtocoleReseau.h"
#include <string>
#include <vector>
#include <map>

class M_SessionLecture {
public:
    M_SessionLecture() = default;

    void configurerLecteurs(const std::vector<LecteurConfig>& configs);
    void genererVideoComplexe(const std::vector<std::string>& listeFichiersEntree, const std::string& dossierSortie);
    void uploaderVideoComplexe(const std::string& dossierSource) const;

    std::vector<std::map<std::string, std::string>> rechercherLecteurs(
        const std::string& ipMulticast, int portDecouverte, int portReponse);

private:
    M_VideoComplexe instanceVideoComplexe;
    std::vector<LecteurConfig> m_lecteurs;
};