#include <easylogging++.h>
#include <boost/filesystem/operations.hpp>
#include <ConnectionManager.h>
#include <SerializationHelper.h>
#include <FileInfo.h>
#include <Peer.h>

INITIALIZE_EASYLOGGINGPP

int main() {
    ConnectionManager::getInstance("127.0.0.1", 2049).listenLoop();

    std::shared_ptr<Connection> conn = ConnectionManager::getInstance().requestConnection(TRACKER_BIND_IP, TRACKER_BIND_PORT);
    char header = PROTOCOL_HEADER_REGISTER;
    FileInfo fi("nazwa", "hash", std::vector<bool>{1,1,1,0,0,0});
    Peer p("127.0.0.1", std::vector<FileInfo>{fi});
    LOG(INFO) << "Sending header: " + std::to_string(header);
    std::string serialized = SerializationHelper::serialize<Peer>(header, p);
    conn.get()->send(serialized);
    std::cout << conn.get()->receive();

    conn = ConnectionManager::getInstance().requestConnection(TRACKER_BIND_IP, TRACKER_BIND_PORT);
    header = PROTOCOL_HEADER_PEERS_WITH_FILE;
    LOG(INFO) << "Sending header: " + std::to_string(header);
    std::string hash = "hash";
    serialized = SerializationHelper::serialize<std::string>(header, hash);
    conn.get()->send(serialized);
    std::cout << conn.get()->receive();

    conn = ConnectionManager::getInstance().requestConnection(TRACKER_BIND_IP, TRACKER_BIND_PORT);
    header = PROTOCOL_HEADER_LIST_FILES;
    LOG(INFO) << "Sending header: " + std::to_string(header);
    serialized = SerializationHelper::serialize<char>(header);
    conn.get()->send(serialized);
    std::cout << conn.get()->receive();

    while(1);
    return 0;
}