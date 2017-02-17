#ifndef FILESHARER_PEERMANAGER_H
#define FILESHARER_PEERMANAGER_H

#include <unordered_set>
#include <string>
#include "Peer.h"

class PeerManager {
    public:
        /**
         * Constructs an instance of PeerManager class
         */
        PeerManager();

        /**
         * Adds a Peer instance to the list of peers that tracker knows about
         * @param peer Instance of Peer to be added to tracker's peer list
         */
        void addPeer(Peer peer);

        /**
         *
         * @param hash Hash of the file that will be searched for
         * @return Vector of pairs of ip of peer with the file and a vector of bool denoting which
         * chunks are available
         */
        std::vector<Peer> getPeersWithFile(std::string hash);
    private:
        std::vector<Peer> peerList;
};

#endif //FILESHARER_PEERMANAGER_H
