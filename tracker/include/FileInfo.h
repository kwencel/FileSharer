#ifndef FILESHARER_FILEINFO_H
#define FILESHARER_FILEINFO_H

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
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
        FileInfo();

        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            ar & filename;
            ar & hash;
            ar & availableChunks;
        }

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


struct FileInfoHasher {
    size_t operator() (const FileInfo &fileInfo) const {
        std::string temp = fileInfo.getFilename() + fileInfo.getHash();
        return (temp.length());
    }
};

#endif //FILESHARER_FILEINFO_H
