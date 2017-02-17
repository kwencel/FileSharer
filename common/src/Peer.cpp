#include "Peer.h"

Peer::Peer(std::string ip, std::vector<FileInfo> fileList) {
    this->ip = ip;
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

const std::vector<FileInfo> Peer::getFileList() const {
    return this->fileList;
}
