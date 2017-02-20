#include "PeerManager.h"
#include <algorithm>

PeerManager::PeerManager() {

}

bool PeerManager::addPeer(Peer peer) {
    boost::optional<Peer*> searchedForPeer = isPeerAdded(peer.getIp(), peer.getPort());
    if (searchedForPeer) {
        (*searchedForPeer)->setFileList(peer.getFileList());
        return false;
    }
    else {
        this->peerList.push_back(peer);
        return true;
    }
}

boost::optional<Peer*> PeerManager::isPeerAdded(std::string ip, uint16_t port) {
    for (Peer &p : peerList) {
        if (p.getIp() == ip && p.getPort() == port) return boost::optional<Peer*>(&p);
    }
    return boost::none;
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



