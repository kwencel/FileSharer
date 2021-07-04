#include <dnet.h>
#include <utility>
#include <ErrorCheckUtils.h>
#include <easylogging++.h>
#include <ProtocolUtils.h>
#include "include/ConnectionManager.h"


ConnectionManager::ConnectionManager(const std::string &bindIP, uint16_t bindPort) {
    ownSocket.sin_family = AF_INET;
    ownSocket.sin_port = htons(bindPort);
    ownSocket.sin_addr.s_addr = inet_addr(bindIP.c_str());
    ownSocketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    CHK_MSG_EX(bind(ownSocketDescriptor, (sockaddr *)&ownSocket, sizeof(ownSocket)),
            std::string("ConnectionManager bind to " + getOwnIP() + ":" + std::to_string(getOwnPort())).c_str());
    epollDescriptor = epoll_create(1);
}

ConnectionManager &ConnectionManager::getInstance(const std::string &bindIP, uint16_t bindPort) {
    static ConnectionManager instance(bindIP, bindPort);
    return instance;
}


std::string ConnectionManager::getOwnIP() const {
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(ownSocket.sin_addr), ip, INET_ADDRSTRLEN);
    return std::string(ip);
}

uint16_t ConnectionManager::getOwnPort() const {
    return ntohs(ownSocket.sin_port);
}

sockaddr_in ConnectionManager::getOwnSocket() const {
    return ownSocket;
}

int ConnectionManager::getOwnSocketDescriptor() const {
    return ownSocketDescriptor;
}

bool ConnectionManager::notifyFileAboutNewConnection(const std::shared_ptr<Connection> &connection) {
    char header = connection->read(1)[0];
    if (header == PROTOCOL_PEER_INIT_HASH) {
        std::string fileHash = connection->read(32);
        for (auto &&fileHandler : fileHandlers) {
            if (fileHandler->file->getHash() == fileHash) {
                fileHandler->addConnection(connection);
                epoll_event epollEvent;
                epollEvent.data.ptr = connection.get();
                epollEvent.events = EPOLLIN;
                epoll_ctl(epollDescriptor, EPOLL_CTL_ADD, connection->peerSocketDescriptor, &epollEvent);
                connection->write(ProtocolUtils::encodeHeader(PROTOCOL_PEER_INIT_ACK));
                return true;
            }
        }
    }
    return false;
}

void ConnectionManager::listenLoop() {
    if (listen(ownSocketDescriptor, 5) == -1) {
        throw std::runtime_error("Client listen error: " + std::string(strerror(errno)));
    }
    std::thread([&]() {
        int peerSocketDescriptor;
        sockaddr_in peerSocket {0};
        socklen_t sizeOfPeerSocket = sizeof(peerSocket);
        while (true) {
            peerSocketDescriptor = accept(ownSocketDescriptor, (sockaddr *) &peerSocket, &sizeOfPeerSocket);
            std::shared_ptr<Connection> conn = std::make_shared<Connection>(peerSocketDescriptor, peerSocket);
            LOG(INFO) << "Accepted new connection from " << conn->getPeerIP() << ":" << conn->getPeerPort();
            if (!notifyFileAboutNewConnection(conn)) {
                LOG(DEBUG) << "FileHandler associated with connection " << conn->getPeerIPandPort() << " not found";
            }
        }
    }).detach();
}

ConnectionManager::~ConnectionManager() {
    close(epollDescriptor);
}

std::shared_ptr<Connection> ConnectionManager::requestConnection(const std::string &peerIP, uint16_t peerPort) const {
    std::shared_ptr<Connection> connection = std::make_shared<Connection>(peerIP, peerPort);
    LOG(INFO) << "Created new connection to " << connection->getPeerIP() << ":" << connection->getPeerPort();
    epoll_event epollEvent;
    epollEvent.data.ptr = connection.get();
    epollEvent.events = EPOLLIN;
    epoll_ctl(epollDescriptor, EPOLL_CTL_ADD, connection->peerSocketDescriptor, &epollEvent);
    return connection;
}

void ConnectionManager::processIncomingConnections() const {
    std::thread([&]() {
        epoll_event epollEvent {0};
        int rc;
        while (true) {
            do {
                rc = epoll_wait(epollDescriptor, &epollEvent, 1, -1);
            } while (rc == -1 && errno == EINTR);
            auto* connection = static_cast<Connection *>(epollEvent.data.ptr);
            if (connection != nullptr) {
                connection->notify();
            }
        }
    }).detach();
}

void ConnectionManager::addFileHandler(const std::shared_ptr<FileHandler> &newFileHandler) {
    std::lock_guard<std::mutex> guard(fileHandlersMutex);
    this->fileHandlers.push_back(newFileHandler);
}

void ConnectionManager::setFileHandlers(std::vector<std::shared_ptr<FileHandler>> fileHandlers) {
    std::lock_guard<std::mutex> guard(fileHandlersMutex);
    this->fileHandlers = std::move(fileHandlers);
}

const std::vector<std::shared_ptr<FileHandler>>& ConnectionManager::getFileHandlers() const {
    return this->fileHandlers;
}

void ConnectionManager::setTrackerDetails(std::string trackerIP, uint16_t trackerPort) {
    this->trackerIP = std::move(trackerIP);
    this->trackerPort = trackerPort;
}

std::string ConnectionManager::getTrackerIP() const {
    return trackerIP;
}

uint16_t ConnectionManager::getTrackerPort() const {
    return trackerPort;
}

void ConnectionManager::addToDownloadingFiles(const std::shared_ptr<FileHandler> &fileHandler) {
    this->currentlyDownloadingFiles.push_back(fileHandler);
}

void ConnectionManager::removeFromDownloadingFiles(FileHandler* fileHandler) {
    for (auto it = this->currentlyDownloadingFiles.begin(); it != this->currentlyDownloadingFiles.end(); ++it) {
        if ((*it)->file->getHash() == fileHandler->file->getHash()) {
            currentlyDownloadingFiles.erase(it);
            break;
        }
    }
}

bool ConnectionManager::isFileBeingDownloaded(const FileInfo& fileInfo) {
    for (const auto &sptrFileHandler : this->currentlyDownloadingFiles) {
        if (sptrFileHandler->file->getHash() == fileInfo.getHash()) {
            LOG(INFO) << "File is being downlaoded already";
            return true;
        }
    }
    LOG(INFO) << "File is not being downloaded already";
    return false;
}
