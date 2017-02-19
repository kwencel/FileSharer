#include <easylogging++.h>
#include "TrackerProtocolTranslator.h"
#include "SerializationHelper.h"
#include "Define.h"

TrackerProtocolTranslator::TrackerProtocolTranslator() {

}

std::string TrackerProtocolTranslator::generateResponse(char header, std::string message) {
    if (header == PROTOCOL_HEADER_REGISTER) {
        Peer newPeer = SerializationHelper::deserialize<Peer>(message);
        peerManager.addPeer(newPeer); // TODO check ip and only update info if a peer with the same ip is already registered
        LOG(INFO) << "Peer registered! IP: " + newPeer.getIp() + ", number of files: " + std::to_string(newPeer.getFileList().size());
        LOG(INFO) << "Files:";
        for (FileInfo fi : newPeer.getFileList()) {
            LOG(INFO) << "Name: " + fi.getFilename() + ", hash: " + fi.getHash() + ", chunks: " + fi.printChunks();
        }
        char returnHeader = PROTOCOL_HEADER_REGISTER_ACCEPT;
        std::string response = SerializationHelper::serialize<std::string>(returnHeader, "");
        LOG(INFO) << "Response size: " + std::to_string(response.size());
        return response;
    }
    else if (header == PROTOCOL_HEADER_PEERS_WITH_FILE) {
        std::string hash = SerializationHelper::deserialize<std::string>(message);
        std::vector<PeerFile> peers = peerManager.getPeersWithFile(hash);
        std::string peerInfo = SerializationHelper::serialize<std::vector<PeerFile>>(header, peers);
        return peerInfo;
    }
    else if (header == PROTOCOL_HEADER_LIST_FILES) {
        std::vector<FileInfo> distinctFiles = peerManager.getDistinctFiles();
        std::string distinctFilesInfo = SerializationHelper::serialize<std::vector<FileInfo>>(header, distinctFiles);
        return distinctFilesInfo;
    }
    else return "WRONG HEADER\n";
}


