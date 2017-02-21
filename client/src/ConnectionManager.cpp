#include <dnet.h>
#include <iostream>
#include <ErrorCheckUtils.h>
#include <easylogging++.h>
#include <ProtocolUtils.h>
#include "include/ConnectionManager.h"


ConnectionManager::ConnectionManager(std::string bindIP, uint16_t bindPort) {
    ownSocket.sin_family = AF_INET;
    ownSocket.sin_port = htons(bindPort);
    ownSocket.sin_addr.s_addr = inet_addr(bindIP.c_str());
    ownSocketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    CHK_MSG_EX(bind(ownSocketDescriptor, (sockaddr *)&ownSocket, sizeof(ownSocket)),
            std::string("ConnectionManager bind to " + getOwnIP() + ":" + std::to_string(getOwnPort())).c_str());
    epollDescriptor = epoll_create(1);
}

ConnectionManager &ConnectionManager::getInstance(std::string bindIP, uint16_t bindPort) {
    static ConnectionManager instance(bindIP, bindPort);
    return instance;
}


std::string ConnectionManager::getOwnIP() {
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(ownSocket.sin_addr), ip, INET_ADDRSTRLEN);
    return std::string(ip);
}

uint16_t ConnectionManager::getOwnPort() {
    return ntohs(ownSocket.sin_port);
}

sockaddr_in ConnectionManager::getOwnSocket() {
    return ownSocket;
}

int ConnectionManager::getOwnSocketDescriptor() {
    return ownSocketDescriptor;
}

bool ConnectionManager::notifyFileAboutNewConnection(std::shared_ptr<Connection> connection) {
    char header = connection.get()->read(1)[0];
    if (header == PROTOCOL_PEER_INIT_HASH) {
        std::string fileHash = connection.get()->read(32);
        for (auto &&fileHandler : fileHandlers) {
            if (fileHandler.get()->file->getHash() == fileHash) {
                fileHandler.get()->addConnection(connection);
                epoll_event epollEvent;
                epollEvent.data.ptr = connection.get();
                epollEvent.events = EPOLLIN;
                epoll_ctl(epollDescriptor, EPOLL_CTL_ADD, connection.get()->peerSocketDescriptor, &epollEvent);
                connection.get()->write(ProtocolUtils::encodeHeader(PROTOCOL_PEER_INIT_ACK));
                return true;
            }
        }
    }
    return false;
}

void ConnectionManager::listenLoop() {
    CHK_MSG(listen(ownSocketDescriptor, 5), "ConnectionManager listen");
    std::thread([&]() {
        int peerSocketDescriptor;
        int enable = 1;
        sockaddr_in peerSocket {0};
        socklen_t sizeOfPeerSocket = sizeof(peerSocket);
        while (true) {
            peerSocketDescriptor = accept(ownSocketDescriptor, (sockaddr *) &peerSocket, &sizeOfPeerSocket);
            setsockopt(peerSocketDescriptor, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
            std::shared_ptr<Connection> conn = std::make_shared<Connection>(peerSocketDescriptor, peerSocket);
            LOG(INFO) << "Accepted new connection from " << conn.get()->getPeerIP() << ":" << conn.get()->getPeerPort();
            if (!notifyFileAboutNewConnection(conn)) {
                // Appropriate file now found
                LOG(DEBUG) << "Connecion would be closed";
                continue;
            }
            connectionsMutex.lock();
            connections.insert(conn);
            connectionsMutex.unlock();
        }
    }).detach();
}

ConnectionManager::~ConnectionManager() {
    close(epollDescriptor);
}

std::unordered_set<std::shared_ptr<Connection>> ConnectionManager::getActiveConnections() {
    connectionsMutex.lock();
    std::unordered_set<std::shared_ptr<Connection>> connections = this->connections;
    connectionsMutex.unlock();
    return connections;
}

std::shared_ptr<Connection> ConnectionManager::requestConnection(std::string peerIP, uint16_t peerPort) {
    std::shared_ptr<Connection> connection = std::make_shared<Connection>(peerIP, peerPort);
    LOG(INFO) << "Created new connection to " << connection.get()->getPeerIP() << ":" << connection.get()->getPeerPort();
    connectionsMutex.lock();
    connections.insert(connection);
    connectionsMutex.unlock();
    epoll_event epollEvent;
    epollEvent.data.ptr = connection.get();
    epollEvent.events = EPOLLIN;
    epoll_ctl(epollDescriptor, EPOLL_CTL_ADD, connection.get()->peerSocketDescriptor, &epollEvent);
    return connection;
}

void ConnectionManager::processIncomingConnections() {
    std::thread([&]() {
        epoll_event epollEvent {0};
        int rc;
        while (true) {
            do {
                rc = epoll_wait(epollDescriptor, &epollEvent, 1, -1);
            } while (rc == -1 && errno == EINTR);
            Connection* connection = static_cast<Connection *>(epollEvent.data.ptr);
            if (connection != nullptr) {
                connection->notify();
            }
        }
    }).detach();
}

void ConnectionManager::removeConnection(Connection *connection) {
    connectionsMutex.lock();
    for (auto it = connections.begin(); it != connections.end(); ++it) {
        if ((*it).get() == connection) {
            connections.erase(it);
            break;
        }
    }
    connectionsMutex.unlock();
}

void ConnectionManager::addFileHandler(std::shared_ptr<FileHandler> newFileHandler) {
    fileHandlersMutex.lock();
    this->fileHandlers.push_back(newFileHandler);
    fileHandlersMutex.unlock();
}

void ConnectionManager::setFileHandlers(std::vector<std::shared_ptr<FileHandler>> fileHandlers) {
    fileHandlersMutex.lock();
    this->fileHandlers = fileHandlers;
    fileHandlersMutex.unlock();
}

const std::vector<std::shared_ptr<FileHandler>>& ConnectionManager::getFileHandlers() const {
    return this->fileHandlers;
}

void ConnectionManager::setTrackerDetails(std::string trackerIP, uint16_t trackerPort) {
    this->trackerIP = trackerIP;
    this->trackerPort = trackerPort;
}

const std::string ConnectionManager::getTrackerIP() const {
    return trackerIP;
}

uint16_t ConnectionManager::getTrackerPort() const {
    return trackerPort;
}