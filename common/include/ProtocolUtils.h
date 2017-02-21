#ifndef FILESHARER_PROTOCOLUTILS_H
#define FILESHARER_PROTOCOLUTILS_H

#include <string>

namespace ProtocolUtils {
    /**
     * Helper function for encoding char as string
     * @param number Char value to be encoded
     * @return String representation of input char
     */
    std::string encodeHeader(char number);

    /**
     * Helper function for encoding uint64_t as string
     * @param number Uint64_t value to be encoded
     * @return String representation of input uint64_t
     */
    std::string encodeSize(uint64_t number);

    /**
     * Helper function for decoding a char from string
     * @param encodedNumber String to be decoded
     * @return Decoded char value
     */
    char decodeHeader(std::string encodedNumber);

    /**
     * Helper function for decoding a uint64_t from string
     * @param encodedNumber String to be decoded
     * @return Decoded uint64_t value
     */
    uint64_t decodeSize(std::string encodedNumber);
}

#endif //FILESHARER_PROTOCOLUTILS_H
