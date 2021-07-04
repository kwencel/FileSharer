#include "PeerFile.h"

#include <utility>

PeerFile::PeerFile(std::string ip, uint16_t port, std::vector<bool> availableChunks) {
    this->ip = std::move(ip);
    this ->port = port;
    this->availableChunks = std::move(availableChunks);
}

PeerFile::PeerFile() {

}

std::string PeerFile::getIp() const {
    return this->ip;
}

uint16_t PeerFile::getPort() const {
    return this->port;
}

std::vector<bool> PeerFile::getAvailableChunks() const {
    return this->availableChunks;
}


