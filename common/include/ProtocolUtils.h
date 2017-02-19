#ifndef FILESHARER_PROTOCOLUTILS_H
#define FILESHARER_PROTOCOLUTILS_H

namespace ProtocolUtils {
    std::string encodeHeader(char number) {
        return std::string(&number, 1);
    }

    std::string encodeSize(uint64_t number) {
        return std::string((char*) &number, 8);
    }

    char decode(std::string encodedNumber) {
        return (*(char *)(encodedNumber.c_str()));
    }

    uint64_t decode(std::string encodedNumber) {
        return (*(uint64_t *)(encodedNumber.c_str()));
    }
}

#endif //FILESHARER_PROTOCOLUTILS_H
