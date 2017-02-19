#include <iostream>
#include <ConnectionManager.h>
#include <ProtocolUtils.h>
#include <easylogging++.h>
#include <ClientProtocolTranslator.h>

FileHandler::FileHandler(File *file) : file(file) { }

FileHandler::FileHandler(FileInfo fileInfo) {
    // Get info about peers that has the file
    std::string peerIP;
    uint16_t peerPort;
    std::vector<bool> downloadedChunks;
    std::tuple<std::string, uint16_t, std::vector<bool>> peerInfo();
    // HASHE!
    requestChunksHashes(peerIP, peerPort);
}



void FileHandler::update(Connection* connection) {
    LOG(INFO) << "Message from" << connection->getPeerIP() << ":" << connection->getPeerPort() << std::endl;
    char header = connection->read(1)[0];
    switch (header) {
        case PROTOCOL_PEER_REQUEST_CHUNK: {
            uint64_t chunkId = ProtocolUtils::decodeSize(connection->read(8));
            sendChunk(connection->getPeerIP(), connection->getPeerPort(), chunkId);
            break;
        }
        case PROTOCOL_PEER_REQUEST_HASHES: {
            sendChunksHashes(connection->getPeerIP(), connection->getPeerPort());
            break;
        }
        case PROTOCOL_PEER_SEND_CHUNK: {

            // TODO receive std::vector<char>, deserialize, invoke .setData() on appropriate chunk
        }
        case PROTOCOL_PEER_SEND_HASHES: {
            // TODO ??? Only needed in File's constructor
        }
        default:
            LOG(ERROR) << "Unknown header";
            break;
    }
}

void FileHandler::download() {

}

std::shared_ptr<Connection> FileHandler::establishConnection(std::string peerIP, uint16_t peerPort) {
    std::shared_ptr<Connection> conn;
    // Find if there is already a connection with this peer
    auto possibleConnection = connections.find(peerIP);
    if (possibleConnection != connections.end()) {
        // Connection already established
        conn = (*possibleConnection).second;
    } else {
        conn = ConnectionManager::getInstance().requestConnection(peerIP, peerPort);
        if (initializeCommunication(conn)) {
            connections.insert({peerIP, conn});
            conn.get()->registerObserver(this);
        } else {
            LOG(ERROR) << "Peer " << peerIP << ":" << peerPort << " did not send ACK on communication initialization!";
        }
    }
    return conn;
}

void FileHandler::addConnection(std::shared_ptr<Connection> connection) {
    connections.insert({connection.get()->getPeerIP(), connection});
    connection.get()->registerObserver(this);
    connection.get()->write(ProtocolUtils::encodeHeader(PROTOCOL_PEER_INIT_ACK));
}

bool FileHandler::initializeCommunication(std::shared_ptr<Connection> connection) {
    connection.get()->write(ProtocolUtils::encodeHeader(PROTOCOL_PEER_INIT_HASH) +
                            ProtocolUtils::encodeSize(32) +
                            file->getHash());
    std::string response = connection.get()->read(1);
    return ProtocolUtils::decodeHeader(response) == PROTOCOL_PEER_INIT_ACK;
}

std::vector<std::string> FileHandler::requestChunksHashes(std::string peerIP, uint16_t peerPort) {
    establishConnection(peerIP, peerPort).get()->write(ProtocolUtils::encodeHeader(PROTOCOL_PEER_REQUEST_HASHES));

}

void FileHandler::sendChunksHashes(std::string peerIP, uint16_t peerPort) {
    std::string header = ProtocolUtils::encodeHeader(PROTOCOL_PEER_SEND_HASHES);
    std::string message = ClientProtocolTranslator::generateMessage<std::vector<std::string>>(header[0], file->getChunksHashes());
    establishConnection(peerIP, peerPort).get()->write(message);
}

void FileHandler::requestChunk(std::string peerIP, uint16_t peerPort, uint64_t chunkId) {
    establishConnection(peerIP, peerPort).get()->write(ProtocolUtils::encodeHeader(PROTOCOL_PEER_REQUEST_CHUNK) +
                                                       ProtocolUtils::encodeSize(chunkId));
}

void FileHandler::sendChunk(std::string peerIP, uint16_t peerPort, uint64_t chunkId) {
    std::string header = ProtocolUtils::encodeHeader(PROTOCOL_PEER_SEND_CHUNK);
    Chunk* chunk = file->getChunks()[chunkId];
    unsigned chunkSize = chunk->getRealSize();
    std::string message(chunk->getData().data(), 0, chunkSize);
    establishConnection(peerIP, peerPort).get()->write(ProtocolUtils::encodeHeader(PROTOCOL_PEER_SEND_CHUNK) +
                                                       ProtocolUtils::encodeSize(chunkSize) +
                                                       message);
}

void FileHandler::startDownload(std::vector<std::pair<std::string, std::vector<bool>>> peers) {
    std::vector<bool> remainingChunks = file->getRemainingChunks();
    // TODO Algorithm to choose which chunk to download from which peer
    for (uint64_t id = 0; id < file->getChunksAmount(); ++id) {
        requestChunk(peers[id].first, CLIENT_BIND_PORT, id);
    }
}