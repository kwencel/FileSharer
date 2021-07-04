#include "Peer.h"

#include <utility>

Peer::Peer(std::string ip, uint16_t port, std::vector<FileInfo> fileList) {
    this->ip = std::move(ip);
    this->port = port;
    this->fileList = std::move(fileList);
}

Peer::Peer() {

}

Peer::~Peer() {

}

boost::optional<std::vector<bool>> Peer::checkForFile(const std::string &hash) {
    for (FileInfo &fi : fileList) {
        if (fi.compareHash(hash)) {
            return fi.getAvailableChunks();
        }
    }
    return boost::none;
}

std::string Peer::getIp() const {
    return this->ip;
}

uint16_t Peer::getPort() const {
    return this->port;
}

std::vector<FileInfo> Peer::getFileList() const {
    return this->fileList;
}

void Peer::setFileList(std::vector<FileInfo> fileList) {
    this->fileList = std::move(fileList);
}

