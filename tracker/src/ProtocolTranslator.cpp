#include "ProtocolTranslator.h"
#include "SerializationHelper.h"
#include "Define.h"

ProtocolTranslator::ProtocolTranslator() {

}

std::string ProtocolTranslator::generateResponse(char message[]) {
    std::string stringMessage = std::string(message);
    MessageType header = decodeHeader(stringMessage);
    return createResponse(header, stringMessage);
}

void ProtocolTranslator::addPeer(Peer peer) {
    this->peerManager.addPeer(peer);
}

ProtocolTranslator::MessageType ProtocolTranslator::decodeHeader(std::string &message) {
    MessageType messageType = MSG_DEFAULT;
    char headerByte = SerializationHelper::deserialize<char>(message);

    if (headerByte == PROTOCOL_HEADER_REGISTER) messageType = MSG_PEER_REGISTRATION;
    else if (headerByte == PROTOCOL_HEADER_PEERS_WITH_FILE) messageType = MSG_PEERS_WITH_FILE;
    else if (headerByte == PROTOCOL_HEADER_LIST_FILES) messageType = MSG_LIST_FILES;

    return messageType;
}

std::string ProtocolTranslator::createResponse(ProtocolTranslator::MessageType header, std::string message) {
    if (header == ProtocolTranslator::MSG_PEER_REGISTRATION) {
        Peer newPeer = SerializationHelper::deserialize<Peer>(message);
        peerManager.addPeer(newPeer); // TODO check ip and only update info if a peer with the same ip is already registered
    }
    else if (header == ProtocolTranslator::MSG_PEERS_WITH_FILE) {
        std::string hash = SerializationHelper::deserialize<std::string>(message);
        std::vector<Peer> peers = peerManager.getPeersWithFile(hash);
        std::string peerInfo = SerializationHelper::serialize<std::vector<Peer>>(peers);
        return peerInfo;
    }
    else if (header == ProtocolTranslator::MSG_LIST_FILES) {
        std::unordered_set<FileInfo, FileInfoHasher> distinctFiles = peerManager.getDistinctFiles();
        std::string distinctFilesInfo = SerializationHelper::serialize<std::unordered_set<FileInfo, FileInfoHasher>>(distinctFiles);
        return distinctFilesInfo;
    }
}


