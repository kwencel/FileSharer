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
         * @param message Message read from socket
         * @return String response to be sent to client
         */
        std::string generateResponse(char header, std::string message, Connection &conn);

    private:
        PeerManager peerManager;
};

#endif //FILESHARER_PROTOCOLTRANSLATOR_H
