#include <iostream>
#include <ConnectionManager.h>
#include "FileHandler.h"



void FileHandler::update(Connection* connection) {
    std::cout << "Coś przyszło od " << connection->getPeerIP() << ":" << connection->getPeerPort() << std::endl;
}

void FileHandler::download() {

}

std::string encodeNumber(uint64_t number) {
    return std::string((char *)(&number), sizeof(number));
}

uint64_t decodeNumber(std::string encoded) {
    return *((uint64_t *)(encoded.c_str()));
}

FileHandler::FileHandler(File *file) : file(file) { }

std::shared_ptr<Connection> FileHandler::establishConnection(std::string peerIP, uint16_t peerPort) {
    std::shared_ptr<Connection> conn;
    // Find if there is already a connection with this peer
    auto possibleConnection = connections.find(peerIP);
    if (possibleConnection != connections.end()) {
        // Connection already established
        conn = (*possibleConnection).second;
    } else {
        conn = ConnectionManager::getInstance().requestConnection(peerIP, peerPort);
        conn.get()->registerObserver(this);
        connections.insert({peerIP, conn});
    }
    return conn;
}

void FileHandler::requestChunk(uint64_t id, std::string peerIP) {
    establishConnection(peerIP, CLIENT_BIND_PORT).get()->write(std::to_string(PROTOCOL_PEER_REQUEST_CHUNK) +
                                                               encodeNumber(id));
}

void FileHandler::startDownload(std::vector<std::pair<std::string, std::vector<bool>>> peers) {
    std::vector<bool> remainingChunks = file->getRemainingChunks();
    // TODO Algorithm to choose which chunk to download from which peer

}
