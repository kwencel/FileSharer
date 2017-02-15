#ifndef FILESHARER_FILEINFO_H
#define FILESHARER_FILEINFO_H

#include <string>
#include <vector>

class FileInfo {
    public:
        /**
         * Constructs an instance of FileInfo class given filename, hash and a vector listing which chunks
         * of the file are available.
         * @param name Name of the file
         * @param hash Hash of the file
         * @param availableChunks Vector of bool values denoting which chunks of the file are available
         */
        FileInfo(const std::string filename, const std::string hash, std::vector<bool> availableChunks);

        /**
         * Compares file's hash to the one given as the argument
         * @param hashToCompare Hash to compare against file hash
         * @return Boolean result of the comparison between file hash and argument hash
         */
        bool compareHash(std::string hashToCompare);

        /**
         * @return Name of the file
         */
        std::string getFilename() const;

        /**
         * @return Hash of the file
         */
        std::string getHash() const;

        /**
         * @return Vector of bool values denoting which chunks of the file are available
         */
         std::vector<bool> getAvailableChunks() const;

    private:
        std::string filename;
        std::string hash;
        std::vector<bool> availableChunks;
};

#endif //FILESHARER_FILEINFO_H
