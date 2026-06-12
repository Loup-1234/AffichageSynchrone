#include <gtest/gtest.h>
#include "../include/Modele/M_BDD.h"

//test recuperer données
TEST(recupereDonneesTest, recuperationApresInsertion) {
    M_BDD db;
    string idtest = "888";

    db.enregistrerDonnees("video", "id_video, temps, id_musique", idtest + ", '01:20', '10'");

    vector<vector<string>> resultat = db.recupereDonnees("*", "video", "id_video = " + idtest);

    ASSERT_FALSE(resultat.empty());
    EXPECT_EQ(resultat[0][0], idtest);

    db.supprimerDonnees("video", "id_video = " + idtest);
}

TEST(recupereDonneesTest, SelectionColonnesSpecifiques) {
    M_BDD db;
    string idTest = "777";

    db.enregistrerDonnees("video", "id_video, temps, id_musique", idTest + ", '05:00', '1'");

    vector<vector<string>> resultat = db.recupereDonnees("temps", "video", "id_video = " + idTest);

    ASSERT_EQ(resultat.size(), 1);
    EXPECT_EQ(resultat[0][0], "05:00");

    db.supprimerDonnees("video", "id_video = " + idTest);
}

TEST(recupereDonneesTest, ElementInexistant) {
    M_BDD db;

    vector<vector<string>> resultat = db.recupereDonnees("*", "video", "id_video = 99999");

    EXPECT_TRUE(resultat.empty());
    EXPECT_EQ(resultat.size(), 0);
}

TEST(recupereDonneesTest, TableInexistante) {
    M_BDD db;

    vector<vector<string>> resultat = db.recupereDonnees("*", "tableFantome", "");

    EXPECT_TRUE(resultat.empty());
}


//test enregistrer données
TEST(enregistrerDonneesTest, parametreValide) {
    M_BDD db;
    string idTest = "69";

    int resultat = db.enregistrerDonnees("video", "id_video, temps, id_musique", idTest + ", '03:45', '99'");
    EXPECT_EQ(resultat, 0);

    vector<vector<string>> table = db.recupereDonnees("*", "video", "id_video = " + idTest);

    db.supprimerDonnees("video", "1");

    ASSERT_EQ(table.size(), 1);
    EXPECT_EQ(table[0][0], idTest);
}

TEST(enregistrerDonneesTest, tableInexistante) {
    M_BDD db;

    int resultat = db.enregistrerDonnees("table_fantome", "colonneFantome", "1");

    EXPECT_NE(resultat, 0);
}

TEST(enregistrerDonneesTest, colonneInvalide) {
    M_BDD db;

    int resultat = db.enregistrerDonnees("video", "id_video, temps", "500");

    EXPECT_NE(resultat, 0);
}
