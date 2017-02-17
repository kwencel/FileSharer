#include "Peer.h"

Peer::Peer(std::string ip, int port, std::vector<FileInfo> fileList) {
    this->ip = ip;
    this->port = port;
    this->fileList = fileList;
}

Peer::Peer() {

}

Peer::~Peer() {

}

boost::optional<std::vector<bool>> Peer::checkForFile(std::string hash) {
    for (FileInfo &fi : fileList) {
        if (fi.compareHash(hash)) {
            return fi.getAvailableChunks();
        }
    }
    return boost::none;
}

const std::string &Peer::getIp() const {
    return ip;
}

void Peer::printSerializedInfo() {
    printf("IP = %s\nBool vector = ", ip.c_str());
    for (FileInfo &f: fileList) {
        for (bool b: f.getAvailableChunks()) {
            printf("%d ", b);
            fflush(stdout);
        }
    }
}
