#include <Connection.h>
#include <TrackerHandler.h>
#include <Peer.h>
#include <ClientProtocolTranslator.h>

namespace {
    Connection establishConnectionToTracker() {
        Connection conn(TRACKER_BIND_IP, TRACKER_BIND_PORT);
        return conn;
    }

    std::string getResponseDataString(Connection &conn) {
        std::string headerAndSize = conn.read(9);
        char header = ProtocolUtils::decodeHeader(headerAndSize.substr(0, 1));
        uint64_t restOfMessageSize = ProtocolUtils::decodeSize(headerAndSize.substr(1, 8));
        std::string responseDataString = conn.read(restOfMessageSize);
        return responseDataString;
    }
}

std::string TrackerHandler::registerToTracker(std::string ownIp, uint16_t ownPort, std::vector<FileInfo> fileInfoVector) {
    Connection conn = establishConnectionToTracker();
    Peer localPeer(ownIp, ownPort, fileInfoVector);

    char header = PROTOCOL_HEADER_REGISTER;
    std::string message = ClientProtocolTranslator::generateMessage<Peer>(header, localPeer);
    conn.write(message);
    std::string responseDataString = getResponseDataString(conn);
    return responseDataString;
}

std::vector<FileInfo> TrackerHandler::getAvailableFiles() {
    Connection conn = establishConnectionToTracker();

    char header = PROTOCOL_HEADER_LIST_FILES;
    std::string message = ClientProtocolTranslator::generateMessage<std::string>(header, "");
    conn.write(message);
    std::string responseDataString = getResponseDataString(conn);
    return ClientProtocolTranslator::decodeMessage<std::vector<FileInfo>>(responseDataString);
}

std::vector<PeerFile> TrackerHandler::getPeersWithFileByHash(std::string hash) {
    Connection conn = establishConnectionToTracker();

    char header = PROTOCOL_HEADER_PEERS_WITH_FILE;
    std::string message = ClientProtocolTranslator::generateMessage<std::string>(header, hash);
    conn.write(message);
    std::string responseDataString = getResponseDataString(conn);
    return ClientProtocolTranslator::decodeMessage<std::vector<PeerFile>>(responseDataString);
}