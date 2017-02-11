#ifndef FILESHARER_CHUNK_H
#define FILESHARER_CHUNK_H


#include <fstream>
#include <File.h>

class File;

class Chunk {
    public:
        /**
         * Constructs the Chunk
         * @param id ID of the created chunk
         * @param realSize The real size of the data chunk will be storing
         * @param associatedFile File that chunk concerns
         */
        Chunk(unsigned long id, unsigned realSize, File *associatedFile);

        /**
         * Returns the real chunk size (last chunk will probably have a realSize that is smaller than the chunk size.
         * @return Real size of the data contained within the chunk
         */
        unsigned getRealSize() const;

        /**
         * Returns the chunk's ID
         * @return Chunk ID
         */
        unsigned long getId() const;

        /**
         * Return the data contained within the chunk. If its not cached in data field it will be fetched from
         * disk.
         * @return Data contained within the chunk
         */
        // TODO Implement caching mechanism (lazy initialization, remove from cache if not used recently / RAM overused
        std::vector<char> &getData();

        /**
         * Saves the data to chunks buffer and mark chunk as downloaded and ready to flush to disk
         * @param data Data to be saved as chunk content
         */
         // TODO Verify hash before saving
        void setData(std::vector<char> data);

        /**
         * Indicates if the chunk contains the valid data.
         */
        bool downloaded = false;

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
};


#endif //FILESHARER_CHUNK_H
