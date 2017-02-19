#ifndef FILESHARER_CONNECTIONMANAGER_H
#define FILESHARER_CONNECTIONMANAGER_H

#include <netinet/in.h>
#include <string>
#include <mutex>
#include <thread>
#include <unordered_set>
#include <sys/epoll.h>
#include <File.h>
#include "Connection.h"

class ConnectionManager {
    public:
        static ConnectionManager &getInstance(std::string bindIP  = CLIENT_BIND_IP, uint16_t bindPort = CLIENT_BIND_PORT);

        sockaddr_in getOwnSocket();
        std::string getOwnIP();
        uint16_t getOwnPort();
        int getOwnSocketDescriptor();
        std::unordered_set<std::shared_ptr<Connection>> getActiveConnections();
        std::shared_ptr<Connection> requestConnection(std::string peerIP, uint16_t peerPort);
        void removeConnection(Connection *connection);

        /**
         * Adds new connections to @a connections set.
         */
        void listenLoop();
        void processIncomingConnections();

        std::vector<FileHandler> fileHandlers;

    private:
        ConnectionManager(std::string bindIP, uint16_t bindPort);
        virtual ~ConnectionManager();

        sockaddr_in ownSocket;
        int ownSocketDescriptor;
        std::unordered_set<std::shared_ptr<Connection>> connections;
        std::mutex connectionsMutex;

        ConnectionManager() = delete;
        ConnectionManager(const ConnectionManager &) = delete;
//        ConnectionManager& operator=(const ConnectionManager&) = delete;
        int epollDescriptor;

        bool notifyFileAboutNewConnection(std::shared_ptr<Connection> connection);
};


#endif //FILESHARER_CONNECTIONMANAGER_H
