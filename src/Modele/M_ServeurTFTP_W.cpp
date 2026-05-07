#include "Modele/M_ServeurTFTP_W.h"
#include <iostream>
#include <fstream>
#include <future>
#include <filesystem>

M_ServeurTFTP_W::M_ServeurTFTP_W(const vector<TransferInfo>& transfers, const string& docRoot)
    : transferData(transfers), documentRoot(docRoot) {
    initWinsock();
}

M_ServeurTFTP_W::~M_ServeurTFTP_W() {
    WSACleanup();
}

void M_ServeurTFTP_W::initWinsock() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
}

void M_ServeurTFTP_W::runAllTransfers() {
    vector<future<TransferStatus>> futures;
    for (const auto& entry : transferData) {
        string fullPath = documentRoot + "/" + entry.path;
        futures.push_back(async(launch::async, &M_ServeurTFTP_W::sendTftpTransfer, this, entry.ip, fullPath));
    }

    for (size_t i = 0; i < futures.size(); ++i) {
        TransferStatus s = futures[i].get();
        cout << "[TFTP] " << transferData[i].path << " -> " << (s == TransferStatus::SUCCESS ? "OK" : "ERREUR") << endl;
    }
}

TransferStatus M_ServeurTFTP_W::sendTftpTransfer(const string& ip, const string& filePath) {
    ifstream file(filePath, ios::binary);
    if (!file) return TransferStatus::LOCAL_FILE_NOT_FOUND;

    SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    // TIMEOUT : Si le client ne répond pas en 1 seconde, on considère une erreur
    DWORD timeout = 1000;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));

    sockaddr_in destAddr{};
    destAddr.sin_family = AF_INET;
    destAddr.sin_port = htons(69);
    inet_pton(AF_INET, ip.c_str(), &destAddr.sin_addr);

    // 1. Envoi WRQ
    string filename = filesystem::path(filePath).filename().string();
    vector<char> packet;
    uint16_t opWrq = htons(WRQ);
    packet.insert(packet.end(), (char*)&opWrq, (char*)&opWrq + 2);
    packet.insert(packet.end(), filename.begin(), filename.end());
    packet.push_back(0);
    string mode = "octet";
    packet.insert(packet.end(), mode.begin(), mode.end());
    packet.push_back(0);

    sendto(sock, packet.data(), (int)packet.size(), 0, (sockaddr*)&destAddr, sizeof(destAddr));

    // 2. Attente ACK 0
    char ackBuf[4];
    int addrLen = sizeof(destAddr);
    if (recvfrom(sock, ackBuf, 4, 0, (sockaddr*)&destAddr, &addrLen) <= 0) {
        closesocket(sock);
        return TransferStatus::TIMEOUT_ERROR;
    }

    // 3. Boucle DATA / ACK
    uint16_t blockNum = 1;
    vector<char> dataBuf(4 + BLOCK_SIZE);
    while (true) {
        file.read(dataBuf.data() + 4, BLOCK_SIZE);
        size_t bytesRead = file.gcount();

        *reinterpret_cast<uint16_t*>(dataBuf.data()) = htons(DATA);
        *reinterpret_cast<uint16_t*>(dataBuf.data() + 2) = htons(blockNum);

        // Envoi et attente de l'ACK spécifique à ce bloc
        bool ackReceived = false;
        for(int retry = 0; retry < 3 && !ackReceived; retry++) {
            sendto(sock, dataBuf.data(), (int)bytesRead + 4, 0, (sockaddr*)&destAddr, sizeof(destAddr));

            if (recvfrom(sock, ackBuf, 4, 0, (sockaddr*)&destAddr, &addrLen) > 0) {
                uint16_t receivedBlock = ntohs(*reinterpret_cast<uint16_t*>(ackBuf + 2));
                if (receivedBlock == blockNum) ackReceived = true;
            }
        }

        if (!ackReceived) {
            closesocket(sock);
            return TransferStatus::TIMEOUT_ERROR;
        }

        if (bytesRead < BLOCK_SIZE) break;
        blockNum++;
    }

    file.close();
    closesocket(sock);
    return TransferStatus::SUCCESS;
}