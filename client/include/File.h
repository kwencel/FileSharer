#ifndef FILESHARER_FILE_H
#define FILESHARER_FILE_H

#include <string>
#include <vector>
#include <fstream>
#include <array>
#include <Chunk.h>
#include <Define.h>
#include <FileInfo.h>
#include "FileHandler.h"

class Chunk;
class FileHandler;

class File {
    friend class FileHandler;
    public:
        /**
         * Constructs the File given the path. Reads file contents from disk.
         * @param name Path to the the file
         * @throw FileNotFoundError if file with given name does not exists
         */
        File(const std::string &name);

        /**
         * Constructs the to-be-downloaded File with a given name and size overwriting the existing file.
         * @param name Path to the file
         * @param size Size of the file
         * @param chunksHashes Vector of target chunk hashes
         */
        File(FileInfo fileInfo, std::vector<std::string> chunksHashes);

        ~File();

        /**
         * @return Amount of chunks the file was divided to.
         */
        unsigned long getChunksAmount() const;

        /**
         * @return Vector of chunks the file was divided to.
         */
        const std::vector<Chunk *> getChunks() const;

        /**
         * @return Name of the file.
         */
        std::string getName() const;

        /**
         * @return Name of the file.
         */
        std::string getRelativePath() const;

        /**
         * @return MD5 hash of the file.
         */
        std::string getHash() const;

        /**
         * @return Target size of the file.
         */
        unsigned long getSize() const;

        /**
         * @return Real size of the file (could be smaller than the target size because the file grows up while it's
         *         being downloaded
         */
        unsigned long getRealSize();

        /**
         * Notifies the file that one of its chunk got downloaded and is ready to be written to disk
         * @param chunkId ID of the downloaded chunk
         */
        void notifyChunkDownloaded(unsigned long chunkId);

        /**
         * Reads the part of file that is associated with the given chunk.
         * @param chunkId ID of the chunk that is going to be read
         * @return Data of the given chunk represented by a vector of chars
         */
        std::vector<char> readChunkData(unsigned long chunkId);

        /**
         * Reads the part of file that is associated with the given chunk.
         * @param chunk Pointer to chunk that is going to be read
         * @return Data of the given chunk represented by a vector of chars
         */
        std::vector<char> readChunkData(Chunk *chunk);

        /**
         * @return Vector of chunks hashes
         */
        std::vector<std::string> getChunksHashes();

        std::vector<bool> getDownloadedChunks();

        bool isDownloaded() const;

        unsigned long getDownloadedChunksAmount() const;

        FileInfo getFileInfo();

        File(File&&) = default;

    private:
        std::string name;
        std::string relativePath;
        std::string hash;
        uintmax_t size;
        std::vector<Chunk *> chunks;
        std::fstream fileStream;
        unsigned long downloadedChunksAmount = 0;

        /**
         * Reads file data from disk.
         * @param from File pointer offset which marks the beginning of the requested range to read
         * @param howMany Number of bytes to read from the starting offset
         * @return File data read from disk in the requested range represented by a vector of chars
         */
        std::vector<char> readBytes(unsigned long from, unsigned long howMany);

        /**
         * Writes file data to disk.
         * @param from File pointer offset which marks the beginning of the requested range to write
         * @param howMany Number of bytes to write from the starting offset
         * @param buffer File data to be written to disk in the requested range represented by a vector of chars
         * @return True if file stream is in the 'good' state (there was no I/O errors and EOF was not reached)
         */
        bool writeBytes(unsigned long from, unsigned long howMany, std::vector<char> buffer);

        /**
         * Writes the contents of a given chunk to disk.
         * @param chunkId ID of the chunk that is going to be written
         */
        void writeChunkData(unsigned long chunkId);

        /**
         * Writes the contents of a given chunk to disk.
         * @param chunk Pointer to the chunk that is going to be written
         */
        void writeChunkData(Chunk *chunk);

        /**
         * Calculates the MD5 hash of the file in the given range of bytes.
         * @param from File pointer offset which marks the beginning of the requested range to hash
         * @param howMany Number of bytes from the 'file' offset which marks the end of requested range of bytes to hash
         * @return MD5 hash calculated for the given range of bytes
         */
        std::string calculateHashMD5(unsigned long from, unsigned long howMany);

        /**
         * Creates a vector of chunks using by accessing a fully downloaded file and calculating the hashes.
         */
        void createChunks();

        /**
         * Creates a vector of chunks using the vector of their expected hashes supplied in the argument.
         * @param chunksHashes Vector storing the chunks hashes
         */
        void createChunks(std::vector<std::string> &chunksHashes);

        /**
         * Creates a .meta file containing expected chunks hashes to be able to resume operation after
         * the program is closed.
         * @param chunksHashes Vector storing the chunks hashes
         */
        void createMeta(std::vector<std::string> &chunksHashes);

        /**
         * Verifies the partially downloaded file condition and restores the vector of Chunk based on the .meta file.
         */
        void verify();

        void checkMemoryLimit();
};


#endif //FILESHARER_FILE_H
