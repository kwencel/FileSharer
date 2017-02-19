#ifndef FILESHARER_PROTOCOLUTILS_H
#define FILESHARER_PROTOCOLUTILS_H

#include <string>

namespace ProtocolUtils {
    std::string encodeHeader(char number);

    std::string encodeSize(uint64_t number);

    char decodeHeader(std::string encodedNumber);

    uint64_t decodeSize(std::string encodedNumber);
}

#endif //FILESHARER_PROTOCOLUTILS_H
