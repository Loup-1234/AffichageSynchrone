#pragma once

#include <string>

#include "../JSON/json.hpp"

#define BLOCK_SIZE 512

using json = nlohmann::json;

using namespace std;

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
    M_ServeurTFTP_W(const string& jsonPath, const string& docRoot);

    ~M_ServeurTFTP_W();

    void runAllTransfers();

private:
    string configPath;
    string documentRoot;
    json transferData;

    void initWinsock();

    void loadConfig();

    TransferStatus sendTftpTransfer(const string& ip, const string& filePath);
};