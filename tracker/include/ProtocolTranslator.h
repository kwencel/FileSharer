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

        void addPeer(Peer peer);

    private:
        PeerManager peerManager;
        /**
         * Enumeration to values of which message headers are decoded
         */
        enum MessageType
        {
            MSG_DEFAULT,
            MSG_PEER_REGISTRATION,
            MSG_PEERS_WITH_FILE,
            MSG_LIST_FILES
        };

        /**
         * Returns the type of message as decoded from message header
         * @param message Message read from socket (converted to string by generateResponse())
         * @return Value of type MessageType to which the header has been decoded
         */
        MessageType decodeHeader(std::string &message);

        /**
         * Creates a string response depending on decoded header
         * @param header Header of the message
         * @return String response to be sent to client
         */
        std::string createResponse(MessageType header, std::string message);
};

#endif //FILESHARER_PROTOCOLTRANSLATOR_H
