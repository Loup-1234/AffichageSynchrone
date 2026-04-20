#include <winsock2.h>
#include <ws2tcpip.h>

#include "ServeurTFTP_W.h"

#include <iostream>
#include <fstream>
#include <future>
#include <filesystem>
#include <stdexcept>

ServeurTFTP_W::ServeurTFTP_W(const string& jsonPath, const string& docRoot)
    : configPath(jsonPath), documentRoot(docRoot) {
    initWinsock();
    loadConfig();
}

ServeurTFTP_W::~ServeurTFTP_W() {
    WSACleanup();
}

void ServeurTFTP_W::runAllTransfers() {
    vector<future<TransferStatus>> futures;
    vector<json> entries;

    for (const auto& entry : transferData) {
        entries.push_back(entry);
        auto filename = entry["path"].get<string>();
        string fullPath = documentRoot + "/" + filename;
        futures.push_back(async(launch::async, &ServeurTFTP_W::sendTftpTransfer, this, entry["ip"].get<string>(), fullPath));
    }

    cout << "--- RAPPORT DES TRANSFERTS ---\n";
    int successCount = 0;
    for (size_t i = 0; i < futures.size(); ++i) {
        const TransferStatus status = futures[i].get();
        const auto& entry = entries[i];
        cout << "Fichier: " << entry["path"].get<string>() << " -> IP: " << entry["ip"].get<string>() << " ... ";

        switch (status) {
            case TransferStatus::SUCCESS:
                cout << "SUCCES\n";
                successCount++;
                break;
            case TransferStatus::LOCAL_FILE_NOT_FOUND:
                cout << "ECHEC (Fichier local non trouve)\n";
                break;
            case TransferStatus::NO_INITIAL_ACK:
                cout << "ECHEC (Timeout: le serveur n'a pas repondu)\n";
                break;
            case TransferStatus::DATA_ACK_TIMEOUT:
                cout << "ECHEC (Timeout: l'ACK pour un bloc de donnees n'a pas ete recu)\n";
                break;
            case TransferStatus::INVALID_ACK:
                cout << "ECHEC (Le serveur a envoye une reponse invalide)\n";
                break;
            case TransferStatus::TRANSFER_SOCKET_ERROR:
                cout << "ECHEC (Erreur de socket)\n";
                break;
        }
    }
    cout << "\n--- RESUME ---\n";
    cout << successCount << " sur " << futures.size() << " transferts reussis.\n";
}

void ServeurTFTP_W::initWinsock() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        throw runtime_error("Echec de l'initialisation de Winsock");
    }
}

void ServeurTFTP_W::loadConfig() {
    ifstream f(configPath);
    if (!f) {
        throw runtime_error("Impossible de trouver '" + configPath + "'. Assurez-vous qu'il est a la racine du projet.");
    }
    transferData = json::parse(f);
}

TransferStatus ServeurTFTP_W::sendTftpTransfer(const string& ip, const string& filePath) {
    ifstream file(filePath, ios::binary);
    if (!file) {
        return TransferStatus::LOCAL_FILE_NOT_FOUND;
    }

    SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == INVALID_SOCKET) {
        return TransferStatus::TRANSFER_SOCKET_ERROR;
    }

    DWORD timeout = 10000; // 10,000 millisecondes
    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout)) == SOCKET_ERROR) {
        closesocket(sock);
        return TransferStatus::TRANSFER_SOCKET_ERROR;
    }

    sockaddr_in destAddr{};
    destAddr.sin_family = AF_INET;
    destAddr.sin_port = htons(69); // Port TFTP standard
    inet_pton(AF_INET, ip.c_str(), &destAddr.sin_addr);

    string filename = filesystem::path(filePath).filename().string();
    string mode = "octet";

    vector<char> wrqPacket;
    uint16_t wrqOpcode = htons(WRQ);
    wrqPacket.insert(wrqPacket.end(), reinterpret_cast<char *>(&wrqOpcode), reinterpret_cast<char *>(&wrqOpcode) + 2);
    wrqPacket.insert(wrqPacket.end(), filename.begin(), filename.end());
    wrqPacket.push_back(0); // Séparateur nul
    wrqPacket.insert(wrqPacket.end(), mode.begin(), mode.end());
    wrqPacket.push_back(0); // Séparateur nul

    sendto(sock, wrqPacket.data(), wrqPacket.size(), 0, reinterpret_cast<sockaddr *>(&destAddr), sizeof(destAddr));

    char ackBuffer[4];
    int serverLen = sizeof(destAddr);
    int recvSize = recvfrom(sock, ackBuffer, sizeof(ackBuffer), 0, reinterpret_cast<sockaddr *>(&destAddr), &serverLen);

    if (recvSize == SOCKET_ERROR) {
        closesocket(sock);
        return (WSAGetLastError() == WSAETIMEDOUT) ? TransferStatus::NO_INITIAL_ACK : TransferStatus::TRANSFER_SOCKET_ERROR;
    }
    if (recvSize < 4 || ntohs(*reinterpret_cast<uint16_t*>(ackBuffer)) != ACK || ntohs(*reinterpret_cast<uint16_t*>(ackBuffer + 2)) != 0) {
        closesocket(sock);
        return TransferStatus::INVALID_ACK;
    }

    uint16_t blockNumber = 1;
    vector<char> dataPacket(4 + BLOCK_SIZE);

    while (true) {
        file.read(dataPacket.data() + 4, BLOCK_SIZE);
        size_t bytesRead = file.gcount();

        *reinterpret_cast<uint16_t*>(dataPacket.data()) = htons(DATA);
        *reinterpret_cast<uint16_t*>(dataPacket.data() + 2) = htons(blockNumber);

        sendto(sock, dataPacket.data(), bytesRead + 4, 0, reinterpret_cast<sockaddr *>(&destAddr), sizeof(destAddr));

        recvSize = recvfrom(sock, ackBuffer, sizeof(ackBuffer), 0, reinterpret_cast<sockaddr *>(&destAddr), &serverLen);
        if (recvSize == SOCKET_ERROR) {
            closesocket(sock);
            return (WSAGetLastError() == WSAETIMEDOUT) ? TransferStatus::DATA_ACK_TIMEOUT : TransferStatus::TRANSFER_SOCKET_ERROR;
        }

        uint16_t ackBlock = ntohs(*reinterpret_cast<uint16_t*>(ackBuffer + 2));
        if (ntohs(*reinterpret_cast<uint16_t*>(ackBuffer)) != ACK || ackBlock != blockNumber) {
            closesocket(sock);
            return TransferStatus::INVALID_ACK;
        }

        if (bytesRead < BLOCK_SIZE) break;
        blockNumber++;
    }

    file.close();
    closesocket(sock);
    return TransferStatus::SUCCESS;
}