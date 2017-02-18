#include "PeerManager.h"

PeerManager::PeerManager() {

}

void PeerManager::addPeer(Peer peer) {
    this->peerList.push_back(peer);
}

std::vector<std::pair<std::string, std::vector<bool>>> PeerManager::getPeersWithFile(std::string hash) {
    std::vector<std::pair<std::string, std::vector<bool>>> peerChunkAvailability;
    for (Peer &p : peerList) {
        boost::optional<std::vector<bool>> availableChunks;
        if ( (availableChunks = p.checkForFile(hash)) ) {
            peerChunkAvailability.push_back(std::make_pair(p.getIp(), *availableChunks));
        }
    }
    return peerChunkAvailability;
}

std::vector<FileInfo> PeerManager::getDistinctFiles() {
    std::vector<FileInfo> distinctFiles;
    std::unordered_set<std::string> hashes;
    
    for (Peer &peer : peerList) {
        for (FileInfo &fileInfo : peer.getFileList()) {
            if (hashes.find(fileInfo.getHash()) == hashes.end()) {
                distinctFiles.push_back(fileInfo);
                hashes.insert(fileInfo.getHash());
            }
        }
    }
    return distinctFiles;
}



