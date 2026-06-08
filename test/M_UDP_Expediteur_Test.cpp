#ifdef _WIN32

#pragma once

#include <gtest/gtest.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>

// Accès aux membres privés pour les tests
#define private public
#include "Modele/M_UDP.h"
#undef private

// Héritage avec testing::Test pour exécuter du code avant et après chaque test.
class ExpediteurUDPTest : public testing::Test {
protected: // Les membres 'protected' sont accessibles par les tests (TEST_F)

    SOCKET socketRecepteur = INVALID_SOCKET;
    const std::string IP_TEST = "127.0.0.1";
    static constexpr int PORT_TEST = 54321;

    // Méthode exécutée AUTOMATIQUEMENT AVANT chaque test
    void SetUp() override {
        // 1. Initialisation de l'API Winsock
        WSADATA donneesWsa;
        ASSERT_EQ(WSAStartup(MAKEWORD(2, 2), &donneesWsa), 0) << "Erreur WSAStartup";

        // 2. Création du socket UDP (AF_INET = IPv4, SOCK_DGRAM = UDP)
        socketRecepteur = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        ASSERT_NE(socketRecepteur, INVALID_SOCKET) << "Erreur création socket";

        // 3. Configuration de l'adresse de réception
        sockaddr_in adresseRecepteur{};
        adresseRecepteur.sin_family = AF_INET;
        adresseRecepteur.sin_port = htons(PORT_TEST);
        inet_pton(AF_INET, IP_TEST.c_str(), &adresseRecepteur.sin_addr);

        // 4. Lier le socket à l'adresse et au port pour écouter les messages
        ASSERT_EQ(bind(socketRecepteur, (sockaddr*)&adresseRecepteur, sizeof(adresseRecepteur)), 0)
            << "Erreur bind";

        // 5. Configuration d'un délai d'attente
        const DWORD delaiAttente = 1000; // 1 seconde
        setsockopt(socketRecepteur, SOL_SOCKET, SO_RCVTIMEO, (char *)&delaiAttente,
            sizeof(delaiAttente));
    }

    // Méthode exécutée AUTOMATIQUEMENT APRÈS chaque test
    void TearDown() override {
        if (socketRecepteur != INVALID_SOCKET) {
            closesocket(socketRecepteur);
        }
        WSACleanup();
    }

    // Méthode utilitaire personnalisée pour lire un paquet reçu
    int recevoirPaquetLocal(PaquetControle &paquetSortie) const {
        sockaddr_in adresseExpediteur{};
        int tailleAdresseExpediteur = sizeof(adresseExpediteur);

        // recvfrom lit les données arrivant sur le socket UDP et les stocke dans 'paquetSortie'
        return recvfrom(socketRecepteur, (char *)&paquetSortie, sizeof(paquetSortie), 0,
                        (sockaddr *)&adresseExpediteur, &tailleAdresseExpediteur);
    }
};


// TESTS NOMINAUX

// TEST 1 : Vérifier l'envoi d'une commande "LECTURE_PAUSE"
TEST_F(ExpediteurUDPTest, Envoyer_LecturePause) {
    M_UDP expediteur(IP_TEST, PORT_TEST);

    // 1. Préparation manuelle du paquet
    // On crée la structure avec le type et la valeur souhaités
    PaquetControle paquetAEnvoyer{Expediteur::MASTER, TypeCommande::ORDRE, Action::PLAY, 0.0f};

    // 2. Envoi des données via la méthode 'envoyer'
    // On passe l'adresse du paquet (&) et sa taille en octets
    bool succesEnvoi = expediteur.envoyer(&paquetAEnvoyer, sizeof(paquetAEnvoyer));

    // On vérifie que la méthode a bien retourné 'true'
    EXPECT_TRUE(succesEnvoi) << "La fonction envoyer() a retourne false.";

    // 3. Réception
    PaquetControle paquetRecu{};
    const int octetsLus = recevoirPaquetLocal(paquetRecu);

    // 4. Vérifications
    ASSERT_EQ(octetsLus, sizeof(PaquetControle)) << "Taille de paquet incorrecte ou paquet non recu (timeout).";
    EXPECT_EQ(paquetRecu.exp, Expediteur::MASTER);
    EXPECT_EQ(paquetRecu.type, TypeCommande::ORDRE);
    EXPECT_EQ(paquetRecu.action, Action::PLAY);
    EXPECT_FLOAT_EQ(paquetRecu.valeur, 0.0f);
}

// TEST 2 : Vérifier l'envoi d'une commande "VOLUME"
TEST_F(ExpediteurUDPTest, Envoyer_Volume) {
    M_UDP expediteur(IP_TEST, PORT_TEST);

    // 1. Préparation de la commande de volume à 45%
    PaquetControle paquetAEnvoyer{Expediteur::MASTER, TypeCommande::ORDRE, Action::VOLUME, 0.45f};

    // 2. Envoi des données
    bool succesEnvoi = expediteur.envoyer(&paquetAEnvoyer, sizeof(paquetAEnvoyer));
    EXPECT_TRUE(succesEnvoi) << "La fonction envoyer() a retourne false.";

    // 3. Réception
    PaquetControle paquetRecu{};
    const int octetsLus = recevoirPaquetLocal(paquetRecu);

    // 4. Vérifications
    ASSERT_EQ(octetsLus, sizeof(PaquetControle)) << "Taille de paquet incorrecte ou paquet non recu (timeout).";
    EXPECT_EQ(paquetRecu.exp, Expediteur::MASTER);
    EXPECT_EQ(paquetRecu.type, TypeCommande::ORDRE);
    EXPECT_EQ(paquetRecu.action, Action::VOLUME);
    EXPECT_FLOAT_EQ(paquetRecu.valeur, 0.45f);
}

// TEST 3 : Vérifier l'envoi d'une commande "PROGRESSION"
TEST_F(ExpediteurUDPTest, Envoyer_Progression) {
    M_UDP expediteur(IP_TEST, PORT_TEST);

    // 1. Préparation de la commande de progression à 75.5%
    PaquetControle paquetAEnvoyer{Expediteur::MASTER, TypeCommande::ORDRE, Action::PROGRESSION, 75.5f};

    // 2. Envoi des données
    bool succesEnvoi = expediteur.envoyer(&paquetAEnvoyer, sizeof(paquetAEnvoyer));
    EXPECT_TRUE(succesEnvoi) << "La fonction envoyer() a retourne false.";

    // 3. Réception
    PaquetControle paquetRecu{};
    const int octetsLus = recevoirPaquetLocal(paquetRecu);

    // 4. Vérifications
    ASSERT_EQ(octetsLus, sizeof(PaquetControle)) << "Taille de paquet incorrecte ou paquet non recu (timeout).";
    EXPECT_EQ(paquetRecu.exp, Expediteur::MASTER);
    EXPECT_EQ(paquetRecu.type, TypeCommande::ORDRE);
    EXPECT_EQ(paquetRecu.action, Action::PROGRESSION);
    EXPECT_FLOAT_EQ(paquetRecu.valeur, 75.5f);
}

#endif