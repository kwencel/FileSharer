#ifndef FILESHARER_CLIENTPROTOCOLTRANSLATOR_H
#define FILESHARER_CLIENTPROTOCOLTRANSLATOR_H

#include <string>
#include <Chunk.h>
#include <SerializationHelper.h>

namespace ClientProtocolTranslator {
        template<typename T>
        std::string generateMessage(char header, T object) {
                std::string message = SerializationHelper::serialize<T>(header, object);
                return message;
        }

        template<typename T>
        T decodeMessage(std::string message) {
                T object = SerializationHelper::deserialize<T>(message);
                return object;
        }



//    Przykład uzycia dla prośby o chunka

//    header = PROTOCOL_PEER_REQUEST_CHUNK;
//    std::string message = ClientProtocolTranslator::generateMessage<uint64_t>(header, chunkId);
//    conn->get().write(message);



//
//    Przykład użycia dla otrzymania prośby o hashe i odesłania hashów

//    std::string first9bytes = connection.get()->read(9);
//    char header = ProtocolUtils::decodeHeader(first9bytes.substr(0,1));
//    uint64_t size = ProtocolUtils::decodeSize(first9bytes.substr(1,8));
//    std::string message = connection.get()->read(size);
//
//    ...
//    if(header == PROTOCOL_PEER_REQUEST_HASHES) {
//        header = PROTOCOL_PEER_SEND_HASHES;
//        std::string hash = ClientProtocolTranslator::decodeMessage<std::string>(message);
//        file <- znajdz plik o danym hashu
//        std::string response = ClientProtocolTranslator::generateMessage<std::vector<std::string>>(header,
//                                                                                               file.getChunksHashes());
//        conn->get().write(message);
//    }


};

#endif //FILESHARER_CLIENTPROTOCOLTRANSLATOR_H
