#include "PeerManager.h"

PeerManager::PeerManager() {

}

void PeerManager::addPeer(Peer peer) {
    this->peerList.push_back(peer);
}

std::vector<Peer> PeerManager::getPeersWithFile(std::string hash) {
    std::vector<Peer> peersWithFile;
    for (Peer &p : peerList) {
        boost::optional<std::vector<bool>> availableChunks;
        if ( (availableChunks = p.checkForFile(hash)) ) {
            peersWithFile.push_back(p);
        }
    }
    return peersWithFile;
}

std::unordered_set<FileInfo, FileInfoHasher> PeerManager::getDistinctFiles() {
    std::unordered_set<FileInfo, FileInfoHasher> distinctFiles;
    std::unordered_set<std::string> hashes;
    for (Peer &p : peerList) {
        std::vector<FileInfo> fileList = p.getFileList();
        for (FileInfo &fi : fileList) {
            if (hashes.find(fi.getHash()) != hashes.end()) {
                //distinctFiles.insert(fi);
                hashes.insert(fi.getHash()); //TODO implement hash for FileInfo
            }
        }
    }
    return distinctFiles;
}



