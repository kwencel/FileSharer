#include "ProtocolTranslator.h"

ProtocolTranslator::ProtocolTranslator() {

}

std::string ProtocolTranslator::generateResponse(char message[]) {
    MessageType header = decodeHeader(message);
    return createResponse(header);
}

ProtocolTranslator::MessageType ProtocolTranslator::decodeHeader(char message[]) {
    MessageType header;
    char firstByte = message[0];
    if (firstByte == 1) header = MSG_PEER_REGISTRATION;
    else if (firstByte == 2) header = MSG_PEERS_WITH_FILE;
    else header = MSG_HELLO;

    return header;
}

std::string ProtocolTranslator::createResponse(ProtocolTranslator::MessageType header) {
    if (header == ProtocolTranslator::MSG_HELLO) return "HELLO\n";
    if (header == ProtocolTranslator::MSG_PEER_REGISTRATION) return "REGISTRATION\n";
    if (header == ProtocolTranslator::MSG_PEERS_WITH_FILE) return "FILES\n";
}
