#pragma once

// --- PROTECTIONS WINSOCK / WINDOWS ---
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#define NOGDI
#define NOUSER

#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <vector>

// On lie la bibliothèque Winsock (pour éviter les erreurs de "LNK" plus tard)
#pragma comment(lib, "ws2_32.lib")

#define BLOCK_SIZE 512

using namespace std;

struct TransferInfo {
    string ip;
    string path;
};

enum OPCODE { WRQ = 2, DATA = 3, ACK = 4 };

enum class TransferStatus {
    SUCCESS,
    LOCAL_FILE_NOT_FOUND,
    TRANSFER_SOCKET_ERROR,
    NO_INITIAL_ACK,
    DATA_ACK_TIMEOUT,
    INVALID_ACK
};

class M_ServeurTFTP_W {
public:
    M_ServeurTFTP_W(const vector<TransferInfo>& transfers, const string& docRoot);
    ~M_ServeurTFTP_W();

    void runAllTransfers();

private:
    string documentRoot;
    vector<TransferInfo> transferData;

    void initWinsock();
    TransferStatus sendTftpTransfer(const string& ip, const string& filePath);
};