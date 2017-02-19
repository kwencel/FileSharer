#include <dnet.h>
#include <ErrorCheckUtils.h>
#include <easylogging++.h>
#include "Connection.h"
#include <unistd.h>
#include <CustomExceptions.h>

Connection::Connection(std::string peerIP, uint16_t peerPort) {
    peerSocket.sin_family = AF_INET;
    peerSocket.sin_port = htons(peerPort);
    peerSocket.sin_addr.s_addr = inet_addr(peerIP.c_str());
    peerSocketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    CHK_CUSTOM(connect(peerSocketDescriptor, (sockaddr *)&peerSocket, sizeof(peerSocket)),
               (throw ConnectionError(std::string("Outgoing connection creation: ") + strerror(errno))));
}

Connection::Connection(int peerSocketDescriptor, sockaddr_in peerSocket) {
    this->peerSocketDescriptor = peerSocketDescriptor;
    this->peerSocket = peerSocket;
}

std::string Connection::getPeerIP() {
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(peerSocket.sin_addr), ip, INET_ADDRSTRLEN);
    return std::string(ip);
}

uint16_t Connection::getPeerPort() {
    return ntohs(peerSocket.sin_port);
}

Connection::~Connection() {
    close(peerSocketDescriptor);
}

ssize_t Connection::write(std::string data) {
    return send(peerSocketDescriptor, data.c_str(), data.length(), 0);
}

std::string Connection::read(size_t howMany) {
    if (howMany > RECEIVE_BUFFER) {
        LOG(DEBUG) << "Requested read won't fit into receiving buffer. Fallback to safe buffer length value.";
    }
    std::string data;
    ssize_t readBytes = 0;
    size_t toRead;
    char buffer[RECEIVE_BUFFER];
    do {
        if (howMany > RECEIVE_BUFFER) {
            toRead = RECEIVE_BUFFER;
        } else {
            toRead = howMany;
        }
        // TODO Add timeout
        readBytes = recv(peerSocketDescriptor, buffer, toRead, 0);
        howMany -= readBytes;
        if (howMany < 0) {
            LOG(DEBUG) << "Read returned more than expected";
        }
        data.append(buffer, static_cast<unsigned long>(readBytes));
    } while (howMany > 0);
    return data;
}

void Connection::notify() {
    for (auto &&observer : observers) {
        observer->update(this);
    }
}

void Connection::registerObserver(Observer *observer) {
    observers.push_front(observer);
}

void Connection::unregisterObserver(Observer *observer) {
    observers.remove(observer);
}
