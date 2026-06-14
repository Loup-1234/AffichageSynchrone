#ifdef _WIN32

#include <gtest/gtest.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>

#include "Modele/M_UDP.h"

class ExpediteurUDPTest : public testing::Test {
protected:
    SOCKET socketRecepteur = INVALID_SOCKET;
    const std::string IP_TEST = "127.0.0.1";
    static constexpr int PORT_TEST = 54321;

    void SetUp() override {
        WSADATA donneesWsa;
        ASSERT_EQ(WSAStartup(MAKEWORD(2, 2), &donneesWsa), 0) << "Erreur WSAStartup";

        socketRecepteur = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        ASSERT_NE(socketRecepteur, INVALID_SOCKET) << "Erreur création socket";

        // Permettre la réutilisation d'adresse pour éviter les conflits avec le bind de la classe
        int opt = 1;
        setsockopt(socketRecepteur, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&opt), sizeof(opt));

        sockaddr_in adresseRecepteur{};
        adresseRecepteur.sin_family = AF_INET;
        adresseRecepteur.sin_port = htons(PORT_TEST);
        inet_pton(AF_INET, IP_TEST.c_str(), &adresseRecepteur.sin_addr);

        ASSERT_EQ(bind(socketRecepteur, (sockaddr*)&adresseRecepteur, sizeof(adresseRecepteur)), 0)
            << "Erreur bind";

        // Augmentation du délai d'attente à 2 secondes pour encaisser le non-bloquant de l'émetteur
        const DWORD delaiAttente = 2000;
        setsockopt(socketRecepteur, SOL_SOCKET, SO_RCVTIMEO, (const char *)&delaiAttente, sizeof(delaiAttente));
    }

    void TearDown() override {
        if (socketRecepteur != INVALID_SOCKET) {
            closesocket(socketRecepteur);
        }
        WSACleanup();
    }

    int recevoirPaquetLocal(PaquetControle &paquetSortie) const {
        sockaddr_in adresseExpediteur{};
        int tailleAdresseExpediteur = sizeof(adresseExpediteur);
        return recvfrom(socketRecepteur, (char *)&paquetSortie, sizeof(paquetSortie), 0,
                        (sockaddr *)&adresseExpediteur, &tailleAdresseExpediteur);
    }
};


// TESTS NOMINAUX

// TEST 1 : Vérifier l'envoi d'une commande "LECTURE_PAUSE"
TEST_F(ExpediteurUDPTest, Envoyer_LecturePause) {
    M_UDP expediteur;

    // Utilisation de l'initialisation Émetteur classique (Unicast) vers localhost
    ASSERT_TRUE(expediteur.initialiserEmetteur(IP_TEST, PORT_TEST)) << "Échec initialisation émetteur";

    PaquetControle paquetAEnvoyer{Expediteur::MASTER, TypeCommande::ORDRE, Action::PLAY, 0.0f};

    bool succesEnvoi = expediteur.envoyer(&paquetAEnvoyer, sizeof(paquetAEnvoyer));
    EXPECT_TRUE(succesEnvoi) << "La fonction envoyer() a retourne false.";

    // Un léger sleep pour laisser le temps à la socket non-bloquante de flush le buffer Windows
    Sleep(10);

    PaquetControle paquetRecu{};
    const int octetsLus = recevoirPaquetLocal(paquetRecu);

    ASSERT_EQ(octetsLus, sizeof(PaquetControle)) << "Taille de paquet incorrecte ou paquet non recu (timeout).";
    EXPECT_EQ(paquetRecu.exp, Expediteur::MASTER);
    EXPECT_EQ(paquetRecu.type, TypeCommande::ORDRE);
    EXPECT_EQ(paquetRecu.action, Action::PLAY);
    EXPECT_FLOAT_EQ(paquetRecu.valeur, 0.0f);
}

// TEST 2 : Vérifier l'envoi d'une commande "VOLUME"
TEST_F(ExpediteurUDPTest, Envoyer_Volume) {
    M_UDP expediteur;

    ASSERT_TRUE(expediteur.initialiserEmetteur(IP_TEST, PORT_TEST)) << "Échec initialisation émetteur";

    PaquetControle paquetAEnvoyer{Expediteur::MASTER, TypeCommande::ORDRE, Action::VOLUME, 0.45f};

    bool succesEnvoi = expediteur.envoyer(&paquetAEnvoyer, sizeof(paquetAEnvoyer));
    EXPECT_TRUE(succesEnvoi) << "La fonction envoyer() a retourne false.";

    Sleep(10);

    PaquetControle paquetRecu{};
    const int octetsLus = recevoirPaquetLocal(paquetRecu);

    ASSERT_EQ(octetsLus, sizeof(PaquetControle)) << "Taille de paquet incorrecte ou paquet non recu (timeout).";
    EXPECT_EQ(paquetRecu.exp, Expediteur::MASTER);
    EXPECT_EQ(paquetRecu.type, TypeCommande::ORDRE);
    EXPECT_EQ(paquetRecu.action, Action::VOLUME);
    EXPECT_FLOAT_EQ(paquetRecu.valeur, 0.45f);
}

// TEST 3 : Vérifier l'envoi d'une commande "PROGRESSION"
TEST_F(ExpediteurUDPTest, Envoyer_Progression) {
    M_UDP expediteur;

    ASSERT_TRUE(expediteur.initialiserEmetteur(IP_TEST, PORT_TEST)) << "Échec initialisation émetteur";

    PaquetControle paquetAEnvoyer{Expediteur::MASTER, TypeCommande::ORDRE, Action::PROGRESSION, 75.5f};

    bool succesEnvoi = expediteur.envoyer(&paquetAEnvoyer, sizeof(paquetAEnvoyer));
    EXPECT_TRUE(succesEnvoi) << "La fonction envoyer() a retourne false.";

    Sleep(10);

    PaquetControle paquetRecu{};
    const int octetsLus = recevoirPaquetLocal(paquetRecu);

    ASSERT_EQ(octetsLus, sizeof(PaquetControle)) << "Taille de paquet incorrecte ou paquet non recu (timeout).";
    EXPECT_EQ(paquetRecu.exp, Expediteur::MASTER);
    EXPECT_EQ(paquetRecu.type, TypeCommande::ORDRE);
    EXPECT_EQ(paquetRecu.action, Action::PROGRESSION);
    EXPECT_FLOAT_EQ(paquetRecu.valeur, 75.5f);
}

#endif