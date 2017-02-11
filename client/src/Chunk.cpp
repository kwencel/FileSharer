#include "Chunk.h"

Chunk::Chunk(unsigned long id, unsigned realSize, File *associatedFile) :
        id(id), realSize(realSize), associatedFile(associatedFile) {

}

unsigned Chunk::getRealSize() const {
    return realSize;
}

std::vector<char> &Chunk::getData() {
    if (data.empty()) {
        // TODO Check if file has this data
        data = associatedFile->readChunkData(id);
    }
    return data;
}

void Chunk::setData(std::vector<char> data) {
    this->data = data;
    associatedFile->notifyChunkDownloaded(id);
}

unsigned long Chunk::getId() const {
    return id;
}
