#ifndef FILESHARER_CHUNK_H
#define FILESHARER_CHUNK_H


#include <fstream>
#include <File.h>

class File;

class Chunk {
    public:
        /**
         * Constructs the Chunk from already downloaded file
         * @param id ID of the created chunk
         * @param realSize The real size of the data chunk will be storing
         * @param associatedFile File that chunk concerns
         */
        Chunk(unsigned long id, unsigned realSize, File *associatedFile);

        /**
         * Constructs the Chunk for to-be-downloaded file
         * @param id ID of the created chunk
         * @param realSize The real size of the data chunk will be storing
         * @param associatedFile File that chunk concerns
         * @param hash MD5 hash of the chunk
         */
        Chunk(unsigned long id, unsigned realSize, File *associatedFile, std::string hash);

        /**
         * Returns the chunk's MD5 hash. If its not cached in @a hash field it will be calculated by accessing the disk.
         * @return Hash of the chunk
         */
        const std::string getHash();

        /**
         * Returns the real chunk size (last chunk will probably have a @a realSize that is smaller than the chunk size.
         * @return Real size of the data contained within the chunk
         */
        unsigned getRealSize() const;

        /**
         * Returns the chunk's ID
         * @return Chunk ID
         */
        unsigned long getId() const;

        /**
         * Returns the data contained within the chunk. If its not cached in @a data field it will be fetched from
         * disk.
         * @return Data contained within the chunk
         */
        // TODO Implement caching mechanism (lazy initialization, remove from cache if not used recently / RAM overused
        std::vector<char> &getData();

        /**
         * Saves the data to chunks buffer and mark chunk as downloaded and ready to flush to disk.
         * Data is not saved if its hash does not match with the expected Chunk hash.
         * @param data Data to be saved as chunk content
         * @return True if hash of saved data matches the expected hash passed in the constructor
         * @throw ChunkSizeMismatchError If @a data has a different size from the expected one in the @a realSize field
         * @throw ChunkHashMismatchError If @a data has a different hash from the expected one in the @a hash field
         */
        void setData(std::vector<char> data);

        /**
         * @return True if the chunk's data was downloaded
         */
        bool isDownloaded() const;



    private:
        /**
         * ID of the chunk. IDs are assigned chronogically until they cover the whole
         * file they are associated with.
         */
        unsigned long id;

        /**
         * Real size could be smaller than chunk size if the chunk is the last one
         * and the data does not fill the chunk in full.
         */
        unsigned realSize;

        /**
         * MD5 checksum of the chunk.
         */
        std::string hash;

        /**
         * Pointer to file associated with the chunk.
         */
        File *associatedFile; // TODO Replace with Observer pattern

        /**
         * Vector containing actual chunk data. It acts as a buffer, if the data
         * has been written do disk and no peers are interested in downloading
         * the chunk data it might be cleared to save system memory.
         */
        std::vector<char> data;

        /**
         * Indicates if the chunk contains the valid data.
         */
        bool downloaded = false;

        /**
         * Calculates the MD5 hash of the data provided in a @a buffer
         * @param buffer A buffer storing the data to be calculated hash for
         * @return MD5 hash calculated for the data provided in the @a buffer
         */
        std::string calculateHashMD5(std::vector<char> &buffer);
};


#endif //FILESHARER_CHUNK_H
