#ifndef FILESHARER_ABSTRACTFILEHANDLER_H
#define FILESHARER_ABSTRACTFILEHANDLER_H


#include <fstream>

class AbstractFileHandler {
        virtual ~AbstractFileHandler() {}
        std::fstream *fileStream;
        virtual void requestChunk(unsigned long long id) = 0;
};


#endif //FILESHARER_ABSTRACTFILEHANDLER_H
