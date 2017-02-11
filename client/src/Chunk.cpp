#include "Chunk.h"

Chunk::Chunk(unsigned long long int id, unsigned int realSize, File *associatedFile) :
        id(id), realSize(realSize), associatedFile(associatedFile) {

}

unsigned int Chunk::getRealSize() const {
    return realSize;
}

std::vector<char> &Chunk::getData() {
    if (data.empty()) {
        // TODO Check if file has this data
        data = associatedFile->readChunk(id);
    }
    return data;
}

void Chunk::setData(std::vector<char> data) {
    this->data = data;
    associatedFile->notifyChunkDownloaded(id);
}
