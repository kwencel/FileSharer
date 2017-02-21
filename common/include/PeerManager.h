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
         * Constructs a vector that contains a PeerFile instance for every peer that has a file identified by hash
         * @param hash Hash of the file in question
         * @return Vector of type PeerFile that contains ip, port and a vector of available chunks for every peer
         * that has the file in question
         */
        std::vector<PeerFile> getPeersWithFile(std::string hash);

        /**
         * Finds all distinct files (by hash) among all peers
         * @return Unordered set of FileInfo instances with unique hashes
         */
        std::vector<FileInfo> getDistinctFiles();
    private:
        /**
         * Checks if a peer with the same ip and port was already added
         * @param ip Ip of newly registered peer
         * @param port Port of newly registered peer
         * @return Pointer to a corresponding Peer instance if it was found. Else boost::none
         */
        boost::optional<Peer*> isPeerAdded(std::string ip, uint16_t port);
        std::vector<Peer> peerList;
};

#endif //FILESHARER_PEERMANAGER_H
