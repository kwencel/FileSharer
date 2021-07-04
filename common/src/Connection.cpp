#include <dnet.h>
#include <ErrorCheckUtils.h>
#include <easylogging++.h>
#include "Connection.h"
#include <unistd.h>
#include <CustomExceptions.h>
#include <fcntl.h>

Connection::Connection(const std::string &peerIP, uint16_t peerPort) {
    peerSocket.sin_family = AF_INET;
    peerSocket.sin_port = htons(peerPort);
    peerSocket.sin_addr.s_addr = inet_addr(peerIP.c_str());
    peerSocketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    long flags;
    if ((flags = fcntl(peerSocketDescriptor, F_GETFL, NULL)) < 0) {
        LOG(ERROR) << "Establishing new Connection: Error on fcntl F_GETFL " + std::string(strerror(errno));
    }
    flags |= O_NONBLOCK;

    if ( fcntl(peerSocketDescriptor, F_SETFL, flags) < 0) {
        LOG(ERROR) << "Establishing new Connection: Error on fcntl F_SETFL " + std::string(strerror(errno));
    }

    int result = connect(peerSocketDescriptor, (sockaddr *)&peerSocket, sizeof(peerSocket));
    if (result < 0) {
        if (errno == EINPROGRESS) {
            LOG(INFO) << "Establishing new Connection: EINPROGRESS in connect() - selecting";
            timeval tv;
            fd_set set;
            do {
                tv.tv_sec = 5;
                tv.tv_usec = 0;
                FD_ZERO(&set);
                FD_SET(peerSocketDescriptor, &set);
                result = select(peerSocketDescriptor + 1, nullptr, &set, nullptr, &tv);
                if (result < 0 && errno != EINTR) {
                    throw ConnectionError("Error on connect: " + std::string(strerror(errno)));
                } else if (result > 0) {
                    socklen_t len = sizeof(int);
                    int valopt;
                    if (getsockopt(peerSocketDescriptor, SOL_SOCKET, SO_ERROR, (void *) (&valopt), &len) < 0) {
                        throw ConnectionError("Error on connect: " + std::string(strerror(errno)));
                    }
                    if (valopt) {
                        throw ConnectionError("Outgoing connection creation failed!");
                    }
                    break;
                }
                else {
                    throw ConnectionError("Timeout in select()");
                }
            } while(true);
        }
        else {
            throw ConnectionError(strerror(errno));
        }
    }
    if ((flags = fcntl(peerSocketDescriptor, F_GETFL, NULL)) < 0) {
        LOG(ERROR) << "Establishing new Connection: Error on fcntl F_GETFL " + std::string(strerror(errno));
        exit(0);
    }
    flags &= (~O_NONBLOCK);
    if (fcntl(peerSocketDescriptor, F_SETFL, flags) < 0) {
        LOG(ERROR) << "Establishing new Connection: Error on fcntl F_SETFL " + std::string(strerror(errno));
        exit(0);
    }

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

uint16_t Connection::getPeerPort() const {
    return ntohs(peerSocket.sin_port);
}

std::string Connection::getPeerIPandPort() {
    std::stringstream ss;
    ss << getPeerIP() << ":" << getPeerPort();
    return ss.str();
}

Connection::~Connection() {
    close(peerSocketDescriptor);
}

ssize_t Connection::write(const std::string& data) const {
    return send(peerSocketDescriptor, data.c_str(), data.length(), 0);
}

std::string Connection::read(size_t howMany, time_t timeout) {
    std::string data;
    ssize_t readBytes = 0;
    size_t toRead;
    int retries = 0;
    char buffer[RECEIVE_BUFFER];

    struct timeval tv;
    tv.tv_sec = timeout;
    tv.tv_usec = 0;
    setsockopt(this->peerSocketDescriptor, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(struct timeval));

    do {
        if (howMany > RECEIVE_BUFFER) {
            toRead = RECEIVE_BUFFER;
        } else {
            toRead = howMany;
        }
        readBytes = recv(peerSocketDescriptor, buffer, toRead, 0);
        if (readBytes == 0) {
            // Socket descriptor was closed on the other end - EOF
            LOG(INFO) << "Closing connection to " << getPeerIPandPort();
            while (not observers.empty()) {
                (*(observers.begin()))->stopObserving(this);
                close(peerSocketDescriptor);
            }
            return data;
        }
        while (errno == EAGAIN && retries < READ_RETRIES) {
            ++retries;
            LOG(WARNING) << "Read() timeout - retry attempt " + std::to_string(retries);
            readBytes = recv(peerSocketDescriptor, buffer, toRead, 0);
        }
        if (retries == READ_RETRIES) {
            throw ReadTimeoutError(this->getPeerIP(), this->getPeerPort());
        }
        if (readBytes == -1) {
            perror("Error during read in Connection");
            throw std::runtime_error("Error during read in Connection");
        }
        howMany -= readBytes;
        if (howMany < 0) {
            LOG(DEBUG) << "Read returned more than expected";
        }
        data.append(buffer, static_cast<unsigned long>(readBytes));
    } while (howMany > 0);
    return data;
}

void Connection::notify() {
    std::forward_list<Observer*> tmpObservers(observers);
    for (auto &&observer : tmpObservers) {
        observer->update(this);
    }
}

void Connection::registerObserver(Observer *observer) {
    observers.push_front(observer);
}

void Connection::unregisterObserver(Observer *observer) {
    observers.remove(observer);
}
