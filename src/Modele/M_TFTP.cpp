#ifdef _WIN32

#include "modele/M_TFTP.h"

#include <filesystem>

M_TFTP::M_TFTP() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
}

M_TFTP::~M_TFTP() {
    WSACleanup();
}

void M_TFTP::envoyer(string ipMaster, string cheminJson) {
    SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == INVALID_SOCKET) {
        cerr << "[DEBUG] [Session Upload TFTP] Erreur lors de la creation de la socket." << endl;
        return;
    }

    sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_port = htons(69);
    inet_pton(AF_INET, ipMaster.c_str(), &server.sin_addr);

    const char* filename = cheminJson.c_str();
    const char* mode = "octet";

    vector<char> wrq;
    wrq.push_back(0); wrq.push_back(2);
    for (size_t i = 0; i < strlen(filename); i++) wrq.push_back(filename[i]);
    wrq.push_back(0);
    for (size_t i = 0; i < strlen(mode); i++) wrq.push_back(mode[i]);
    wrq.push_back(0);

    sendto(sock, wrq.data(), (int)wrq.size(), 0, (sockaddr*)&server, sizeof(server));
    cout << "[DEBUG] [Session Upload TFTP] Requete WRQ envoyee pour " << cheminJson << " vers " << ipMaster << " sur le port 69." << endl;

    DWORD tv = 3000;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));

    char buf[516];
    sockaddr_in peer{};
    int peerLen = sizeof(peer);
    int n = recvfrom(sock, buf, sizeof(buf), 0, (sockaddr*)&peer, &peerLen);

    if (n < 4 || buf[1] != 4 || buf[2] != 0 || buf[3] != 0) {
        cerr << "[DEBUG] [Session Upload TFTP] Erreur : ACK(0) non recu." << endl;
        closesocket(sock);
        return;
    }
    cout << "[DEBUG] [Session Upload TFTP] ACK(0) recu, debut du transfert..." << endl;

    ifstream file(cheminJson, ios::binary);
    if (!file) {
        cerr << "[DEBUG] [Session Upload TFTP] Impossible d'ouvrir le fichier : " << cheminJson << endl;
        closesocket(sock);
        return;
    }

    uint16_t block = 1;

    while (!file.eof()) {
        file.read(buf + 4, BLOCK_SIZE);
        int bytesRead = (int)file.gcount();
        buf[0] = 0; buf[1] = 3;
        buf[2] = (block >> 8) & 0xFF;
        buf[3] = block & 0xFF;

        int tries = 0;
        bool acked = false;

        while (tries < MAX_RETRIES) {
            sendto(sock, buf, bytesRead + 4, 0, (sockaddr*)&peer, peerLen);
            n = recvfrom(sock, buf, sizeof(buf), 0, (sockaddr*)&peer, &peerLen);

            if (n >= 4 && buf[1] == 4) {
                uint16_t ackBlock = ((unsigned char)buf[2] << 8) | (unsigned char)buf[3];
                if (ackBlock == block) { acked = true; break; }
            }
            tries++;
            cout << "[DEBUG] [Session Upload TFTP] Timeout atteint, tentative " << tries << " sur " << MAX_RETRIES << "..." << endl;
        }

        if (!acked) {
            cerr << "[DEBUG] [Session Upload TFTP] Echec de validation du bloc " << block << "." << endl;
            file.close();
            closesocket(sock);
            return;
        }

        if (bytesRead < BLOCK_SIZE) break;
        block++;
    }

    file.close();
    closesocket(sock);
    cout << "[DEBUG] [Session Upload TFTP] Upload du fichier termine avec succes !" << endl;
}

bool M_TFTP::recevoirFichierPousse(const string& fichierLocal) {
    SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == INVALID_SOCKET) {
        cerr << "[DEBUG] [Session Reception TFTP] Erreur lors de la creation de la socket." << endl;
        return false;
    }

    sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_port = htons(69);
    server.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, (sockaddr*)&server, sizeof(server)) < 0) {
        cerr << "[DEBUG] [Session Reception TFTP] Erreur d'association (bind) sur le port 69." << endl;
        closesocket(sock);
        return false;
    }

    cout << "[DEBUG] [Session Reception TFTP] Serveur TFTP lance et a l'ecoute sur le port 69..." << endl;

    char buf[516];
    sockaddr_in client{};
    int clientLen = sizeof(client);

    int n = recvfrom(sock, buf, sizeof(buf), 0, (sockaddr*)&client, &clientLen);
    if (n < 4 || buf[1] != 2) {
        cout << "[DEBUG] [Session Reception TFTP] Requete WRQ invalide ou corrompue." << endl;
        closesocket(sock);
        return false;
    }

    ofstream fichier(fichierLocal, ios::binary);
    if (!fichier) {
        cout << "[DEBUG] [Session Reception TFTP] Impossible de creer le fichier local : " << fichierLocal << endl;
        closesocket(sock);
        return false;
    }

    cout << "[DEBUG] [Session Reception TFTP] Reception en cours vers : " << fichierLocal << endl;

    char ack[4] = {0, 4, 0, 0};
    sendto(sock, ack, 4, 0, (sockaddr*)&client, clientLen);

    uint16_t expectedBlock = 1;

    while (true) {
        n = recvfrom(sock, buf, sizeof(buf), 0, (sockaddr*)&client, &clientLen);
        if (n < 4) continue;

        uint16_t opcode = ((unsigned char)buf[0] << 8) | (unsigned char)buf[1];
        uint16_t block  = ((unsigned char)buf[2] << 8) | (unsigned char)buf[3];

        if (opcode == 3 && block == expectedBlock) {
            fichier.write(buf + 4, n - 4);

            ack[2] = buf[2];
            ack[3] = buf[3];
            sendto(sock, ack, 4, 0, (sockaddr*)&client, clientLen);

            if (n < 516) break;
            expectedBlock++;
        }
    }

    fichier.close();
    closesocket(sock);
    cout << "[DEBUG] [Session Reception TFTP] Fichier recu et enregistre avec succes !" << endl;
    return true;
}

