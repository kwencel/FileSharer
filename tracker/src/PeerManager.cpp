#include "PeerManager.h"

PeerManager::PeerManager() {

}

void PeerManager::addPeer(Peer peer) {
    this->peerList.push_back(peer);
}

std::vector<std::pair<std::string, std::vector<bool>>> PeerManager::getPeersWithFile(std::string hash) {
    std::vector<std::pair<std::string, std::vector<bool>>> peersWithFile;
    for (Peer &p : peerList) {
        boost::optional<std::vector<bool>> availableChunks;
        if ( (availableChunks = p.checkForFile(hash)) ) {
            std::pair<std::string, std::vector<bool>> newPair = std::make_pair(p.getIp(), *availableChunks);
            peersWithFile.push_back(newPair);
        }
    }
}



