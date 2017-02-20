#include <easylogging++.h>
#include "TrackerProtocolTranslator.h"
#include "SerializationHelper.h"
#include "Define.h"

TrackerProtocolTranslator::TrackerProtocolTranslator() {

}

std::string TrackerProtocolTranslator::generateResponse(char header, std::string message) {
    if (header == PROTOCOL_HEADER_REGISTER) {
        Peer newPeer = SerializationHelper::deserialize<Peer>(message);
        if (peerManager.addPeer(newPeer)) {
            LOG(INFO) << "Peer registered! IP: " + newPeer.getIp() + ", number of files: " +
                         std::to_string(newPeer.getFileList().size());
            LOG(INFO) << "Files:";
            for (FileInfo fi : newPeer.getFileList()) {
                LOG(INFO) << "Name: " + fi.getName() + ", hash: " + fi.getHash() + ", chunks: " + fi.printChunks();
            }
            header = PROTOCOL_HEADER_REGISTER_ACCEPT;
        }
        else {
            LOG(INFO) << "Peer with IP: " + newPeer.getIp() + " already registered!";
            header = PROTOCOL_HEADER_REGISTER_DUPLICATE;
        }
        std::string response = SerializationHelper::serialize<std::string>(header, "");
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


