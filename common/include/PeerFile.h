#ifndef FILESHARER_PEERFILE_H
#define FILESHARER_PEERFILE_H

#include <string>
#include <vector>
#include <boost/serialization/access.hpp>

class PeerFile {
    public:
        /**
         * Constructs an instance of type PeerFile
         * @param ip Ip of requested peer
         * @param port Port of requested peer
         * @param availableChunks Chunks of a file that the corresponding peer has
         */
        PeerFile(std::string ip, uint16_t port, std::vector<bool> availableChunks);
        PeerFile();
        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            ar & ip;
            ar & port;
            ar & availableChunks;
        }

        /**
         * @return IP number of peer
         */
        std::string getIp() const;

        /**
         * @return Port number of peer
         */
        uint16_t getPort() const;

        /**
         * @return Bool vector describing which chunks the peer has
         */
        std::vector<bool> getAvailableChunks() const;
    private:
        std::string ip;
        uint16_t port;
        std::vector<bool> availableChunks;
};
#endif //FILESHARER_PEERFILE_H
