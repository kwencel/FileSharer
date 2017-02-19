#include <iostream>
#include <ConnectionManager.h>
#include <ProtocolUtils.h>
#include <easylogging++.h>
#include <ClientProtocolTranslator.h>
#include <PeerFile.h>
#include <bitset>

FileHandler::FileHandler(File *file) : file(file) { }

FileHandler::FileHandler(std::string name) {
    file = std::make_unique<File>(name);
}

FileHandler::FileHandler(FileInfo fileInfo) {
    this->fileInfo = fileInfo;
    char header = PROTOCOL_HEADER_PEERS_WITH_FILE;
    std::string hash = fileInfo.getHash();

    // Get information about peers having this file from tracker
    Connection trackerConnection(TRACKER_BIND_IP, TRACKER_BIND_PORT);
    std::string peersWithFileRequest = ClientProtocolTranslator::generateMessage<std::string>(header, hash);
    trackerConnection.write(peersWithFileRequest);
    std::string response = trackerConnection.read(9);
    header = ProtocolUtils::decodeHeader(response.substr(0, 1));
    uint64_t size = ProtocolUtils::decodeSize(response.substr(1, 8));
    response = trackerConnection.read(size);
    peersWithFile = ClientProtocolTranslator::decodeMessage<std::vector<PeerFile>>(response);

    // Ask first peer about chunks hashes
    PeerFile peer = peersWithFile[0];
    std::string peerIP = peer.getIp();
    uint16_t peerPort = peer.getPort();
    std::shared_ptr<Connection> peerConnection = establishConnection(peerIP, peerPort);
    peerConnection.get()->write(ProtocolUtils::encodeHeader(PROTOCOL_PEER_REQUEST_HASHES));
    response = peerConnection.get()->read(9);
    header = ProtocolUtils::decodeHeader(response.substr(0, 1));
    size =  ProtocolUtils::decodeSize(response.substr(1, 8));
    response = peerConnection.get()->read(size);

    std::vector<std::string> hashes = ClientProtocolTranslator::decodeMessage<std::vector<std::string>>(response);

    // Finally call the File constructor
    file = std::make_unique<File>(fileInfo, hashes);
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
            uint64_t chunkId = ProtocolUtils::decodeSize(connection->read(8));
            uint64_t size = ProtocolUtils::decodeSize(connection->read(8));
            std::vector<char> chunkData = receiveChunk(connection, size);
            file.get()->chunks[chunkId]->setData(chunkData);
            break;
        }
        default:
            LOG(ERROR) << "Unknown header " << std::bitset<8>(static_cast<unsigned long long>(header));
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
                            fileInfo.getHash());
    std::string response = connection.get()->read(1);
    return ProtocolUtils::decodeHeader(response) == PROTOCOL_PEER_INIT_ACK;
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
                                                       ProtocolUtils::encodeSize(chunkId) +
                                                       ProtocolUtils::encodeSize(chunkSize) +
                                                       message);
}

std::vector<char> FileHandler::receiveChunk(Connection *connection, uint64_t size) {
    std::string chunkData = connection->read(size);
    return std::vector<char>(chunkData.begin(), chunkData.end());
}

void FileHandler::startDownload(std::vector<PeerFile> peersWithFile) {
    // TODO Algorithm to choose which chunk to download from which peer
    for (uint64_t id = 0; id < file->getChunksAmount(); ++id) {
        requestChunk(peersWithFile[0].getIp(), peersWithFile[0].getPort(), id);
    }
}