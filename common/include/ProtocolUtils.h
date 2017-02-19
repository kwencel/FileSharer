#ifndef FILESHARER_PROTOCOLUTILS_H
#define FILESHARER_PROTOCOLUTILS_H

#include <string>

namespace ProtocolUtils {
    std::string encodeHeader(char number) {
        return std::string(&number, 1);
    }

    std::string encodeSize(uint64_t number) {
        return std::string((char*) &number, 8);
    }

    char decodeHeader(std::string encodedNumber) {
        return (*(char *)(encodedNumber.c_str()));
    }

    uint64_t decodeSize(std::string encodedNumber) {
        return (*(uint64_t *)(encodedNumber.c_str()));
    }
}

#endif //FILESHARER_PROTOCOLUTILS_H
