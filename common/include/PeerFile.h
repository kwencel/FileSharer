#ifndef FILESHARER_PEERFILE_H
#define FILESHARER_PEERFILE_H

#include <string>
#include <vector>
#include <boost/serialization/access.hpp>

class PeerFile {
    public:
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

        std::vector<bool> getAvailableChunks() const;
    private:
        std::string ip;
        uint16_t port;
        std::vector<bool> availableChunks;
};
#endif //FILESHARER_PEERFILE_H
