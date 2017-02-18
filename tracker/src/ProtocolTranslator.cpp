#include <easylogging++.h>
#include "ProtocolTranslator.h"
#include "SerializationHelper.h"
#include "Define.h"

ProtocolTranslator::ProtocolTranslator() {

}

std::string ProtocolTranslator::generateResponse(char message[]) {
    std::string stringMessage = std::string(message);
    return createResponse(stringMessage);
}

void ProtocolTranslator::addPeer(Peer peer) {
    this->peerManager.addPeer(peer);
}

std::string ProtocolTranslator::createResponse(std::string message) {
    LOG(INFO) << "Message: " + message;
    char headerByte = SerializationHelper::deserializeHeader(message);
    LOG(INFO) << "Incoming header: " + std::to_string(headerByte);

    if (headerByte == PROTOCOL_HEADER_REGISTER) {
        Peer newPeer = SerializationHelper::deserialize<Peer>(message);
        peerManager.addPeer(newPeer); // TODO check ip and only update info if a peer with the same ip is already registered
        LOG(INFO) << "Peer registered! IP: " + newPeer.getIp() + ", number of files: " + std::to_string(newPeer.getFileList().size());
        LOG(INFO) << "Files:";
        for (FileInfo fi : newPeer.getFileList()) {
            LOG(INFO) << "Name: " + fi.getFilename() + ", hash: " + fi.getHash() + ", chunks: " + fi.printChunks();
        }
        return "SUCCESS\n";
    }
    else if (headerByte == PROTOCOL_HEADER_PEERS_WITH_FILE) {
        std::string hash = SerializationHelper::deserialize<std::string>(message);
        std::vector<std::pair<std::string, std::vector<bool>>> peers = peerManager.getPeersWithFile(hash);
        std::string peerInfo = SerializationHelper::serialize<std::vector<std::pair<std::string, std::vector<bool>>>>(peers);
        return peerInfo;
    }
    else if (headerByte == PROTOCOL_HEADER_LIST_FILES) {
        std::vector<FileInfo> distinctFiles = peerManager.getDistinctFiles();
        std::string distinctFilesInfo = SerializationHelper::serialize<std::vector<FileInfo>>(distinctFiles);
        return distinctFilesInfo;
    }
    else return "WRONG HEADER\n";
}


