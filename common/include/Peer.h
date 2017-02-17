#ifndef FILESHARER_PEER_H
#define FILESHARER_PEER_H

#include <string>
#include <vector>
#include <boost/optional.hpp>
#include "FileInfo.h"

class Peer {
    public:
        /**
         * Constructs an instance of class Peer
         * @param ip IP number of the peer
         * @param port Port to which peer connected
         * @param fileList Vector of FileInfo of files that the peer has and uploads
         */
        Peer(std::string ip, int port, std::vector<FileInfo> fileList);

        /**
         * Destructor of Peer (unused)
         */
        ~Peer();

        /**
         * Checks if this peer has uploaded a certain file (identified by its hash) and returns a vector of chunk availability
         * @param hash Hash of the requested file
         * @return Boost::optional wrapper of a vector of bool values denoting which chunks of the requested file this peer has.
         * If the file has not been found, returns boost::none.
         */
        boost::optional<std::vector<bool>> checkForFile(std::string hash);

        /**
         * @return IP number of peer
         */
        const std::string &getIp() const;

    private:
        std::string ip;
        int port;
        std::vector<FileInfo> fileList;

};

#endif //FILESHARER_PEER_H
