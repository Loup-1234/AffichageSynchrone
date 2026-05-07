#include "../../include/Modele/M_ServeurTFTP_W.h"
#include <iostream>
#include <fstream>
#include <future>
#include <filesystem>
#include <chrono> // Pour le délai de régulation

M_ServeurTFTP_W::M_ServeurTFTP_W(const vector<TransferInfo>& transfers, const string& docRoot)
    : transferData(transfers), documentRoot(docRoot) {
    initWinsock();
}

M_ServeurTFTP_W::~M_ServeurTFTP_W() {
    WSACleanup();
}

void M_ServeurTFTP_W::runAllTransfers() {
    vector<future<TransferStatus>> futures;

    for (const auto& entry : transferData) {
        string fullPath = documentRoot + "/" + entry.path;
        futures.push_back(async(launch::async, &M_ServeurTFTP_W::sendTftpTransfer, this, entry.ip, fullPath));
    }

    cout << "--- RAPPORT DES TRANSFERTS (Mode Sans ACK) ---\n";
    int successCount = 0;
    for (size_t i = 0; i < futures.size(); ++i) {
        const TransferStatus status = futures[i].get();
        const auto& entry = transferData[i];
        cout << "Fichier: " << entry.path << " -> IP: " << entry.ip << " ... ";

        if (status == TransferStatus::SUCCESS) {
            cout << "ENVOYE\n";
            successCount++;
        } else {
            cout << "ECHEC\n";
        }
    }
}

void M_ServeurTFTP_W::initWinsock() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
}

TransferStatus M_ServeurTFTP_W::sendTftpTransfer(const string& ip, const string& filePath) {
    ifstream file(filePath, ios::binary);
    if (!file) return TransferStatus::LOCAL_FILE_NOT_FOUND;

    SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == INVALID_SOCKET) return TransferStatus::TRANSFER_SOCKET_ERROR;

    sockaddr_in destAddr{};
    destAddr.sin_family = AF_INET;
    destAddr.sin_port = htons(69);
    inet_pton(AF_INET, ip.c_str(), &destAddr.sin_addr);

    // 1. Préparation du WRQ
    string filename = filesystem::path(filePath).filename().string();
    vector<char> wrqPacket;
    uint16_t wrqOpcode = htons(WRQ);
    wrqPacket.insert(wrqPacket.end(), (char*)&wrqOpcode, (char*)&wrqOpcode + 2);
    wrqPacket.insert(wrqPacket.end(), filename.begin(), filename.end());
    wrqPacket.push_back(0);
    string mode = "octet";
    wrqPacket.insert(wrqPacket.end(), mode.begin(), mode.end());
    wrqPacket.push_back(0);

    // Envoi du WRQ - On n'attend pas de réponse !
    sendto(sock, wrqPacket.data(), (int)wrqPacket.size(), 0, (sockaddr*)&destAddr, sizeof(destAddr));

    // On laisse une milliseconde au client pour réagir
    this_thread::sleep_for(chrono::milliseconds(1));

    uint16_t blockNumber = 1;
    vector<char> dataPacket(4 + BLOCK_SIZE);

    // 2. Boucle d'envoi DATA massive
    while (true) {
        file.read(dataPacket.data() + 4, BLOCK_SIZE);
        size_t bytesRead = file.gcount();

        *reinterpret_cast<uint16_t*>(dataPacket.data()) = htons(DATA);
        *reinterpret_cast<uint16_t*>(dataPacket.data() + 2) = htons(blockNumber);

        // Envoi direct
        int sent = sendto(sock, dataPacket.data(), (int)bytesRead + 4, 0, (sockaddr*)&destAddr, sizeof(destAddr));
        if (sent == SOCKET_ERROR) {
            closesocket(sock);
            return TransferStatus::TRANSFER_SOCKET_ERROR;
        }

        // --- REGULATION DU DEBIT ---
        // Très important en UDP sans ACK pour ne pas saturer le buffer du client
        this_thread::sleep_for(chrono::microseconds(400));

        if (bytesRead < BLOCK_SIZE) break;
        blockNumber++;
    }

    file.close();
    closesocket(sock);
    return TransferStatus::SUCCESS;
}