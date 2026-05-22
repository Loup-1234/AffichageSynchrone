#ifdef _WIN32

#include "Modele/M_TFTP_W.h\"

M_TFTP_W::M_TFTP_W() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
}

M_TFTP_W::~M_TFTP_W() {
    WSACleanup();
}

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

    sendto(sock, wrq.data(), static_cast<int>(wrq.size()), 0, (sockaddr*)&server, sizeof(server));

    char buf[516];
    sockaddr_in from{};
    int fromLen = sizeof(from);

    int n = recvfrom(sock, buf, sizeof(buf), 0, (sockaddr*)&from, &fromLen);
    if (n >= 4 && buf[1] == 4) {
        cout << "[TFTP] WRQ initialisé, envoi du fichier : " << cheminJson << endl;
    } else {
        cout << "[TFTP] Échec de l'initialisation WRQ" << endl;
        closesocket(sock);
        return;
    }

    ifstream fichier(cheminJson, ios::binary);
    if (!fichier) {
        cout << "[TFTP] Fichier introuvable localement" << endl;
        closesocket(sock);
        return;
    }

    uint16_t blockNum = 1;
    while (!fichier.eof()) {
        char dataBuf[516];
        dataBuf[0] = 0; dataBuf[1] = 3;
        dataBuf[2] = (blockNum >> 8) & 0xFF;
        dataBuf[3] = blockNum & 0xFF;

        fichier.read(dataBuf + 4, BLOCK_SIZE);
        streamsize lus = fichier.gcount();

        int essai = 0;
        bool succesAck = false;
        while (essai < MAX_RETRIES && !succesAck) {
            sendto(sock, dataBuf, static_cast<int>(lus + 4), 0, (sockaddr*)&from, fromLen);

            fd_set ensembles;
            FD_ZERO(&ensembles);
            FD_SET(sock, &ensembles);
            timeval tv{2, 0}; // 2 secondes de timeout

            if (select(static_cast<int>(sock) + 1, &ensembles, nullptr, nullptr, &tv) > 0) {
                int nAck = recvfrom(sock, buf, sizeof(buf), 0, (sockaddr*)&from, &fromLen);
                if (nAck >= 4 && buf[1] == 4) {
                    uint16_t ackBlock = ((unsigned char)buf[2] << 8) | (unsigned char)buf[3];
                    if (ackBlock == blockNum) {
                        succesAck = true;
                    }
                }
            }
            if (!succesAck) essai++;
        }

        if (!succesAck) {
            cout << "[TFTP] Échec de transmission du bloc " << blockNum << endl;
            break;
        }
        blockNum++;
    }

    cout << "[TFTP] Transfert réussi de " << cheminJson << endl;
    closesocket(sock);
}

bool M_TFTP_W::recevoirFichierPousse(int port, const string& fichierLocal) {
    SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == INVALID_SOCKET) return false;

    sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
        closesocket(sock);
        return false;
    }

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

            expectedBlock++;

            if (n - 4 < BLOCK_SIZE) {
                cout << "Fichier reçu avec succès !" << endl;
                break;
            }
        }
    }

    fichier.close();
    closesocket(sock);
    return true;
}

#endif // _WIN32