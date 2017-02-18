#ifndef FILESHARER_PROTOCOLTRANSLATOR_H
#define FILESHARER_PROTOCOLTRANSLATOR_H

#include <string>
#include "PeerManager.h"

class ProtocolTranslator {
    public:
        ProtocolTranslator();
        /**
         * Generates a response to a message received from a client
         * @param message Message read from socket
         * @return String response to be sent to client
         */
        std::string generateResponse(char message[]);

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
        std::string createResponse(std::string message);
};

#endif //FILESHARER_PROTOCOLTRANSLATOR_H
