#include "FileInfo.h"

#include <utility>

FileInfo::FileInfo(std::string filename, std::string hash, unsigned long size, std::vector<bool> availableChunks, unsigned long numberOfPeers) {
    this->filename = std::move(filename);
    this->hash = std::move(hash);
    this->size = size;
    this->numberOfPeers = numberOfPeers;
    this->availableChunks = std::move(availableChunks);
}

FileInfo::FileInfo() {

}

std::string FileInfo::printChunks() {
    std::string chunks;
    for (bool b : availableChunks) {
        chunks += std::to_string(b);
    }
    return chunks;
}

bool FileInfo::compareHash(const std::string &hashToCompare) {
    return (this->hash == hashToCompare);
}

std::string FileInfo::getName() const {
    return this->filename;
}

std::string FileInfo::getHash() const {
    return this->hash;
}

uint64_t FileInfo::getSize() const {
    return this->size;
}


std::vector<bool> FileInfo::getAvailableChunks() const {
    return this->availableChunks;
}

void FileInfo::setNumberOfPeers(unsigned long numberOfPeers) {
    this->numberOfPeers = numberOfPeers;
}

unsigned long FileInfo::getNumberOfPeers() const {
    return numberOfPeers;
}



