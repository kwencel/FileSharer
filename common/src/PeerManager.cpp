#include "PeerManager.h"

PeerManager::PeerManager() {

}

void PeerManager::addPeer(Peer peer) {
    this->peerList.push_back(peer);
}

std::vector<PeerFile> PeerManager::getPeersWithFile(std::string hash) {
    std::vector<PeerFile> peerChunkAvailability;
    for (Peer &p : peerList) {
        boost::optional<std::vector<bool>> availableChunks;
        if ( (availableChunks = p.checkForFile(hash)) ) {
            peerChunkAvailability.emplace_back(PeerFile(p.getIp(), p.getPort(), *availableChunks));
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



