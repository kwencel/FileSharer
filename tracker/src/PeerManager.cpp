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



