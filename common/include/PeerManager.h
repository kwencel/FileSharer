#ifndef FILESHARER_PEERMANAGER_H
#define FILESHARER_PEERMANAGER_H

#include <unordered_set>
#include <string>
#include "Peer.h"
#include <PeerFile.h>

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
        bool addPeer(Peer peer);

        /**
         *
         * @param hash Hash of the file that will be searched for
         * @return Vector of pairs of ip of peer with the file and a vector of bool denoting which
         * chunks are available
         */
        std::vector<PeerFile> getPeersWithFile(std::string hash);

        /**
         * Find all distinct files (by hash) among all peers
         * @return Unordered set of FileInfo instances with unique hashes
         */
        std::vector<FileInfo> getDistinctFiles();
    private:
        boost::optional<Peer*> isPeerAdded(std::string ip);
        std::vector<Peer> peerList;
};

#endif //FILESHARER_PEERMANAGER_H
