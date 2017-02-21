#ifndef FILESHARER_CLIENTPROTOCOLTRANSLATOR_H
#define FILESHARER_CLIENTPROTOCOLTRANSLATOR_H

#include <string>
#include <Chunk.h>
#include <SerializationHelper.h>

namespace ClientProtocolTranslator {
        /**
         * Generate message to be sent by client
         * @tparam T Type of object to serialize
         * @param header Message header
         * @param object Object to be serialized
         * @return Generated message string
         */
        template<typename T>
        std::string generateMessage(char header, T object) {
                std::string message = SerializationHelper::serialize<T>(header, object);
                return message;
        }

        /**
         * Decode message sent by client
         * @tparam T Type of object to deserialize
         * @param message Message to be deserialized
         * @return Object of type T obtained from message
         */
        template<typename T>
        T decodeMessage(std::string message) {
                T object = SerializationHelper::deserialize<T>(message);
                return object;
        }
};

#endif //FILESHARER_CLIENTPROTOCOLTRANSLATOR_H
