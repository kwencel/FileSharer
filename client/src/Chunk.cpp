#include "Chunk.h"
#include <CustomExceptions.h>
#include <openssl/md5.h>

Chunk::Chunk(unsigned long id, unsigned realSize, File *associatedFile) :
        id(id), realSize(realSize), associatedFile(associatedFile), downloaded(true) { };

Chunk::Chunk(unsigned long id, unsigned realSize, File *associatedFile, const std::string &hash) :
        id(id), realSize(realSize), hash(hash), associatedFile(associatedFile) {
    std::vector<char> tmpData;
    tmpData = associatedFile->readChunkData(this);
    if (calculateHashMD5(tmpData) == hash) {
        downloaded = true;
    }
}

unsigned Chunk::getRealSize() const {
    return realSize;
}

std::vector<char> &Chunk::getData() {
    if (data.empty()) {
        if (not downloaded) {
            throw std::runtime_error("Chunk " + std::to_string(id) + "is not on disk");
        }
        data = associatedFile->readChunkData(id);
    }
    return data;
}

void Chunk::setData(const std::vector<char> &data) {
    if (data.size() != realSize) {
        throw ChunkSizeMismatchError(realSize, data.size());
    }
    std::string calculatedHash = calculateHashMD5(data);
    if (getHash() != calculatedHash) {
        throw ChunkHashMismatchError(hash, calculatedHash);
    }
    this->data = data;
    this->downloaded = true;
    associatedFile->notifyChunkDownloaded(id);
}

unsigned long Chunk::getId() const {
    return id;
}

std::string Chunk::getHash() {
    if (hash.empty()) {
        hash = calculateHashMD5(getData());
    }
    return hash;
}

bool Chunk::isDownloaded() const {
    return downloaded;
}

std::string Chunk::calculateHashMD5(const std::vector<char> &buffer) {
    MD5_CTX mdContext;
    MD5_Init(&mdContext);
    unsigned char c[MD5_DIGEST_LENGTH];

    MD5_Update(&mdContext, buffer.data(), buffer.size());
    MD5_Final(c, &mdContext);

    std::stringstream ss;
    for (unsigned char i : c) {
        ss << std::hex << std::setfill('0') << std::setw(2) << (unsigned short) i;
    }
    return ss.str();
}

bool Chunk::isCached() const {
    return !data.empty();
}

void Chunk::clearCache() {
    data.clear();
    data.shrink_to_fit();
}

