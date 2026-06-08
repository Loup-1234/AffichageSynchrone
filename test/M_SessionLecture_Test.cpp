#include <gtest/gtest.h>

// Accès aux membres privés pour les tests
#define private public
#include "Modele/M_SessionLecture.h"
#undef private

// Fixture de test (elle seule est officiellement amie avec la classe)
class M_SessionLecture_Test : public ::testing::Test {
protected:
    M_SessionLecture* session;

    void SetUp() override {
        session = new M_SessionLecture("127.0.0.1", 1234);
    }

    void TearDown() override {
        delete session;
    }

    // Méthodes passerelles permettant aux tests d'accéder proprement aux données privées
    void executerCalculCapacites(int nombreTotalVideos) {
        session->calculerCapacitesVideo(nombreTotalVideos);
    }

    std::vector<LecteurConfig>& lecteurs() {
        return session->m_lecteurs;
    }

    std::vector<std::vector<std::string>>& cacheConfigReseau() {
        return session->m_cacheConfigReseau;
    }
};

// -----------------------------------------------------------------------------
// CAS DE TEST 1 : Les conditions de garde (Sécurité)
// -----------------------------------------------------------------------------
TEST_F(M_SessionLecture_Test, TestConditionsDeGarde) {
    // Cas A : Liste de lecteurs vide
    lecteurs().clear();
    executerCalculCapacites(5);
    SUCCEED(); // Si ça n'a pas crashé, le test est validé

    // Cas B : Nombre de vidéos négatif ou nul
    lecteurs().push_back({.id = 0, .mac = "", .ip = "", .nbVideosCapacite = 0});
    executerCalculCapacites(0);
    EXPECT_EQ(lecteurs()[0].nbVideosCapacite, 0);
}

// -----------------------------------------------------------------------------
// CAS DE TEST 2 : Le Master est seul (Cas particulier)
// -----------------------------------------------------------------------------
TEST_F(M_SessionLecture_Test, TestMasterSeulPrendTout) {
    lecteurs().push_back({.id = 0, .mac = "", .ip = "", .nbVideosCapacite = 0});
    executerCalculCapacites(6);

    // Le Master seul doit récupérer l'intégralité du flux
    EXPECT_EQ(lecteurs()[0].nbVideosCapacite, 6);
}

// -----------------------------------------------------------------------------
// CAS DE TEST 3 : Répartition proportionnelle (Prorata et lissage)
// -----------------------------------------------------------------------------
TEST_F(M_SessionLecture_Test, TestRepartitionProportionnelleEtSlots) {
    // Injection d'un écran 1080p dans le cache réseau (Format : MAC, IP, OS, Largeur, Hauteur)
    cacheConfigReseau().push_back({"00:1A:2B:3C:4D:5E", "192.168.1.50", "Linux", "1920", "1080"});

    std::vector<LecteurConfig> configurationSalles = {
        {.id = 0, .mac = "", .ip = "", .nbVideosCapacite = 0},
        {.id = 1, .mac = "00:1A:2B:3C:4D:5E", .ip = "192.168.1.50", .nbVideosCapacite = 0}
    };
    session->configurerLecteurs(configurationSalles);

    // Lancement du calcul pour 4 vidéos au total
    executerCalculCapacites(4);

    // Vérification de la répartition attendue (2 slots pour Master, 3 pour le Client)
    EXPECT_EQ(lecteurs()[0].nbVideosCapacite, 2);
    EXPECT_EQ(lecteurs()[1].nbVideosCapacite, 3);
}

// -----------------------------------------------------------------------------
// CAS DE TEST 4 : Résolution inconnue (Fallback de sécurité)
// -----------------------------------------------------------------------------
TEST_F(M_SessionLecture_Test, TestFallbackResolutionInconnue) {
    std::vector<LecteurConfig> configurationSalles = {
        {.id = 0, .mac = "", .ip = "", .nbVideosCapacite = 0},
        {.id = 1, .mac = "00:1A:2B:3C:4D:5F", .ip = "10.0.0.99", .nbVideosCapacite = 0} // Absent du cache
    };
    session->configurerLecteurs(configurationSalles);

    // L'algorithme doit appliquer le fallback par défaut (1080p) sans lever d'exception
    ASSERT_NO_THROW(executerCalculCapacites(3));
    EXPECT_GT(lecteurs()[1].nbVideosCapacite, 0);
}