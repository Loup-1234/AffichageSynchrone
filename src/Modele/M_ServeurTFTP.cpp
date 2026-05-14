#include "../../include/Modele/M_ServeurTFTP.h"
#include <iostream>
#include <fstream>
#include <future>
#include <filesystem>

M_ServeurTFTP::M_ServeurTFTP(const std::vector<TransferInfo> &transfers, const std::string &docRoot)
    : documentRoot(docRoot), transferData(transfers) {
    initNetwork();
}

M_ServeurTFTP::~M_ServeurTFTP() {
#ifdef _WIN32
    WSACleanup();
#endif
}

void M_ServeurTFTP::initNetwork() {
#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
}

void M_ServeurTFTP::runAllTransfers() {
    std::vector<std::future<TransferStatus> > futures;

    // Lancement asynchrone de chaque transfert dans un thread séparé
    for (const auto &entry: transferData) {
        std::string fullPath = documentRoot + "/" + entry.path;
        futures.push_back(std::async(std::launch::async, &M_ServeurTFTP::sendTftpTransfer, this, entry.ip, fullPath));
    }

    // Attente et affichage des résultats
    for (size_t i = 0; i < futures.size(); ++i) {
        TransferStatus s = futures[i].get();
        std::cout << "[TFTP] " << transferData[i].path << " -> ";
        if (s == TransferStatus::SUCCESS) std::cout << "OK" << std::endl;
        else if (s == TransferStatus::NOT_IMPLEMENTED_ON_OS) std::cout << "SIMULÉ (Linux)" << std::endl;
        else std::cout << "ERREUR" << std::endl;
    }
}

TransferStatus M_ServeurTFTP::sendTftpTransfer(const std::string &ip, const std::string &filePath) {
#ifdef _WIN32
    // --- Implémentation du protocole TFTP pour Windows ---

    std::ifstream file(filePath, std::ios::binary);
    if (!file) return TransferStatus::LOCAL_FILE_NOT_FOUND;

    SocketType sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == INVALID_SOCKET) return TransferStatus::TRANSFER_SOCKET_ERROR;

    // Configuration d'un timeout de 1s pour éviter de bloquer indéfiniment sur un recvfrom
    DWORD timeout = 1000;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *) &timeout, sizeof(timeout));

    sockaddr_in destAddr{};
    destAddr.sin_family = AF_INET;
    destAddr.sin_port = htons(69); // Port standard TFTP
    inet_pton(AF_INET, ip.c_str(), &destAddr.sin_addr);

    // 1. Envoi de la requête d'écriture (WRQ)
    std::string filename = std::filesystem::path(filePath).filename().string();
    std::vector<char> packet;
    uint16_t opWrq = htons(WRQ);
    packet.insert(packet.end(), (char *) &opWrq, (char *) &opWrq + 2);
    packet.insert(packet.end(), filename.begin(), filename.end());
    packet.push_back(0); // Séparateur null
    std::string mode = "octet"; // Mode binaire
    packet.insert(packet.end(), mode.begin(), mode.end());
    packet.push_back(0);

    sendto(sock, packet.data(), (int) packet.size(), 0, (sockaddr *) &destAddr, sizeof(destAddr));

    // 2. Attente de l'ACK 0 (Confirmation de la requête WRQ)
    char ackBuf[4];
    int addrLen = sizeof(destAddr);
    if (recvfrom(sock, ackBuf, 4, 0, (sockaddr *) &destAddr, &addrLen) <= 0) {
        closesocket(sock);
        return TransferStatus::TIMEOUT_ERROR;
    }

    // 3. Boucle d'envoi des données (DATA) et réception des acquittements (ACK)
    uint16_t blockNum = 1;
    std::vector<char> dataBuf(4 + BLOCK_SIZE);
    while (true) {
        file.read(dataBuf.data() + 4, BLOCK_SIZE);
        size_t bytesRead = file.gcount();

        // Préparation de l'entête DATA [Opcode(2 bytes) | Block#(2 bytes)]
        *reinterpret_cast<uint16_t *>(dataBuf.data()) = htons(DATA);
        *reinterpret_cast<uint16_t *>(dataBuf.data() + 2) = htons(blockNum);

        bool ackReceived = false;
        // Mécanisme de re-tentative (3 essais) en cas de perte de paquet UDP
        for (int retry = 0; retry < 3 && !ackReceived; retry++) {
            sendto(sock, dataBuf.data(), (int) bytesRead + 4, 0, (sockaddr *) &destAddr, sizeof(destAddr));

            if (recvfrom(sock, ackBuf, 4, 0, (sockaddr *) &destAddr, &addrLen) > 0) {
                uint16_t receivedBlock = ntohs(*reinterpret_cast<uint16_t *>(ackBuf + 2));
                if (receivedBlock == blockNum) ackReceived = true;
            }
        }

        if (!ackReceived) {
            closesocket(sock);
            return TransferStatus::TIMEOUT_ERROR;
        }

        // Si on a lu moins que BLOCK_SIZE, c'est le dernier paquet
        if (bytesRead < BLOCK_SIZE) break;
        blockNum++;
    }

    file.close();
    closesocket(sock);
    return TransferStatus::SUCCESS;

#else
    // --- Mode simulation pour les environnements non-Windows ---
    std::cout << "[DEBUG] Simulation du transfert TFTP :" << std::endl;
    std::cout << "Fichier : " << filePath << std::endl;
    std::cout << "Destination IP : " << ip << std::endl;

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    return TransferStatus::NOT_IMPLEMENTED_ON_OS;
#endif
}