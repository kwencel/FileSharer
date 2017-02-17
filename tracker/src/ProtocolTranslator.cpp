#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include "ProtocolTranslator.h"
#include "define.h"

ProtocolTranslator::ProtocolTranslator() {

}

std::string ProtocolTranslator::generateResponse(char message[]) {
    MessageType header = decodeHeader(message);
    return createResponse(header);
}

void ProtocolTranslator::addPeer(Peer peer) {
    this->peerManager.addPeer(peer);
}

ProtocolTranslator::MessageType ProtocolTranslator::decodeHeader(char message[]) {
    MessageType header;
    char firstByte = message[0];
    if (firstByte == PROTOCOL_HEADER_REGISTER) header = MSG_PEER_REGISTRATION;
    else if (firstByte == PROTOCOL_HEADER_FILE_REQUEST) header = MSG_PEERS_WITH_FILE;
    else if (firstByte == 'T') header = MSG_TEST;
    else header = MSG_HELLO;

    return header;
}

std::string ProtocolTranslator::createResponse(ProtocolTranslator::MessageType header) {
    if (header == ProtocolTranslator::MSG_HELLO) return "HELLO\n";
    if (header == ProtocolTranslator::MSG_PEER_REGISTRATION) return "REGISTRATION\n";
    if (header == ProtocolTranslator::MSG_PEERS_WITH_FILE) return "FILES\n";
    if (header == ProtocolTranslator::MSG_TEST) {
        std::vector<Peer> peers = peerManager.getPeersWithFile("HASH");
        std::stringstream archive_stream;
        boost::archive::text_oarchive archive(archive_stream);
        archive << peers[0];
        archive << peers[1];
        return archive_stream.str();
    }
}


