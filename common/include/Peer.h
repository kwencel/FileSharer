#ifndef FILESHARER_PEER_H
#define FILESHARER_PEER_H

#include <string>
#include <vector>
#include <boost/optional.hpp>
#include "FileInfo.h"

class Peer {
    public:
        Peer(std::string ip, int port, std::vector<FileInfo> fileList);
        ~Peer();
        boost::optional<std::vector<bool>> checkForFile(std::string hash);
        const std::string &getIp() const;

    private:
        std::string ip;
        int port;
        std::vector<FileInfo> fileList;

};

#endif //FILESHARER_PEER_H
