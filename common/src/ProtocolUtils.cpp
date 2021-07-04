#include <ProtocolUtils.h>

std::string ProtocolUtils::encodeHeader(char number) {
    return std::string(&number, 1);
}

std::string ProtocolUtils::encodeSize(uint64_t number) {
    return std::string((char*) &number, 8);
}

char ProtocolUtils::decodeHeader(const std::string &encodedNumber) {
    return (*(char *)(encodedNumber.c_str()));
}

uint64_t ProtocolUtils::decodeSize(const std::string &encodedNumber) {
    return (*(uint64_t *)(encodedNumber.c_str()));
}
