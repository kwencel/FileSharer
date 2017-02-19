#ifndef FILESHARER_FILEHANDLER_H
#define FILESHARER_FILEHANDLER_H


#include "Observer.h"
#include "Connection.h"
#include <memory>
#include <unordered_set>
#include <map>
#include "File.h"

class File;

class FileHandler : public Observer {
    public:
        FileHandler(File *file);

        FileHandler(FileInfo fileInfo);

        virtual void update(Connection*) override;
        //        vector<pair<string(ip), vector<chunk>>
        void startDownload(std::vector<std::pair<std::string, std::vector<bool>>> peers);

        void download();
        File *file;

        std::shared_ptr<Connection> establishConnection(std::string peerIP, uint16_t peerPort);

        void addConnection(std::shared_ptr<Connection> connection);

        bool initializeCommunication(std::shared_ptr<Connection> connection);

        std::vector<std::string> requestChunksHashes(std::string peerIP, uint16_t peerPort);

        void sendChunksHashes(std::string peerIP, uint16_t peerPort);

        void requestChunk(std::string peerIP, uint16_t peerPort, uint64_t chunkId);

        void sendChunk(std::string peerIP, uint16_t peerPort, uint64_t chunkId);

    private:
        /**
         * Stores connections to peers that has some chunks of the file
         * key: IP of the connected peer
         * value: Pointer to the connection
         */
        std::map<std::string, std::shared_ptr<Connection>> connections;
};


#endif //FILESHARER_FILEHANDLER_H
