

#include <gtest/gtest.h>
#include <vector>
#include <stdexcept>

// Accès aux membres privés pour les tests
#define private public
#include "Modele/M_VideoComplexe.h"
#undef private

using namespace std;

// TESTS DE BASE

// TEST 1 : « vidéo1 » et « vidéo2 » alignées
TEST(TestsCorrelation, SignauxIdentiques) {
    M_VideoComplexe video;

    const vector<float> video1 = {0.0f, 1.0f, 2.0f, 3.0f, 2.0f, 1.0f, 0.0f};
    const vector<float> video2 = {0.0f, 1.0f, 2.0f, 3.0f, 2.0f, 1.0f, 0.0f};

    // Passage des 4 paramètres : pointeur1, taille1, pointeur2, taille2
    EXPECT_EQ(video.xcorr(video1.data(), video1.size(), video2.data(), video2.size()), 0);
}

// TEST 2 : « vidéo1 » en avance sur « vidéo2 »
TEST(TestsCorrelation, SignalDecale) {
    M_VideoComplexe video;

    const vector<float> video1 = {1.0f, 2.0f, 3.0f, 0.0f, 0.0f, 0.0f};
    const vector<float> video2 = {0.0f, 0.0f, 1.0f, 2.0f, 3.0f, 0.0f};

    const int decalage = video.xcorr(video1.data(), video1.size(), video2.data(), video2.size());

    // Vérifie si le décalage trouvé est de 2 (ou -2 selon l'ordre interne)
    EXPECT_TRUE(decalage == 2 || decalage == -2);
}

// TESTS AVANCÉS

// TEST 3 : « vidéo2 » en avance sur « vidéo1 » avec des tailles différentes
TEST(TestsCorrelation, TaillesDifferentes) {
    M_VideoComplexe video;

    const vector<float> video1 = {0.1f, 0.1f, 0.5f, 1.0f, 0.8f, 0.2f, 0.1f};
    const vector<float> video2 = {0.5f, 1.0f, 0.8f};

    const int decalage = video.xcorr(video1.data(), video1.size(), video2.data(), video2.size());

    // L'extrait commence à l'indice 2 de l'audio principal
    EXPECT_EQ(decalage, 2);
}

// TEST 4 : « vidéo1 » et « vidéo2 » alignées avec du bruit dans la « vidéo2 »
TEST(TestsCorrelation, SignalBruite) {
    M_VideoComplexe video;

    const vector<float> video1 = {0.0f, 1.0f, 2.0f, 3.0f, 2.0f, 1.0f, 0.0f};
    const vector<float> video2 = {0.02f, 0.95f, 2.05f, 2.98f, 2.01f, 0.99f, 0.01f};

    EXPECT_EQ(video.xcorr(video1.data(), video1.size(), video2.data(), video2.size()), 0);
}

// TEST 5 : « vidéo1 » et « vidéo2 » avec son totalement différent
TEST(TestsCorrelation, AucuneCorrelation) {
    M_VideoComplexe video;

    const vector<float> video1 = {1.0f, 1.0f, 1.0f};
    const vector<float> video2 = {-1.0f, -1.0f, -1.0f};

    // On vérifie au moins que l'algorithme ne plante pas
    EXPECT_NO_THROW(video.xcorr(video1.data(), video1.size(), video2.data(), video2.size()));
}

// TESTS DE GESTION D'ERREURS

// TEST 6 : « vidéo1 » sans son
TEST(TestsCorrelation, ErreurSignalVide) {
    M_VideoComplexe video;

    const vector<float> video1 = {};
    const vector<float> video2 = {1.0f, 2.0f, 3.0f};

    // L'absence de données doit lever une exception de type invalid_argument
    EXPECT_THROW(video.xcorr(video1.data(), video1.size(), video2.data(), video2.size()), std::invalid_argument);
    EXPECT_THROW(video.xcorr(video2.data(), video2.size(), video1.data(), video1.size()), std::invalid_argument);
}