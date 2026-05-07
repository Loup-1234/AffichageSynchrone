#include "Modele/M_ServeurTFTP_W.h"
#include <iostream>
#include <fstream>
#include <future>
#include <filesystem>
#include <chrono>

using namespace std;

M_ServeurTFTP_W::M_ServeurTFTP_W(const vector<TransferInfo>& transfers, const string& docRoot)
    : transferData(transfers), documentRoot(docRoot) {
    initWinsock();
}

M_ServeurTFTP_W::~M_ServeurTFTP_W() {
    WSACleanup();
}

void M_ServeurTFTP_W::initWinsock() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "[ERREUR] Echec de l'initialisation Winsock" << endl;
    }
}

void M_ServeurTFTP_W::runAllTransfers() {
    vector<future<TransferStatus>> futures;

    cout << "\n--- DEMARRAGE DES TRANSFERTS (MODE SANS ACK) ---" << endl;
    cout << "Dossier source : " << documentRoot << endl;

    for (const auto& entry : transferData) {
        string fullPath = documentRoot + "/" + entry.path;
        futures.push_back(async(launch::async, &M_ServeurTFTP_W::sendTftpTransfer, this, entry.ip, fullPath));
    }

    int successCount = 0;
    for (size_t i = 0; i < futures.size(); ++i) {
        const TransferStatus status = futures[i].get();
        const auto& entry = transferData[i];

        cout << ">> Resultat pour " << entry.path << " [" << entry.ip << "] : ";
        if (status == TransferStatus::SUCCESS) {
            cout << "TERMINE AVEC SUCCES" << endl;
            successCount++;
        } else if (status == TransferStatus::LOCAL_FILE_NOT_FOUND) {
            cout << "ERREUR (Fichier introuvable)" << endl;
        } else {
            cout << "ERREUR (Probleme Socket)" << endl;
        }
    }
    cout << "--- RESUME : " << successCount << "/" << futures.size() << " transferts effectues ---\n" << endl;
}

TransferStatus M_ServeurTFTP_W::sendTftpTransfer(const string& ip, const string& filePath) {
    // 1. Verification de l'existence du fichier
    ifstream file(filePath, ios::binary);
    if (!file) {
        return TransferStatus::LOCAL_FILE_NOT_FOUND;
    }

    // 2. Creation de la socket UDP
    SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == INVALID_SOCKET) {
        return TransferStatus::TRANSFER_SOCKET_ERROR;
    }

    // Configuration de la destination (Port 69 TFTP)
    sockaddr_in destAddr{};
    destAddr.sin_family = AF_INET;
    destAddr.sin_port = htons(69);
    inet_pton(AF_INET, ip.c_str(), &destAddr.sin_addr);

    // 3. Envoi de la requête d'écriture (WRQ)
    string filename = filesystem::path(filePath).filename().string();
    vector<char> wrq;
    uint16_t op = htons(WRQ);
    wrq.insert(wrq.end(), (char*)&op, (char*)&op + 2);
    wrq.insert(wrq.end(), filename.begin(), filename.end());
    wrq.push_back(0);
    string mode = "octet";
    wrq.insert(wrq.end(), mode.begin(), mode.end());
    wrq.push_back(0);

    cout << "[SERVEUR] Envoi WRQ [" << filename << "] vers " << ip << "..." << endl;
    sendto(sock, wrq.data(), (int)wrq.size(), 0, (sockaddr*)&destAddr, sizeof(destAddr));

    // Pause de sécurité pour laisser le client ouvrir son descripteur de fichier
    this_thread::sleep_for(chrono::milliseconds(10));

    // 4. Boucle d'envoi des données (DATA)
    uint16_t blockNum = 1;
    vector<char> dataBuffer(4 + BLOCK_SIZE);
    size_t totalBytesSent = 0;

    cout << "[SERVEUR] Debut du streaming des blocs pour " << filename << "..." << endl;

    while (true) {
        file.read(dataBuffer.data() + 4, BLOCK_SIZE);
        size_t bytesRead = file.gcount();

        // Ajout du header TFTP (Opcode DATA + Numero de bloc)
        *reinterpret_cast<uint16_t*>(dataBuffer.data()) = htons(DATA);
        *reinterpret_cast<uint16_t*>(dataBuffer.data() + 2) = htons(blockNum);

        int result = sendto(sock, dataBuffer.data(), (int)bytesRead + 4, 0, (sockaddr*)&destAddr, sizeof(destAddr));

        if (result == SOCKET_ERROR) {
            cerr << "[SERVEUR] Erreur d'envoi au bloc " << blockNum << endl;
            closesocket(sock);
            return TransferStatus::TRANSFER_SOCKET_ERROR;
        }

        totalBytesSent += bytesRead;

        // REGULATION : Indispensable sans ACK pour ne pas saturer le recepteur
        this_thread::sleep_for(chrono::microseconds(500));

        // Affichage de progression tous les 1000 blocs pour ne pas polluer la console
        if (blockNum % 1000 == 0) {
            cout << "[LOG] " << filename << " : " << (totalBytesSent / 1024) << " Ko envoyes..." << endl;
        }

        if (bytesRead < BLOCK_SIZE) break; // Dernier paquet envoyé
        blockNum++;
    }

    cout << "[SERVEUR] Succes : " << filename << " envoye (" << totalBytesSent << " octets, " << blockNum << " blocs)." << endl;

    file.close();
    closesocket(sock);
    return TransferStatus::SUCCESS;
}