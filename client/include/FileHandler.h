#ifndef FILESHARER_FILEHANDLER_H
#define FILESHARER_FILEHANDLER_H


#include "Observer.h"
#include "Connection.h"
#include <memory>
#include <unordered_set>
#include <map>
#include <PeerFile.h>
#include "File.h"

class File;
//class PeerFile;

class FileHandler : public Observer {
    public:
        FileHandler(FileInfo fileInfo);

        FileHandler(std::string name);

        virtual void update(Connection*) override;

        void download();

        void beginDownload();

        std::shared_ptr<Connection> establishConnection(std::string peerIP, uint16_t peerPort);

        void addConnection(std::shared_ptr<Connection> connection);

        bool initializeCommunication(std::shared_ptr<Connection> connection);

        void sendChunksHashes(std::string peerIP, uint16_t peerPort);

        void requestChunk(std::string peerIP, uint16_t peerPort, uint64_t chunkId);

        void sendChunk(std::string peerIP, uint16_t peerPort, uint64_t chunkId);

        std::unique_ptr<File> file;

    private:
        /**
         * Stores connections to peers that has some chunks of the file
         * key: IP of the connected peer
         * value: Pointer to the connection
         */
        std::map<std::string, std::shared_ptr<Connection>> connections;

        void receiveChunk(Connection *connection);

        std::vector<PeerFile> peersWithFile;
        FileInfo fileInfo;
};


#endif //FILESHARER_FILEHANDLER_H
