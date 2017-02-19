#include "PeerFile.h"

PeerFile::PeerFile(std::string ip, uint16_t port, std::vector<bool> availableChunks) {
    this->ip = ip;
    this ->port = port;
    this->availableChunks = availableChunks;
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


