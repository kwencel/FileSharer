#include <easylogging++.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <Define.h>
#include "TrackerProtocolTranslator.h"
#include "ErrorCheckUtils.h"
#include <boost/serialization/vector.hpp>
#include <ConnectionManager.h>
#include <Connection.h>

INITIALIZE_EASYLOGGINGPP

int main() {
    TrackerProtocolTranslator protocolTranslator;
    ConnectionManager &cm = ConnectionManager::getInstance(TRACKER_BIND_IP, TRACKER_BIND_PORT);
    cm.listenLoop();

    while (1) {
        for (auto &&connection : cm.getActiveConnections()) {
            std::string request = connection.get()->read(9);
            char header = *(char *)(request.c_str());
            LOG(DEBUG) << "Header: " + std::to_string(header);
            uint64_t objectSize = *(uint64_t *)(request.c_str() + 1);
            LOG(DEBUG) << "Object size: " + std::to_string(objectSize);

            std::string message = connection.get()->read((size_t) objectSize); //TODO change type in read to uint64_t or cast to size_t here
            LOG(DEBUG) << "Object size: " + std::to_string(objectSize);
            std::string response = protocolTranslator.generateResponse(header, message);
            connection.get()->write(response);
            //cm.removeConnection(connection.get()); //TODO check if works (double free error if uncommented)
        }
    }

    return 0;
}