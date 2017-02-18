#include <easylogging++.h>
#include <boost/filesystem/operations.hpp>
#include <ConnectionManager.h>

INITIALIZE_EASYLOGGINGPP

int main() {
    ConnectionManager &cm = ConnectionManager::getInstance("192.168.1.103", 2048);
    cm.listenLoop();
//    cm.listenLoop();
//    std::cout << "Press to connect to 2049\n";
//    getchar();
//    std::unordered_set<std::shared_ptr<Connection>> connections = cm.getActiveConnections();
//    for (const auto& conn : connections) {
//        conn.get().
//    }
//    std::cout << conn.get()->receive();

    while(1);
    return 0;
}