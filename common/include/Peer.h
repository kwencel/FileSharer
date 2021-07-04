#ifndef FILESHARER_PEER_H
#define FILESHARER_PEER_H

#include <string>
#include <vector>
#include <boost/optional.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <FileInfo.h>

class Peer {
    public:
        /**
         * Constructs an instance of class Peer
         * @param ip IP number of the peer
         * @param port Port to which peer connected
         * @param fileList Vector of FileInfo of files that the peer has and uploads
         */
        Peer(std::string ip, uint16_t port, std::vector<FileInfo> fileList);
        Peer();

        /**
         * Destructor of Peer (unused)
         */
        ~Peer();

        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            ar & ip;
            ar & port;
            ar & fileList;
        }

        /**
         * Checks if this peer has uploaded a certain file (identified by its hash) and returns a vector of chunk availability
         * @param hash Hash of the requested file
         * @return Boost::optional wrapper of a vector of bool values denoting which chunks of the requested file this peer has.
         * If the file has not been found, returns boost::none.
         */
        boost::optional<std::vector<bool>> checkForFile(const std::string& hash);

        /**
         * @return IP number of peer
         */
        std::string getIp() const;

        /**
         * @return Port number of peer
         */
        uint16_t getPort() const;

        /**
         * @return List of peers files as a vector of type FileInfo
         */
        std::vector<FileInfo> getFileList() const;

        void setFileList(std::vector<FileInfo> fileList);

    private:
        std::string ip;
        uint16_t port;
        std::vector<FileInfo> fileList;

};

#endif //FILESHARER_PEER_H