bool M_TFTP::recevoirFichierPousseMaster(const string& fichierLocal) {
    SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == INVALID_SOCKET) {
        cerr << "[DEBUG] [Session Master TFTP] Erreur lors de la creation de la socket." << endl;
        return false;
    }

    sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_port = htons(69);
    server.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, (sockaddr*)&server, sizeof(server)) < 0) {
        cerr << "[DEBUG] [Session Master TFTP] Erreur d'association (bind) sur le port 69." << endl;
        closesocket(sock);
        return false;
    }

    DWORD timeout = 3000; // 3 secondes
    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout)) == SOCKET_ERROR) {
        cerr << "[DEBUG] [Session Master TFTP] Erreur lors de la configuration du timeout initial." << endl;
        closesocket(sock);
        return false;
    }

    cout << "[DEBUG] [Session Master TFTP] Serveur TFTP lance sur le port 69 (Timeout actif a l'ecoute)..." << endl;

    char buf[516];
    sockaddr_in client{};
    int clientLen = sizeof(client);

    int n = recvfrom(sock, buf, sizeof(buf), 0, (sockaddr*)&client, &clientLen);

    if (n == SOCKET_ERROR) {
        int err = WSAGetLastError();
        if (err == WSAETIMEDOUT) {
            cerr << "[DEBUG] [Session Master TFTP] Erreur : Aucune requete WRQ recue. Le lecteur n'a pas repondu." << endl;
        } else {
            cerr << "[DEBUG] [Session Master TFTP] Erreur de reception WRQ (Code WSAGetLastError : " << err << ")." << endl;
        }
        closesocket(sock);
        return false;
    }

    if (n < 4 || buf[1] != 2) {
        cout << "[DEBUG] [Session Master TFTP] Requete WRQ invalide." << endl;
        closesocket(sock);
        return false;
    }

    ofstream fichier(fichierLocal, ios::binary);
    if (!fichier) {
        cout << "[DEBUG] [Session Master TFTP] Impossible de creer le fichier local : " << fichierLocal << endl;
        closesocket(sock);
        return false;
    }

    cout << "[DEBUG] [Session Master TFTP] Reception du flux en cours vers : " << fichierLocal << endl;

    char ack[4] = {0, 4, 0, 0};
    sendto(sock, ack, 4, 0, (sockaddr*)&client, clientLen);

    uint16_t expectedBlock = 1;

    while (true) {
        n = recvfrom(sock, buf, sizeof(buf), 0, (sockaddr*)&client, &clientLen);

        if (n == SOCKET_ERROR) {
            int err = WSAGetLastError();
            if (err == WSAETIMEDOUT) {
                cerr << "[DEBUG] [Session Master TFTP] Erreur : Temps d'attente depasse pendant le transfert. Abandon." << endl;
            } else {
                cerr << "[DEBUG] [Session Master TFTP] Erreur de reception de bloc (Code WSAGetLastError : " << err << ")." << endl;
            }
            fichier.close();
            closesocket(sock);
            return false;
        }

        if (n < 4) continue;

        uint16_t opcode = ((unsigned char)buf[0] << 8) | (unsigned char)buf[1];
        uint16_t block  = ((unsigned char)buf[2] << 8) | (unsigned char)buf[3];

        if (opcode == 3 && block == expectedBlock) {
            fichier.write(buf + 4, n - 4);

            ack[2] = buf[2];
            ack[3] = buf[3];
            sendto(sock, ack, 4, 0, (sockaddr*)&client, clientLen);

            if (n < 516) break;
            expectedBlock++;
        }
    }

    fichier.close();
    closesocket(sock);
    cout << "[DEBUG] [Session Master TFTP] Fichier Master recu et valide avec succes !" << endl;
    return true;
}

/**
 * @brief Lance un serveur TFTP permanent sur le port 69 capable de recevoir plusieurs fichiers simultanément.
 * @param dossierCible Le dossier local où seront enregistrés tous les fichiers JSON reçus.
 */
