#include <thread>
#include <string>
#include <atomic>
#include "QueuedPacket.h"
#include "Operators.h"

static std::atomic_bool flag = false;

static void operatorThreadEntry(std::string cameraId) {
    auto operators = new Operators();
    operators->init();

    while (flag) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void startOperators(std::string_view cameraId) {
    flag = true;

    std::thread thread(operatorThreadEntry, std::string(cameraId));
    thread.detach();
}

void stopOperators() {
    
}