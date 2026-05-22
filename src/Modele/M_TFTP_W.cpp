#include "modele/M_TFTP_W.h"

M_TFTP_W::M_TFTP_W() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
}

M_TFTP_W::~M_TFTP_W() {
    WSACleanup();
}

// --- Méthode d'envoi (Silencieuse sur les blocs) ---
void M_TFTP_W::envoyer(string ipMaster, string cheminJson) {
    SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == INVALID_SOCKET) {
        cerr << "Erreur création socket" << endl;
        return;
    }

    sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_port = htons(TFTP_PORT);
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
    cout << "WRQ envoyé pour : " << cheminJson << " vers " << ipMaster << endl;

    DWORD tv = 3000;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));

    char buf[516];
    sockaddr_in peer{};
    int peerLen = sizeof(peer);
    int n = recvfrom(sock, buf, sizeof(buf), 0, (sockaddr*)&peer, &peerLen);

    if (n < 4 || buf[1] != 4 || buf[2] != 0 || buf[3] != 0) {
        cerr << "Erreur : ACK(0) non reçu" << endl;
        closesocket(sock);
        return;
    }
    cout << "ACK(0) reçu, début du transfert..." << endl;

    ifstream file(cheminJson, ios::binary);
    if (!file) {
        cerr << "Impossible d'ouvrir : " << cheminJson << endl;
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
            cout << "Timeout, tentative " << tries << "..." << endl;
        }

        if (!acked) {
            cerr << "Échec bloc " << block << endl;
            file.close();
            closesocket(sock);
            return;
        }

        // L'affichage du bloc envoyé a été supprimé d'ici pour nettoyer la console
        if (bytesRead < BLOCK_SIZE) break;
        block++;
    }

    file.close();
    closesocket(sock);
    cout << "Transfert terminé !" << endl;
}

// --- Méthode de Réception (Silencieuse sur les blocs) ---
bool M_TFTP_W::recevoirFichierPousse(int port, const string& fichierLocal) {
    SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == INVALID_SOCKET) {
        cerr << "Erreur création socket" << endl;
        return false;
    }

    sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, (sockaddr*)&server, sizeof(server)) < 0) {
        cerr << "Erreur de bind" << endl;
        closesocket(sock);
        return false;
    }

    cout << "Serveur TFTP lancé sur le port " << port << "..." << endl;

    char buf[516];
    sockaddr_in client{};
    int clientLen = sizeof(client);

    int n = recvfrom(sock, buf, sizeof(buf), 0, (sockaddr*)&client, &clientLen);
    if (n < 4 || buf[1] != 2) {
        cout << "WRQ invalide" << endl;
        closesocket(sock);
        return false;
    }

    ofstream fichier(fichierLocal, ios::binary);
    if (!fichier) {
        cout << "Impossible de créer le fichier local : " << fichierLocal << endl;
        closesocket(sock);
        return false;
    }

    cout << "Réception en cours de la vidéo vers : " << fichierLocal << endl;

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

            // L'affichage du bloc reçu a été supprimé d'ici pour garder la console propre

            if (n < 516) break;
            expectedBlock++;
        }
    }

    fichier.close();
    closesocket(sock);
    cout << "Fichier vidéo reçu avec succès !" << endl;
    return true;
}