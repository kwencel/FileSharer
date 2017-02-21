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
        /**
         * Constructs the Singleton ConnectionManager instance. Both parameters are optional
         * for convenient usage. The first call to this function need the parameters filled however.
         * @param bindIP Placeholder value for making parameter optional
         * @param bindPort Placeholder value for making parameter optional
         * @return One and the only instance of ConnectionManager
         */
        static ConnectionManager &getInstance(std::string bindIP  = "", uint16_t bindPort = 0);

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
        void addFileHandler(std::shared_ptr<FileHandler> newFileHandler);
        void setFileHandlers(std::vector<std::shared_ptr<FileHandler>> fileHandlers);
        const std::vector<std::shared_ptr<FileHandler>>& getFileHandlers() const;
        void setTrackerDetails(std::string trackerIP, uint16_t trackerPort);
        const std::string getTrackerIP() const;
        uint16_t getTrackerPort() const;

    private:
        ConnectionManager(std::string bindIP, uint16_t bindPort);
        virtual ~ConnectionManager();

        sockaddr_in ownSocket;
        int ownSocketDescriptor;
        std::unordered_set<std::shared_ptr<Connection>> connections;
        std::mutex connectionsMutex;
        std::vector<std::shared_ptr<FileHandler>> fileHandlers;
        std::mutex fileHandlersMutex;

    private:
        uint16_t trackerPort;
        std::string trackerIP;

        ConnectionManager() = delete;
        ConnectionManager(const ConnectionManager &) = delete;
        int epollDescriptor;

        bool notifyFileAboutNewConnection(std::shared_ptr<Connection> connection);
};


#endif //FILESHARER_CONNECTIONMANAGER_H
