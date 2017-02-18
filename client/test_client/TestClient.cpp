#include <easylogging++.h>
#include <boost/filesystem/operations.hpp>
#include <ConnectionManager.h>

INITIALIZE_EASYLOGGINGPP

int main() {
    ConnectionManager::getInstance("127.0.0.1", 2049).listenLoop();
    std::shared_ptr<Connection> conn = ConnectionManager::getInstance().requestConnection(TRACKER_BIND_IP, TRACKER_BIND_PORT);
    conn.get()->send("1");
    std::cout << conn.get()->receive();

    while(1);
    return 0;
}