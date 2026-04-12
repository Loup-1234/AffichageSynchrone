#include "../include/Master/Master.h"

int main() {
    Master monInterface("127.0.0.1", 1234);
    monInterface.executer();
    return 0;
}