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

        /**
         * @return Socket tha ConnectionManager was bind to
         */
        sockaddr_in getOwnSocket();

        /**
         * @return IP that was used to bind ConnectionManager
         */
        std::string getOwnIP();

        /**
         * @return IP thatwas used to bind ConnectionManager
         */
        uint16_t getOwnPort();

        /**
         * @return Descriptor of the socket ConnectionManager was bind to
         */
        int getOwnSocketDescriptor();

        /**
         * Adds new connections to @a connections set.
         */
        void listenLoop();

        /**
         * Notifies subscribed FileHandlers when new data is available on the Connection
         */
        void processIncomingConnections();

        /**
         * Creates and returns new connection. Adds it to the epoll waiting queue
         * @param peerIP IP of the peer you want to connect to
         * @param peerPort Port of the peer you want to connect to
         * @return Shared pointer to the newly created Connection
         */
        std::shared_ptr<Connection> requestConnection(std::string peerIP, uint16_t peerPort);

        /**
         * Adds new FileHandler to the monitoring vector. If new connection arrives this queue will be searched
         * to determine which FileHandler should respond to the request.
         * @param newFileHandler FileHandler to be added to the vector.
         */
        void addFileHandler(std::shared_ptr<FileHandler> newFileHandler);

        /**
         * Replaces the currently monitored FileHandlers vector with the new one.
         * @param fileHandlers
         */
        void setFileHandlers(std::vector<std::shared_ptr<FileHandler>> fileHandlers);

        /**
         * @return Returns the currently monitored FileHandlers vector
         */
        const std::vector<std::shared_ptr<FileHandler>>& getFileHandlers() const;

        /**
         * Sets tracker IP and Port for convenient access across the application
         * @param trackerIP Public IP of the tracker
         * @param trackerPort Port of the tracker
         */
        void setTrackerDetails(std::string trackerIP, uint16_t trackerPort);

        /**
         * @return Tracker's public IP adress
         */
        const std::string getTrackerIP() const;

        /**
         * @return Tracker's port
         */
        uint16_t getTrackerPort() const;

    private:
        /**
         * Private constructor to ensure there is only one instance of
         * ConnectionManager in the application
         * @param bindIP IP address that ConnectionManager will be bound to
         * @param bindPort Port that ConnectionManager will be bound to
         */
        ConnectionManager(std::string bindIP, uint16_t bindPort);

        /**
         * Destructs ConnectionManager and closes the epoll descriptor
         */
        virtual ~ConnectionManager();

        sockaddr_in ownSocket;
        int ownSocketDescriptor;
        std::vector<std::shared_ptr<FileHandler>> fileHandlers;
        std::mutex fileHandlersMutex;

    private:
        /**
         * Responds to the INIT protocol message and adds the connection to the epoll queue
         * @param connection Connection that issued the INIT message
         * @return True if the appropriate FileHandler was found and connection was successfully
         * added to the epoll queue
         */
        bool notifyFileAboutNewConnection(std::shared_ptr<Connection> connection);

        uint16_t trackerPort;
        std::string trackerIP;
        int epollDescriptor;

        ConnectionManager() = delete;
        ConnectionManager(const ConnectionManager &) = delete;
};


#endif //FILESHARER_CONNECTIONMANAGER_H
