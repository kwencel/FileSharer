#include <easylogging++.h>
#include <boost/filesystem/operations.hpp>
#include "include/ConnectionManager.h"
#include <SerializationHelper.h>
#include <Peer.h>

INITIALIZE_EASYLOGGINGPP

int main() {
//    ConnectionManager::getInstance("127.0.0.1", 2049).listenLoop();
//
//    std::shared_ptr<Connection> conn = ConnectionManager::getInstance().requestConnection(TRACKER_BIND_IP, TRACKER_BIND_PORT);
//    char header = PROTOCOL_HEADER_REGISTER;
//    FileInfo fi("nazwa", "hash", std::vector<bool>{1,1,1,0,0,0});
//    Peer p("127.0.0.1", std::vector<FileInfo>{fi});
//    LOG(INFO) << "Sending header: " + std::to_string(header);
//    std::string serialized = SerializationHelper::serialize<Peer>(header, p);
//    conn.get()->write(serialized);
//    std::cout << conn.get()->read();
//
//    conn = ConnectionManager::getInstance().requestConnection(TRACKER_BIND_IP, TRACKER_BIND_PORT);
//    header = PROTOCOL_HEADER_PEERS_WITH_FILE;
//    LOG(INFO) << "Sending header: " + std::to_string(header);
//    std::string hash = "hash";
//    serialized = SerializationHelper::serialize<std::string>(header, hash);
//    conn.get()->write(serialized);
//    std::cout << conn.get()->read();
//
//    conn = ConnectionManager::getInstance().requestConnection(TRACKER_BIND_IP, TRACKER_BIND_PORT);
//    header = PROTOCOL_HEADER_LIST_FILES;
//    LOG(INFO) << "Sending header: " + std::to_string(header);
//    serialized = SerializationHelper::serialize<char>(header);
//    conn.get()->write(serialized);
//    std::cout << conn.get()->read();
//
//    while(1);

//    std::shared_ptr<Connection> conn = std::make_shared<Connection>("127.0.0.1", 2048);
//    FileHandler handler;
//    conn.get()->registerObserver(&handler);
//    conn.get()->notify();
//    conn.re
//    conn.get()
//    conn.registerObserver(std::make_shared<Connection>(conn), )

    ConnectionManager &cm = ConnectionManager::getInstance("127.0.0.1", 2050);
//    cm.listenLoop();
    auto conn = cm.requestConnection(TRACKER_BIND_IP, TRACKER_BIND_PORT);
    uint32_t number = 2534625;

    char header = PROTOCOL_HEADER_REGISTER;
    FileInfo fi("nazwa", "hash", 10, std::vector<bool>{1,1,1,0,0,0});
    Peer p("127.0.0.1", 2500, std::vector<FileInfo>{fi});
    LOG(INFO) << "Sending header: " + std::to_string(header);
    std::string serialized = SerializationHelper::serialize<Peer>(header, p);
    conn.get()->write(serialized);
    std::string request = conn.get()->read(9);
    header = *(char *)(request.c_str());
    LOG(INFO) << "Received header: " + std::to_string(header);
    if (header == PROTOCOL_HEADER_REGISTER_ACCEPT)
        std::cout << "SUCCESS!\n" << std::endl;

    while(1);

    return 0;
}