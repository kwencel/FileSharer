#ifndef FILESHARER_FILE_H
#define FILESHARER_FILE_H

#include <string>
#include <vector>
#include <fstream>
#include <AbstractFileHandler.h>
#include <array>
#include <Chunk.h>
#include <define.h>

class Chunk;

class File {
    public:
        /**
         * Constructs the File given the path. Reads file contents from disk.
         * @param name Path to the the file
         */
        File(const std::string &name);

        /**
         * Constructs the File with a given name and size overwriting the existing file.
         * @param name Path to the file
         * @param size Size of the file
         */
        File(const std::string &name, uintmax_t size);

        /**
         * Performs a (possibly deferred) write of a downloaded chunk.
         * @param chunkId ID of the downloaded chunk
         */
        void notifyChunkDownloaded(unsigned long long int chunkId);

        std::vector<char> readChunk(unsigned long long int chunkId);

        bool verify();

        void createMeta();

        void createChunks();

        unsigned long int getChunksAmount() const;

        const std::vector<Chunk *> &getChunks() const;

    private:
        std::string name;
        std::string hash;
        uintmax_t size;
        std::vector<Chunk*> chunks;
        std::fstream fileStream;
        AbstractFileHandler *fileHandler;

        uintmax_t getRealSize();

        std::vector<char> readBytes(uintmax_t from, uintmax_t howMany);

        bool writeBytes(uintmax_t from, uintmax_t howMany, std::vector<char> buffer);
};


#endif //FILESHARER_FILE_H
