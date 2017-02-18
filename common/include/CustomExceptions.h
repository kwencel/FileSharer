#ifndef FILESHARER_CUSTOMEXCEPTIONS_H
#define FILESHARER_CUSTOMEXCEPTIONS_H

/**
 * Use this as a template for writing custom runtime errors
 */
class TemplateRuntimeError : public std::runtime_error {
    public:
        TemplateRuntimeError(const std::string& reason) : std::runtime_error(reason) { }
};

class ChunkSizeMismatchError : public std::logic_error {
    public:
        ChunkSizeMismatchError(unsigned long chunkSize, unsigned long gotSize) :
                std::logic_error("Got data vector of size " + std::to_string(gotSize) +
                                 "B instead of expected " + std::to_string(chunkSize) + "B") { }
};

class ChunkHashMismatchError : public std::runtime_error {
    public:
        ChunkHashMismatchError(std::string chunkHash, std::string gotHash) :
                std::runtime_error("Got data vector hash " + gotHash + " instead of expected " + chunkHash) { }
};

class ConnectionError: public std::runtime_error {
    public:
        ConnectionError(const std::string &reason) : std::runtime_error(reason) {}
};

class FileNotFoundError : public std::runtime_error {
    public:
        FileNotFoundError(std::string filename) :
                std::runtime_error("File " + filename + " does not exists") { }
};


#endif //FILESHARER_CUSTOMEXCEPTIONS_H