void M_TFTP::demarrerServeurMultiThread(const string& dossierCible) {
    SOCKET sockEcoute = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockEcoute == INVALID_SOCKET) {
        cerr << "[SERVER] Erreur de creation de la socket d'ecoute." << endl;
        return;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(69); // Port TFTP standard
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockEcoute, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        cerr << "[SERVER] Erreur de bind sur le port 69." << endl;
        closesocket(sockEcoute);
        return;
    }

    cout << "[SERVER] Serveur TFTP Multi-Thread actif sur le port 69..." << endl;

    char buf[516];
    while (true) {
        sockaddr_in clientAddr{};
        int clientLen = sizeof(clientAddr);

        // On attend une requete (principalement des requetes WRQ de paquets multicast)
        int n = recvfrom(sockEcoute, buf, sizeof(buf), 0, (sockaddr*)&clientAddr, &clientLen);
        if (n == SOCKET_ERROR) continue;

        // Verification de l'Opcode : 2 correspond a une requete WRQ (Write Request)
        uint16_t opcode = ((unsigned char)buf[0] << 8) | (unsigned char)buf[1];
        if (opcode == 2) {
            // Extraction securisee du nom de fichier inclus dans le paquet TFTP
            string nomFichier(buf + 2);

            // On lance un thread detaché pour traiter ce client de maniere autonome
            // Le port 69 est ainsi immediatement libere pour l'appareil suivant
            thread(&M_TFTP::recevoirFichierThread, this, clientAddr, nomFichier, dossierCible).detach();
        }
    }

    closesocket(sockEcoute);
}

/**
 * @brief Tâche d'arrière-plan (Thread) dédiée à la réception des blocs d'un client unique sur un port éphémère.
 */
void M_TFTP::recevoirFichierThread(sockaddr_in clientAddr, string nomFichier, string dossierCible) {
    namespace fs = std::filesystem;

    // 1. Récupération de l'IP du client
    char ipStr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(clientAddr.sin_addr), ipStr, INET_ADDRSTRLEN);

    // CORRECTION : On extrait uniquement le nom du fichier (ex: "config_lecteur.json")
    // en éliminant les dossiers parents (ex: "json\") envoyés par le client
    string nomFichierPur = fs::path(nomFichier).filename().string();

    // Construction du chemin propre
    string cheminComplet = dossierCible + "/" + string(ipStr) + "_" + nomFichierPur;
    cout << "[THREAD] Nouvelle session WRQ depuis " << ipStr << ". Fichier cible : " << cheminComplet << endl;

    // 2. Création d'une socket dédiée a ce client
    SOCKET sockSocketClient = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockSocketClient == INVALID_SOCKET) return;

    // Configuration d'un Timeout sur cette socket (3 secondes)
    DWORD timeout = 3000;
    setsockopt(sockSocketClient, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));

    // Liaison à un port aléatoire libre
    sockaddr_in localAddr{};
    localAddr.sin_family = AF_INET;
    localAddr.sin_port = htons(0);
    localAddr.sin_addr.s_addr = INADDR_ANY;
    bind(sockSocketClient, (sockaddr*)&localAddr, sizeof(localAddr));

    // 3. Ouverture du fichier local (Maintenant le chemin sera valide !)
    ofstream fichier(cheminComplet, ios::binary);
    if (!fichier) {
        cerr << "[THREAD] Impossible de creer le fichier : " << cheminComplet << endl;
        closesocket(sockSocketClient);
        return;
    }

    // 4. Envoi de l'ACK(0) depuis notre NOUVEAU port pour initier le transfert alternatif
    char ack[4] = {0, 4, 0, 0};
    sendto(sockSocketClient, ack, 4, 0, (sockaddr*)&clientAddr, sizeof(clientAddr));

    char buf[516];
    uint16_t expectedBlock = 1;
    int clientLen = sizeof(clientAddr);

    // 5. Boucle de reception des paquets de donnees (DATA)
    while (true) {
        int n = recvfrom(sockSocketClient, buf, sizeof(buf), 0, (sockaddr*)&clientAddr, &clientLen);

        if (n == SOCKET_ERROR) {
            cerr << "[THREAD] Timeout ou erreur de reception pour " << ipStr << ". Abandon." << endl;
            break;
        }

        if (n < 4) continue;

        uint16_t opcode = ((unsigned char)buf[0] << 8) | (unsigned char)buf[1];
        uint16_t block  = ((unsigned char)buf[2] << 8) | (unsigned char)buf[3];

        if (opcode == 3 && block == expectedBlock) {
            // Ecriture des donnees utiles (on retire les 4 octets d'en-tete TFTP)
            fichier.write(buf + 4, n - 4);

            // Preparation et envoi de l'ACK correspondant au bloc recu
            ack[2] = buf[2];
            ack[3] = buf[3];
            sendto(sockSocketClient, ack, 4, 0, (sockaddr*)&clientAddr, clientLen);

            // TFTP specifie que si un bloc fait moins de 516 octets (entete comprise), c'est la fin du fichier
            if (n < 516) {
                cout << "[THREAD] Fichier recu avec succes depuis " << ipStr << " !" << endl;
                break;
            }
            expectedBlock++;
        }
    }

    fichier.close();
    closesocket(sockSocketClient);
}

#endif