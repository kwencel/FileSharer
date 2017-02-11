#include <File.h>
#include <easylogging++.h>
#include <boost/filesystem.hpp>
#include <define.h>

File::File(const std::string &name) {
    this->name = name;
    // Check if the file already exists
    if (boost::filesystem::exists(name)) {
        fileStream.open(name, std::ios::in | std::ios::out | std::ios::binary);
        this->size = getRealSize();
        LOG(INFO) << "File " << name << " with size " << size << "B opened";
        // Check if the corresponding .meta file exists
        if (boost::filesystem::exists(name + ".meta")) {
            // Partially downloaded file detected. Verify its condition.
            verify();
        } else {
            LOG(INFO) << "Metadata for " << name << " does not exist";
            createMeta();
        }
    } else {
        throw std::runtime_error("File does not exists");
    }
}

File::File(const std::string &name, uintmax_t size) {
    this->size = size;
    fileStream.open(name, std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc);
    createChunks();
    LOG(INFO) << "File " << name << " with size " << size << "B created";
    // TODO Preallocate file
}

bool File::verify() {
    createChunks();
    return true;
}

void File::createMeta() {
    createChunks();
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

uintmax_t File::getRealSize() {
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
    uintmax_t from = chunk->getId() * CHUNK_SIZE;
    writeBytes(from, howMany, chunk->getData());
}

std::vector<char> File::readChunkData(unsigned long chunkId) {
    Chunk *chunk = chunks[chunkId];
    return readChunkData(chunk);
}

std::vector<char> File::readChunkData(Chunk *chunk) {
    unsigned howMany = chunk->getRealSize();
    uintmax_t from = chunk->getId() * CHUNK_SIZE;
    return readBytes(from, howMany);
}

unsigned long File::getChunksAmount() const {
    return chunks.size();
}

const std::vector<Chunk *> File::getChunks() const {
    return chunks;
}

std::vector<char> File::readBytes(uintmax_t from, uintmax_t howMany) {
    fileStream.seekg(from);
    std::vector<char> buffer(howMany);
    fileStream.read(buffer.data(), howMany);
    return buffer;
}

bool File::writeBytes(uintmax_t from, uintmax_t howMany, std::vector<char> buffer) {
    fileStream.seekg(from);
    fileStream.write(buffer.data(), howMany);
    return true;
}

std::string File::getName() const {
    return name;
}

std::string File::getHash() const {
    return hash;
}

uintmax_t File::getSize() const {
    return size;
}
