#ifndef FILESHARER_TRACKERPROTOCOLTRANSLATOR_H
#define FILESHARER_TRACKERPROTOCOLTRANSLATOR_H

#include <string>
#include "PeerManager.h"
#include <Connection.h>

class TrackerProtocolTranslator {
    public:
        TrackerProtocolTranslator();
        /**
         * Generates a response to a message received from a client
         * @param header Header read from socket
         * @param message Message read from socket
         * @param conn Reference to a Connection instance corresponding to the sender
         * @return String response to be sent to client
         */
        std::string generateResponse(char header, const std::string& message, Connection &conn);

    private:
        PeerManager peerManager;
};

#endif //FILESHARER_PROTOCOLTRANSLATOR_H
