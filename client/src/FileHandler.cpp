#include <iostream>
#include <ConnectionManager.h>
#include <ProtocolUtils.h>
#include <easylogging++.h>
#include <ClientProtocolTranslator.h>
#include <bitset>
#include <TrackerHandler.h>
#include <QtCore/QMetaObject>
#include <boost/filesystem/operations.hpp>

FileHandler::FileHandler(std::string name) {
    file = std::make_unique<File>(name);
}

FileHandler::FileHandler(FileInfo fileInfo) {
    this->fileInfo = fileInfo;
    char header = PROTOCOL_HEADER_PEERS_WITH_FILE;
    std::string hash = fileInfo.getHash();

    // Get information about peers having this file from tracker
    peersWithFile = TrackerHandler::getPeersWithFileByHash(hash);

    if (boost::filesystem::exists(FILES_PATH_PREFIX + fileInfo.getName() + ".meta")) {
        // File partially downloaded - read expected chunks hashes from .meta file and verify files integrity
        file = std::make_unique<File>(fileInfo.getName());
    } else {
        // File does not exists on disk even partially. Download expected chunks hashes from any peer having the file
        PeerFile peer = peersWithFile[0];
        std::shared_ptr<Connection> peerConnection = establishConnection(peer.getIp(), peer.getPort(), true);
        peerConnection.get()->write(ProtocolUtils::encodeHeader(PROTOCOL_PEER_REQUEST_HASHES));
        std::string response = peerConnection.get()->read(9);
        header = ProtocolUtils::decodeHeader(response.substr(0, 1));
        uint64_t size = ProtocolUtils::decodeSize(response.substr(1, 8));
        response = peerConnection.get()->read(size);

        std::vector<std::string> hashes = ClientProtocolTranslator::decodeMessage<std::vector<std::string>>(response);

        // Finally call the File constructor
        file = std::make_unique<File>(fileInfo, hashes);
        peerConnection.get()->registerObserver(this);
    }
}

void FileHandler::update(Connection* connection) {
    LOG(INFO) << "Message from " << connection->getPeerIPandPort();
    char header = connection->read(1)[0];
    switch (header) {
        case PROTOCOL_PEER_REQUEST_CHUNK: {
            uint64_t chunkId = ProtocolUtils::decodeSize(connection->read(8));
            LOG(INFO) << connection->getPeerIPandPort() << " requested a chunk " << chunkId << " of file " << file.get()->name;
            sendChunk(connection->getPeerIP(), connection->getPeerPort(), chunkId);
            break;
        }
        case PROTOCOL_PEER_REQUEST_HASHES: {
            LOG(INFO) << "Peer requested a vector of chunks hashes of file " << file.get()->name;
            sendChunksHashes(connection->getPeerIP(), connection->getPeerPort());
            break;
        }
        case PROTOCOL_PEER_SEND_CHUNK: {
            uint64_t chunkId = receiveChunk(connection);
            LOG(INFO) << connection->getPeerIPandPort() << " sent a chunk " << chunkId << " data";
            updateProgress();
            break;
        }
        case PROTOCOL_PEER_CONNECTION_CLOSE: {
            LOG(INFO) << "Received connection close";
            connections.erase(connection->getPeerIP() + ":" + std::to_string(connection->getPeerPort()));
            ConnectionManager::getInstance().removeConnection(connection);
            break;
        }
        default:
            LOG(ERROR) << "Unknown header " << std::bitset<8>(static_cast<unsigned long long>(header));
            break;
    }
}

void FileHandler::download() {

}

std::shared_ptr<Connection> FileHandler::establishConnection(std::string peerIP, uint16_t peerPort, bool dontRegister) {
    std::shared_ptr<Connection> conn;
    // Find if there is already a connection with this peer
    auto possibleConnection = connections.find(peerIP + ":" + std::to_string(peerPort));
    if (possibleConnection != connections.end()) {
        // Connection already established
        conn = (*possibleConnection).second;
    } else {
        conn = ConnectionManager::getInstance().requestConnection(peerIP, peerPort);
        if (initializeCommunication(conn)) {
            connections.insert({peerIP + ":" + std::to_string(peerPort), conn});
            if (!dontRegister) {
                conn.get()->registerObserver(this);
            }
        } else {
            LOG(ERROR) << "Peer " << peerIP << ":" << peerPort << " did not send ACK on communication initialization!";
        }
    }
    return conn;
}

void FileHandler::addConnection(std::shared_ptr<Connection> connection) {
    connections.insert({connection.get()->getPeerIP() + ":" + std::to_string(connection.get()->getPeerPort()), connection});
    connection.get()->registerObserver(this);
}

bool FileHandler::initializeCommunication(std::shared_ptr<Connection> connection) {
    connection.get()->write(ProtocolUtils::encodeHeader(PROTOCOL_PEER_INIT_HASH) + fileInfo.getHash());
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
    std::string message = ClientProtocolTranslator::generateMessage<std::vector<char>>(header[0], chunk->getData());
    establishConnection(peerIP, peerPort).get()->write(message + ProtocolUtils::encodeSize(chunkId));
}

uint64_t FileHandler::receiveChunk(Connection *connection) {
    uint64_t serializedVectorSize = ProtocolUtils::decodeSize(connection->read(8));
    std::string response = connection->read(serializedVectorSize);
    std::vector<char> chunkData = ClientProtocolTranslator::decodeMessage<std::vector<char>>(response);
    uint64_t chunkId = ProtocolUtils::decodeSize(connection->read(8));
    file.get()->chunks[chunkId]->setData(chunkData);
    return chunkId;
}

void FileHandler::beginDownload() {
    std::vector<uint64_t> peerLoads(peersWithFile.size(), 0);
    for (uint64_t id = 0; id < file->getChunksAmount(); ++id) {
        if (!file->getChunks()[id]->isDownloaded()) {
            std::vector<uint64_t> loads;
            std::vector<uint64_t> indexes;
            for (uint64_t peerIndex = 0; peerIndex < peersWithFile.size(); ++peerIndex) {
                if (peersWithFile[peerIndex].getAvailableChunks()[id]) {
                    indexes.push_back(peerIndex);
                    loads.push_back(peerLoads[peerIndex]);
                }
            }
            if (loads.size() != 0) {
                long lowestLoadPosition = std::distance(loads.begin(), std::min_element(loads.begin(), loads.end()));
                long lowestLoadPeerIndex = indexes[lowestLoadPosition];
                requestChunk(peersWithFile[lowestLoadPeerIndex].getIp(), peersWithFile[lowestLoadPeerIndex].getPort(),
                             id);
                peerLoads[lowestLoadPeerIndex]++;
            }
        }
    }
}

void FileHandler::updateProgress() {
    emit updateFileHandlerProgress(this);
}
