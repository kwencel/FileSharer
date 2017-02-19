#ifndef FILESHARER_CLIENTPROTOCOLTRANSLATOR_H
#define FILESHARER_CLIENTPROTOCOLTRANSLATOR_H

#include <string>
#include <Chunk.h>
#include <SerializationHelper.h>

namespace ClientProtocolTranslator {
        template<typename T>
        std::string ClientProtocolTranslator::generateMessage(char header, T object) {
            std::string message = SerializationHelper::serialize<T>(header, object);
            return message;
        }

        template<typename T>
        T ClientProtocolTranslator::decodeMessage(std::string message) {
            T object = SerializationHelper::deserialize<T>(message);
            return object;
        }
};

#endif //FILESHARER_CLIENTPROTOCOLTRANSLATOR_H
