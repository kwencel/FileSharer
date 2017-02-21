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
        FileInfo(const std::string filename, const std::string hash, unsigned long size, std::vector<bool> availableChunks, unsigned long numberOfPeers = 0);
        FileInfo();

        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            ar & filename;
            ar & hash;
            ar & size;
            ar & numberOfPeers;
            ar & availableChunks;
        }

        /**
         * @return String for printing availableChunks vector
         */
        std::string printChunks();

        /**
         * Compares file's hash to the one given as the argument
         * @param hashToCompare Hash to compare against file hash
         * @return Boolean result of the comparison between file hash and argument hash
         */
        bool compareHash(std::string hashToCompare);

        /**
         * @return Name of the file
         */
        std::string getName() const;

        /**
         * @return Hash of the file
         */
        std::string getHash() const;
        
        /**
         * @return Size of the file 
         */
        unsigned long getSize() const;

        /**
         * @return Vector of bool values denoting which chunks of the file are available
         */
         std::vector<bool> getAvailableChunks() const;

        /**
         * @return Number of peers that have this file
         */
        unsigned long getNumberOfPeers() const;

        /**
         * Sets number of peers
         * @param numberOfPeers
         */
        void setNumberOfPeers(unsigned long numberOfPeers);

    private:
        std::string filename;
        std::string hash;
        unsigned long size;
        unsigned long numberOfPeers;
        std::vector<bool> availableChunks;
};

#endif //FILESHARER_FILEINFO_H
