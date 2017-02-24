#ifndef FILESHARER_FILEHANDLER_H
#define FILESHARER_FILEHANDLER_H


#include "Observer.h"
#include "Connection.h"
#include <memory>
#include <unordered_set>
#include <map>
#include <PeerFile.h>
#include <QObject>
#include "File.h"
#include <mutex>
#include <memory>

class File;

class FileHandler :  public QObject, public Observer {
        Q_OBJECT

    public:
        /**
         * Constructs the FileHandler from information gathered from tracker
         * @param fileInfo Basic information about the file
         */
        FileHandler(FileInfo fileInfo);

        /**
         * Constructs the FileHandler based on information read from disk
         * @param name Name of the file
         */
        FileHandler(std::string name);

        /**
         * Allows Connection to notify FileTracker about incoming data (Observer/Listener pattern applied)
         */
        virtual void update(Connection*) override;

        /**
         * Removed observed connection from internal list and unsubscribes from its notifications
         */
        virtual void stopObserving(Connection*) override;

        /**
         * Starts the download process
         */
        void beginDownload();

        /**
         * Establishes connection to peer with given IP and port. Makes the FileWriter subscriber of this newly created
         * Connection notifications by default
         * @param peerIP IP of the peer you want to connect to
         * @param peerPort IP of the peer you want to connect to
         * @param dontRegister IP of the peer you want to connect to
         * @return Shared pointer to the newly established connection
         */
        std::shared_ptr<Connection> establishConnection(std::string peerIP, uint16_t peerPort, bool dontRegister = false);

        /**
         * Adds connection to the internal list and subscribes to its notifications
         * @param connection Connection
         */
        void addConnection(std::shared_ptr<Connection> connection);

        /**
         * Adds connection to the internal list and subscribes to its notifications
         */
        bool initializeCommunication(std::shared_ptr<Connection> connection);

        /**
         * Sends chunks hashes to the given Peer
         * @param peerIP IP of the peer you want to send the information to
         * @param peerPort Port of the peer you want to send the information to
         */
        void sendChunksHashes(std::string peerIP, uint16_t peerPort);

        /**
         * Sends a chunk's data request to the given Peer
         * @param peerIP IP of the peer you want to send the information to
         * @param peerPort Port of the peer you want to send the information to
         * @param chunkId ID of the chunk the request concern
         */
        void requestChunk(std::string peerIP, uint16_t peerPort, uint64_t chunkId);

        /**
         * Sends a chunk's data to the given Peer
         * @param peerIP IP of the peer you want to send the information to
         * @param peerPort Port of the peer you want to send the information to
         * @param chunkId ID of the chunk the request concern
         */
        void sendChunk(std::string peerIP, uint16_t peerPort, uint64_t chunkId);

        /**
         * Unique pointer to the File associated with the FileHandler
         */
        std::unique_ptr<File> file;

    signals:
        void updateFileHandlerProgress(FileHandler* fileHandler);

    private:
        /**
         * Stores connections to peers that has some chunks of the file
         * key: IP of the connected peer
         * value: Pointer to the connection
         */
        std::map<std::string, std::shared_ptr<Connection>> connections;

        /**
         * Reads the incoming chunk data and saves it to disk
         * @param connection
         * @return ID of the chunk which data was received
         */
        uint64_t receiveChunk(Connection *connection);

        /**
         * Updates the download progress information in the GUI
         * @param fileHandler FileHandler that will be used as a data source for the update
         */
        void updateProgress();

        /**
         * Represents the information which Peers (IP and Port) have which chunks of the File.
         */
        std::vector<PeerFile> peersWithFile;

        /**
         * Represents the most basic information about the File.
         */
        FileInfo fileInfo;
};


#endif //FILESHARER_FILEHANDLER_H
