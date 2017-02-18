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

ssize_t Connection::send(std::string data) {
    return write(peerSocketDescriptor, data.c_str(), data.length());
}

std::string Connection::receive(size_t howMany) {
    std::string data;
    ssize_t readBytes = 0;
    char buffer[RECEIVE_BUFFER];
    while ((readBytes = read(peerSocketDescriptor, buffer, howMany)) != 0) {
        data.append(buffer, static_cast<unsigned long>(readBytes));
    }
    return data;
}
