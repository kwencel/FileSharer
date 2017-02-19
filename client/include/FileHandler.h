#ifndef FILESHARER_FILEHANDLER_H
#define FILESHARER_FILEHANDLER_H


#include <Observer.h>
#include <Connection.h>
#include <memory>
#include <unordered_set>
#include <map>
#include "File.h"

class File;

class FileHandler : public Observer {
    public:
        FileHandler(File *file);
        virtual void update(Connection*) override;
        //        vector<pair<string(ip), vector<chunk>>
        void startDownload(std::vector<std::pair<std::string, std::vector<bool>>> peers);
        void download();

    private:
        File *file;
        /**
         * Stores connections to peers that has some chunks of the file
         * key: IP of the connected peer
         * value: Pointer to the connection
         */
        std::map<std::string, std::shared_ptr<Connection>> connections;


        void requestChunk(uint64_t id, std::string peerIP);

        std::shared_ptr<Connection> establishConnection(std::string peerIP, uint16_t peerPort);
};


#endif //FILESHARER_FILEHANDLER_H
