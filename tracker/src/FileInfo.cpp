#include "FileInfo.h"

FileInfo::FileInfo(const std::string filename, const std::string hash, std::vector<bool> availableChunks) {
    this->filename = filename;
    this->hash = hash;
    this->availableChunks = availableChunks;
}

bool FileInfo::compareHash(std::string hashToCompare) {
    return (this->hash == hashToCompare);
}

std::string FileInfo::getFilename() const {
    return this->filename;
}

std::string FileInfo::getHash() const {
    return this->hash;
}

std::vector<bool> FileInfo::getAvailableChunks() const {
    return this->availableChunks;
}


