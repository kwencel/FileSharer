#include <dnet.h>
// TODO replace dnet.h with sth more commonly installed
#include <iostream>
#include <ErrorCheckUtils.h>
#include <easylogging++.h>
#include "include/ConnectionManager.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
ConnectionManager::ConnectionManager(std::string bindIP, uint16_t bindPort) {
    ownSocket.sin_family = AF_INET;
    ownSocket.sin_port = htons(bindPort);
    ownSocket.sin_addr.s_addr = inet_addr(bindIP.c_str());
    ownSocketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    CHK_MSG(bind(ownSocketDescriptor, (sockaddr *)&ownSocket, sizeof(ownSocket)),
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
    std::string header = connection.get()->read(1);
    if (header[0] == PROTOCOL_PEER_INIT_HASH) {
        std::string fileHash = connection.get()->read(32);
        for (auto &&fileHandler : fileHandlers) {
            if (fileHandler.get()->file->getHash() == fileHash) {
                fileHandler.get()->addConnection(connection);
                epoll_event epollEvent;
                epollEvent.data.ptr = connection.get();
                epollEvent.events = EPOLLIN;
                epoll_ctl(epollDescriptor, EPOLL_CTL_ADD, connection.get()->peerSocketDescriptor, &epollEvent);
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
                close(peerSocketDescriptor);
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
    // TODO Ping every client to make sure connection is active - remove inactive connections from set
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
        epoll_event epollEvent;
        while (true) {
            epoll_wait(epollDescriptor, &epollEvent, 1, -1);
            static_cast<Connection *>(epollEvent.data.ptr)->notify();
        }
    }).detach();
}

void ConnectionManager::removeConnection(Connection *connection) {
//    epoll_ctl(epollDescriptor, EPOLL_CTL_DEL, connection.get()->peerSocketDescriptor, )
    connectionsMutex.lock();
    connections.erase(std::shared_ptr<Connection>(connection)); // FIXME verify if it's working
    connectionsMutex.unlock();
}

#pragma clang diagnostic pop