#include <File.h>
#include <easylogging++.h>
#include <boost/filesystem.hpp>
#include <openssl/md5.h>
#include <CustomExceptions.h>

File::File(const std::string &name) {
    this->name = name;
    // Check if the file already exists
    if (boost::filesystem::exists(name)) {
        fileStream.open(name, std::ios::in | std::ios::out | std::ios::binary);
        // Check if the corresponding .meta file exists
        if (boost::filesystem::exists(name + ".meta")) {
            // Partially downloaded file detected. Verify its condition and rebuild chunks info from .meta file
            verify();
        } else {
            // Assuming the file is fully downloaded and ready to be shared
            LOG(INFO) << "Metadata for " << name << " does not exist";
            size = getRealSize();
            createChunks();
            hash = calculateHashMD5(0, size);
        }
        LOG(INFO) << "File " << name << " with size " << size << "B opened";
//        fileHandler = std::make_unique<FileHandler>(this);
    } else {
        throw FileNotFoundError(name);
    }
}

//File::File(const std::string &name, unsigned long size, std::string fileHash, std::vector<std::string> chunksHashes) {
//    this->name = name;
//    this->size = size;
//    this->hash = fileHash;
//    fileHandler = std::make_unique<FileHandler>(this);
////    fileHandler // TODO Download chunksHashes;
//    fileStream.open(name, std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc);
//    createChunks(chunksHashes);
//    createMeta(chunksHashes);
//    LOG(INFO) << "File " << name << " with size " << size << "B created";
//    // TODO Preallocate file
//
//}

File::File(FileInfo fileInfo, std::vector<std::string> chunksHashes) {
    this->name = fileInfo.getFilename();
    this->size = fileInfo.getSize();
    this->hash = fileInfo.getHash();
    fileStream.open(name, std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc);
    createChunks(chunksHashes);
    createMeta(chunksHashes);
    LOG(INFO) << "File " << name << " with size " << size << "B created";
    // TODO Preallocate file
}


void File::verify() {
    std::ifstream meta;
    meta.open(name + ".meta", std::ios::in);
    meta >> hash;
    meta >> size;
    unsigned long chunksAmount = 1 + ((size - 1) / CHUNK_SIZE);
    std::vector<std::string> chunksHashes;
    chunksHashes.reserve(chunksAmount);
    for (unsigned long i = 0; i < chunksAmount; ++i) {
        std::string chunkHash;
        meta >> chunkHash;
        chunksHashes.push_back(chunkHash);
    }
    createChunks(chunksHashes);
}

void File::createMeta(std::vector<std::string> &chunksHashes) {
    std::ofstream meta;
    meta.open(name + ".meta", std::ios::out | std::ios::trunc);
    meta << hash << std::endl;
    meta << size << std::endl;
    for (unsigned long i = 0; i < chunksHashes.size(); ++i) {
        meta << chunksHashes[i] << std::endl;
    }
}

void File::createChunks() {
    // (size / CHUNK_SIZE) division rounded up
    unsigned long chunksAmount = 1 + ((size - 1) / CHUNK_SIZE);
    unsigned remainder = (unsigned) (size % CHUNK_SIZE);
    chunks.reserve(chunksAmount);
    unsigned long id;
    for (id = 0; id < chunksAmount - 1; ++id) {
        chunks.push_back(new Chunk(id, CHUNK_SIZE, this));
    }
    chunks.push_back(new Chunk(id, remainder, this));
}

void File::createChunks(std::vector<std::string> &chunksHashes) {
    unsigned long chunksAmount = 1 + ((size - 1) / CHUNK_SIZE);
    unsigned remainder = (unsigned) (size % CHUNK_SIZE);
    chunks.reserve(chunksAmount);
    unsigned long id;
    for (id = 0; id < chunksAmount - 1; ++id) {
        chunks.push_back(new Chunk(id, CHUNK_SIZE, this, chunksHashes[id]));
    }
    chunks.push_back(new Chunk(id, remainder, this, chunksHashes[id]));
}

unsigned long File::getRealSize() {
    return boost::filesystem::file_size(name);
}

void File::notifyChunkDownloaded(unsigned long chunkId) {
    // TODO Implement lazy saving (waiting for neighbouring chunks and saving them in one or few sequential writes)
    writeChunkData(chunkId);
}

void File::writeChunkData(unsigned long chunkId) {
    Chunk *chunk = chunks[chunkId];
    writeChunkData(chunk);
}

void File::writeChunkData(Chunk *chunk) {
    unsigned howMany = chunk->getRealSize();
    unsigned long from = chunk->getId() * CHUNK_SIZE;
    writeBytes(from, howMany, chunk->getData());
}

std::vector<char> File::readChunkData(unsigned long chunkId) {
    Chunk *chunk = chunks[chunkId];
    return readChunkData(chunk);
}

std::vector<char> File::readChunkData(Chunk *chunk) {
    unsigned howMany = chunk->getRealSize();
    unsigned long from = chunk->getId() * CHUNK_SIZE;
    return readBytes(from, howMany);
}

unsigned long File::getChunksAmount() const {
    return chunks.size();
}

const std::vector<Chunk *> File::getChunks() const {
    return chunks;
}

std::vector<char> File::readBytes(unsigned long from, unsigned long howMany) {
    fileStream.clear(fileStream.eofbit);
    fileStream.seekg(from);
    std::vector<char> buffer(howMany);
    fileStream.read(buffer.data(), howMany);
    std::streamsize bytes = fileStream.gcount();
    if (bytes < howMany) {
        buffer.resize((unsigned long) bytes);
    }
    return buffer;
}

bool File::writeBytes(unsigned long from, unsigned long howMany, std::vector<char> buffer) {
    fileStream.clear(fileStream.eofbit);
    fileStream.seekg(from);
    fileStream.write(buffer.data(), howMany);
    return fileStream.good();
}

std::string File::getName() const {
    return name;
}

std::string File::getHash() const {
    return hash;
}

unsigned long File::getSize() const {
    return size;
}

std::string File::calculateHashMD5(unsigned long from, unsigned long howMany) {
    MD5_CTX mdContext;
    MD5_Init(&mdContext);
    unsigned char c[MD5_DIGEST_LENGTH];

    std::vector<char> buffer;
    unsigned long endPos = from + howMany;

    do {
        buffer = readBytes(from, MD5_BUFFER_SIZE);
        from += buffer.size();
        MD5_Update(&mdContext, buffer.data(), buffer.size());
    } while (from < endPos || buffer.size() > 0);
    MD5_Final(c, &mdContext);

    std::stringstream ss;
    for (int i = 0; i < MD5_DIGEST_LENGTH; ++i) {
        ss << std::hex << std::setfill('0') << std::setw(2) << (unsigned short) c[i];
    }
    return ss.str();
}

std::vector<std::string> File::getChunksHashes() {
    std::vector<std::string> chunksHashes;
    chunksHashes.reserve(chunks.size());
    for (auto &&chunk : chunks) {
        chunksHashes.push_back(chunk->getHash());
    }
    return chunksHashes;
}

File::~File() {
    fileStream.close();
    for (auto &&chunk : chunks) {
        delete chunk;
    }
}

std::vector<bool> File::getDownloadedChunks() {
    std::vector<bool> remainingChunks(getChunksAmount(), false);
    for (int i = 0; i < getChunksAmount(); ++i) {
        if (chunks[i]->isDownloaded()) {
            remainingChunks[i] = true;
        }
    }
    return remainingChunks;
}

FileInfo File::getFileInfo() {
    return FileInfo(getName(), getHash(), getDownloadedChunks());
}
