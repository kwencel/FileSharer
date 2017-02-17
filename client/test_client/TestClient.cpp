#include <easylogging++.h>
#include <boost/filesystem/operations.hpp>
#include <ConnectionManager.h>

INITIALIZE_EASYLOGGINGPP

int main() {
    ConnectionManager::getInstance().listenLoop();
    std::shared_ptr<Connection> conn = ConnectionManager::getInstance().requestConnection("127.0.0.1", 2049);
    conn.get()->send("test");

    while(1);
    return 0;
}