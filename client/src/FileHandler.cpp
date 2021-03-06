#include <iostream>
#include <ConnectionManager.h>
#include <ProtocolUtils.h>
#include <easylogging++.h>
#include <ClientProtocolTranslator.h>
#include <bitset>
#include <TrackerHandler.h>
#include <boost/filesystem/operations.hpp>

FileHandler::FileHandler(const std::string &name) {
    file = std::make_unique<File>(name);
}

FileHandler::FileHandler(const FileInfo &fileInfo) {
    this->fileInfo = fileInfo;
    std::string hash = fileInfo.getHash();

    // Get information about peers having this file from tracker
    peersWithFile = TrackerHandler::getPeersWithFileByHash(hash);

    if (boost::filesystem::exists(FILES_PATH_PREFIX + fileInfo.getName() + ".meta")) {
        // File partially downloaded - read expected chunks hashes from .meta file and verify files integrity
        file = std::make_unique<File>(fileInfo.getName());
    } else {
        // File does not exists on disk even partially. Download expected chunks hashes from any peer having the file
        std::shared_ptr<Connection> peerConnection;
        unsigned peerNumber = 0;
        PeerFile peer;
        std::vector<std::string> hashes;
        bool success = false;

        while (!success) {
            peer = peersWithFile[peerNumber];
            try {
                peerConnection = establishConnection(peer.getIp(), peer.getPort(), true);
                peerConnection->write(ProtocolUtils::encodeHeader(PROTOCOL_PEER_REQUEST_HASHES));
                std::string response = peerConnection->read(9);
                ProtocolUtils::decodeHeader(response.substr(0, 1));
                uint64_t size = ProtocolUtils::decodeSize(response.substr(1, 8));
                response = peerConnection->read(size);
                hashes = ClientProtocolTranslator::decodeMessage<std::vector<std::string>>(response);
                success = true;
            } catch (const std::exception &e) {
                LOG(WARNING) << "Communication with peer " << peer.getIp() << ":" << peer.getPort()
                             << " failed. Trying the next peer";
                if (peerNumber == peersWithFile.size() - 1) {
                    LOG(ERROR) << "This peer was the last one having the file. Communication with all peers failed. Cannot download the file.";
                    throw std::runtime_error("Unable to connect to any peer having the file");
                } else {
                    ++peerNumber;
                }
            }
        }

        // Finally call the File constructor
        file = std::make_unique<File>(fileInfo, hashes);
        peerConnection->registerObserver(this);
    }
}

void FileHandler::update(Connection *connection) {
    LOG(INFO) << "Message from " << connection->getPeerIPandPort();
    char header = connection->read(1)[0];
    switch (header) {
        case PROTOCOL_PEER_REQUEST_CHUNK: {
            uint64_t chunkId = ProtocolUtils::decodeSize(connection->read(8));
            LOG(INFO) << connection->getPeerIPandPort() << " requested a chunk " << chunkId << " of file " << file->name;
            sendChunk(connection->getPeerIP(), connection->getPeerPort(), chunkId);
            break;
        }
        case PROTOCOL_PEER_REQUEST_HASHES: {
            LOG(INFO) << "Peer requested a vector of chunks hashes of file " << file->name;
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
            LOG(INFO) << "Received connection close.";
            break;
        }
        default:
            LOG(ERROR) << "Unknown header " << std::bitset<8>(static_cast<unsigned long long>(header));
            break;
    }
}

std::shared_ptr<Connection> FileHandler::establishConnection(const std::string &peerIP, uint16_t peerPort, bool dontRegister) {
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
                conn->registerObserver(this);
            }
        } else {
            LOG(ERROR) << "Peer " << peerIP << ":" << peerPort << " did not send ACK on communication initialization!";
        }
    }
    return conn;
}

void FileHandler::addConnection(const std::shared_ptr<Connection> &connection) {
    connections.insert({connection->getPeerIPandPort(), connection});
    connection->registerObserver(this);
}

void FileHandler::stopObserving(Connection *connection) {
    connection->unregisterObserver(this);
    connections.erase(connection->getPeerIPandPort());
}

bool FileHandler::initializeCommunication(const std::shared_ptr<Connection> &connection) {
    connection->write(ProtocolUtils::encodeHeader(PROTOCOL_PEER_INIT_HASH) + fileInfo.getHash());
    std::string response = connection->read(1);
    return ProtocolUtils::decodeHeader(response) == PROTOCOL_PEER_INIT_ACK;
}

void FileHandler::sendChunksHashes(const std::string &peerIP, uint16_t peerPort) {
    std::string header = ProtocolUtils::encodeHeader(PROTOCOL_PEER_SEND_HASHES);
    std::string message = ClientProtocolTranslator::generateMessage<std::vector<std::string>>(header[0], file->getChunksHashes());
    establishConnection(peerIP, peerPort)->write(message);
}

void FileHandler::requestChunk(const std::string &peerIP, uint16_t peerPort, uint64_t chunkId) {
    establishConnection(peerIP, peerPort)->write(ProtocolUtils::encodeHeader(PROTOCOL_PEER_REQUEST_CHUNK) +
                                                       ProtocolUtils::encodeSize(chunkId));
}

void FileHandler::sendChunk(const std::string &peerIP, uint16_t peerPort, uint64_t chunkId) {
    std::string header = ProtocolUtils::encodeHeader(PROTOCOL_PEER_SEND_CHUNK);
    Chunk* chunk = file->getChunks()[chunkId];
    std::string message = ClientProtocolTranslator::generateMessage<std::vector<char>>(header[0], chunk->getData());
    establishConnection(peerIP, peerPort)->write(message + ProtocolUtils::encodeSize(chunkId));
}

uint64_t FileHandler::receiveChunk(Connection *connection) const {
    uint64_t serializedVectorSize = ProtocolUtils::decodeSize(connection->read(8));
    std::string response = connection->read(serializedVectorSize);
    std::vector<char> chunkData = ClientProtocolTranslator::decodeMessage<std::vector<char>>(response);
    uint64_t chunkId = ProtocolUtils::decodeSize(connection->read(8));
    file->chunks[chunkId]->setData(chunkData);
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
            if (!loads.empty()) {
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
