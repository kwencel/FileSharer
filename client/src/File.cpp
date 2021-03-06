#include <File.h>
#include <easylogging++.h>
#include <boost/filesystem.hpp>
#include <openssl/md5.h>
#include <CustomExceptions.h>

File::File(const std::string &name) {
    std::string baseName = name.substr(name.find_last_of("/\\") + 1);
    this->name = name;
    this->relativePath = FILES_PATH_PREFIX + name;
    // Check if the file already exists
    if (boost::filesystem::exists(relativePath)) {
        fileStream.open(relativePath, std::ios::in | std::ios::out | std::ios::binary);
        // Check if the corresponding .meta file exists
        if (boost::filesystem::exists(relativePath + ".meta")) {
            // Partially downloaded file detected. Verify its condition and rebuild chunks info from .meta file
            verify();
            for (auto &&chunk : chunks) {
                if (chunk->isDownloaded()) {
                    ++downloadedChunksAmount;
                }
            }
        } else {
            // Assuming the file is fully downloaded and ready to be shared
            LOG(INFO) << "Metadata for " << name << " does not exist";
            size = getRealSize();
            if (size == 0) {
                throw std::runtime_error("File size is equal to 0");
            }
            createChunks();
            downloadedChunksAmount = getChunksAmount();
            hash = calculateHashMD5(0, size);
        }
        LOG(INFO) << "File " << name << " with size " << size << "B opened";
    } else {
        throw FileNotFoundError(name);
    }
}

File::File(const FileInfo& fileInfo, std::vector<std::string> chunksHashes) {
    this->name = fileInfo.getName();
    this->relativePath = FILES_PATH_PREFIX + name;
    this->size = fileInfo.getSize();
    this->hash = fileInfo.getHash();
    boost::filesystem::create_directory(FILES_PATH_PREFIX);
    fileStream.open(relativePath, std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc);
    createChunks(chunksHashes);
    for (auto &&chunk : chunks) {
        if (chunk->isDownloaded()) {
            ++downloadedChunksAmount;
        }
    }
    createMeta(chunksHashes);
    LOG(INFO) << "File " << name << " with size " << size << "B created";
}


void File::verify() {
    std::ifstream meta;
    meta.open(relativePath + ".meta", std::ios::in);
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
    meta.open(relativePath + ".meta", std::ios::out | std::ios::trunc);
    meta << hash << std::endl;
    meta << size << std::endl;
    for (auto &chunkHash : chunksHashes) {
        meta << chunkHash << std::endl;
    }
}

void File::createChunks() {
    // (size / CHUNK_SIZE) division rounded up
    unsigned long chunksAmount = 1 + ((size - 1) / CHUNK_SIZE);
    auto remainder = (unsigned) (size % CHUNK_SIZE);
    if (remainder == 0) {
        remainder = CHUNK_SIZE;
    }
    chunks.reserve(chunksAmount);
    unsigned long id;
    for (id = 0; id < chunksAmount - 1; ++id) {
        chunks.push_back(new Chunk(id, CHUNK_SIZE, this));
    }
    chunks.push_back(new Chunk(id, remainder, this));
}

void File::createChunks(std::vector<std::string> &chunksHashes) {
    unsigned long chunksAmount = 1 + ((size - 1) / CHUNK_SIZE);
    auto remainder = (unsigned) (size % CHUNK_SIZE);
    if (remainder == 0) {
        remainder = CHUNK_SIZE;
    }
    chunks.reserve(chunksAmount);
    unsigned long id;
    for (id = 0; id < chunksAmount - 1; ++id) {
        chunks.push_back(new Chunk(id, CHUNK_SIZE, this, chunksHashes[id]));
    }
    chunks.push_back(new Chunk(id, remainder, this, chunksHashes[id]));
}

unsigned long File::getRealSize() {
    return boost::filesystem::file_size(relativePath);
}

void File::notifyChunkDownloaded(unsigned long chunkId) {
    writeChunkData(chunkId);
    ++downloadedChunksAmount;
    if (isDownloaded()) {
        // File is fully downloaded, verify global hash
        if (calculateHashMD5(0, getRealSize()) != hash) {
            throw std::runtime_error("Global file hash mismatch!");
        }
        // File is fully downloaded, .meta files is no longer necessary
        boost::filesystem::remove(relativePath + ".meta");
    }
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

std::vector<Chunk *> File::getChunks() const {
    return chunks;
}

std::vector<char> File::readBytes(unsigned long from, unsigned long howMany) {
    checkMemoryLimit();
    fileStream.clear(std::fstream::eofbit);
    fileStream.seekg(from);
    std::vector<char> buffer(howMany);
    fileStream.read(buffer.data(), howMany);
    std::streamsize bytes = fileStream.gcount();
    if ((unsigned long)bytes < howMany) {
        buffer.resize((unsigned long) bytes);
    }
    return buffer;
}

bool File::writeBytes(unsigned long from, unsigned long howMany, std::vector<char> buffer) {
    checkMemoryLimit();
    fileStream.clear(std::fstream::eofbit);
    fileStream.seekg(from);
    fileStream.write(buffer.data(), howMany);
    return fileStream.good();
}

std::string File::getName() const {
    return name;
}

std::string File::getRelativePath() const {
    return relativePath;
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
    unsigned char digest[MD5_DIGEST_LENGTH];

    std::vector<char> buffer;
    unsigned long endPos = from + howMany;

    do {
        buffer = readBytes(from, MD5_BUFFER_SIZE);
        from += buffer.size();
        MD5_Update(&mdContext, buffer.data(), buffer.size());
    } while (from < endPos || !buffer.empty());
    MD5_Final(digest, &mdContext);

    std::stringstream ss;
    for (unsigned char c : digest) {
        ss << std::hex << std::setfill('0') << std::setw(2) << (unsigned short) c;
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
    for (unsigned int i = 0; i < getChunksAmount(); ++i) {
        if (chunks[i]->isDownloaded()) {
            remainingChunks[i] = true;
        }
    }
    return remainingChunks;
}

FileInfo File::getFileInfo() {
    return FileInfo(getName(), getHash(), getSize(), getDownloadedChunks());
}

bool File::isDownloaded() const {
    return downloadedChunksAmount == getChunksAmount();
}

unsigned long File::getDownloadedChunksAmount() const {
    return downloadedChunksAmount;
}

void File::checkMemoryLimit() {
    unsigned long cachedChunks = 0;
    for (auto &&chunk : chunks) {
        if (chunk->isCached()) {
            ++cachedChunks;
        }
    }
    if (((cachedChunks * CHUNK_SIZE_KB) / 1024) > MEMORY_PER_FILE_LIMIT_MB) {
        for (auto &&chunk : chunks) {
            chunk->clearCache();
        }
        LOG(INFO) << "Cached data of file " << name << " cleared";
    }
}
