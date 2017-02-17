#ifndef FILESHARER_CONNECTIONMANAGER_H
#define FILESHARER_CONNECTIONMANAGER_H


#include <netinet/in.h>
#include <string>
#include <mutex>
#include <thread>
#include <unordered_set>
#include "Connection.h"

class ConnectionManager {
    public:
        static ConnectionManager &getInstance();

        sockaddr_in getOwnSocket();
        std::string getOwnIP();
        uint16_t getOwnPort();
        int getOwnSocketDescriptor();
        std::unordered_set<std::shared_ptr<Connection>> getActiveConnections();
        std::shared_ptr<Connection> requestConnection(std::string peerIP, uint16_t peerPort);
        void listenLoop();

    private:
        ConnectionManager();
        ConnectionManager(const ConnectionManager &) = delete;
        ConnectionManager& operator=(const ConnectionManager&) = delete;
        virtual ~ConnectionManager();

        sockaddr_in ownSocket;
        int ownSocketDescriptor;
        std::unordered_set<std::shared_ptr<Connection>> connections;
        std::mutex connectionsMutex;
};


#endif //FILESHARER_CONNECTIONMANAGER_H
