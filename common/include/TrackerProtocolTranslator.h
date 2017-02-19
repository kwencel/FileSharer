#ifndef FILESHARER_TRACKERPROTOCOLTRANSLATOR_H
#define FILESHARER_TRACKERPROTOCOLTRANSLATOR_H

#include <string>
#include "PeerManager.h"

class TrackerProtocolTranslator {
    public:
        TrackerProtocolTranslator();
        /**
         * Generates a response to a message received from a client
         * @param message Message read from socket
         * @return String response to be sent to client
         */
        std::string generateResponse(char header, std::string message);

        /**
         * Add peer to PeerManager (calls PeerManager::addPeer())
         * @param peer Peer to be added
         */
        void addPeer(Peer peer);

    private:
        PeerManager peerManager;

        /**
         * Creates a string response depending on decoded header
         * @param header Header of the message
         * @return String response to be sent to client
         */
        std::string createResponse(char header, std::string message);
};

#endif //FILESHARER_PROTOCOLTRANSLATOR_H
