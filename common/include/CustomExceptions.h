#ifndef FILESHARER_CUSTOMEXCEPTIONS_H
#define FILESHARER_CUSTOMEXCEPTIONS_H

/**
 * Use this as a template for writing custom runtime errors
 */
class TemplateRuntimeError : public std::runtime_error {
    public:
        TemplateRuntimeError(const std::string& reason) : std::runtime_error(reason) { }
};

/**
 * Exception thrown when chunk has a different size than expected
 */
class ChunkSizeMismatchError : public std::logic_error {
    public:
        ChunkSizeMismatchError(unsigned long chunkSize, unsigned long gotSize) :
                std::logic_error("Got data vector of size " + std::to_string(gotSize) +
                                 "B instead of expected " + std::to_string(chunkSize) + "B") { }
};

/**
 * Exception thrown when downloaded chunk hash is not correct
 */
class ChunkHashMismatchError : public std::runtime_error {
    public:
        ChunkHashMismatchError(std::string chunkHash, std::string gotHash) :
                std::runtime_error("Got data vector hash " + gotHash + " instead of expected " + chunkHash) { }
};


/**
 * Exception thrown when a connection cannot be established
 */
class ConnectionError: public std::runtime_error {
    public:
        ConnectionError(const std::string &reason) : std::runtime_error(reason) {}
};

/**
 * Exception thrown when file <filename> has not been found
 */
class FileNotFoundError : public std::runtime_error {
    public:
        FileNotFoundError(std::string filename) :
                std::runtime_error("File " + filename + " does not exists") { }
};

/**
 * Exception thrown on timeout on socket while using recv()
 */
class ReadTimeoutError : public std::runtime_error {
    public:
        ReadTimeoutError(std::string ip, uint16_t port) :
                std::runtime_error("Read timeout on " + ip + ":" + std::to_string(port)) { }
};


#endif //FILESHARER_CUSTOMEXCEPTIONS_H
