#include <dnet.h>
#include <iostream>
#include <Define.h>
#include <ErrorCheckUtils.h>
#include <easylogging++.h>
#include "ConnectionManager.h"

ConnectionManager::ConnectionManager() {
    ownSocket.sin_family = AF_INET;
    ownSocket.sin_port = htons(BIND_PORT);
    ownSocket.sin_addr.s_addr = inet_addr(BIND_IP);
    ownSocketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    CHK_MSG(bind(ownSocketDescriptor, (sockaddr *)&ownSocket, sizeof(ownSocket)), "Connection Manager Bind");
}

ConnectionManager &ConnectionManager::getInstance() {
    static ConnectionManager instance;
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

void ConnectionManager::listenLoop() {
    CHK_MSG(listen(ownSocketDescriptor, 5), "ConnectionManager listen");
    int peerSocketDescriptor;
    sockaddr_in peerSocket {0};
    socklen_t sizeOfPeerSocket = sizeof(peerSocket);
    int enable = 1;
    std::thread([&]() {
        while (std::cin) {
            peerSocketDescriptor = accept(ownSocketDescriptor, (sockaddr *) &peerSocketDescriptor, &sizeOfPeerSocket);
            setsockopt(peerSocketDescriptor, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
            std::shared_ptr<Connection> conn = std::make_shared<Connection>(peerSocketDescriptor, peerSocket);
            LOG(INFO) << "Accepted new connection from " << conn.get()->getPeerIP() << ":" << conn.get()->getPeerPort();
            connectionsMutex.lock();
            connections.insert(conn);
            connectionsMutex.unlock();
        }
    }).detach();
}

ConnectionManager::~ConnectionManager() {

}

std::unordered_set<std::shared_ptr<Connection>> ConnectionManager::getActiveConnections() {
    connectionsMutex.lock();
    // TODO Ping every client to make sure connection is active - remove inactive connections from set
    std::unordered_set<std::shared_ptr<Connection>> connections = this->connections;
    connectionsMutex.unlock();
    return connections;
}

std::shared_ptr<Connection> ConnectionManager::requestConnection(std::string peerIP, uint16_t peerPort) {
    std::shared_ptr<Connection> conn = std::make_shared<Connection>(peerIP, peerPort);
    LOG(INFO) << "Created new connection to " << conn.get()->getPeerIP() << ":" << conn.get()->getPeerPort();
    connectionsMutex.lock();
    connections.insert(conn);
    connectionsMutex.unlock();
    return conn;
}
