#ifndef FILESHARER_SERIALIZATIONHELPER_H
#define FILESHARER_SERIALIZATIONHELPER_H

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/unordered_set.hpp>
#include <boost/serialization/utility.hpp>
#include <ProtocolUtils.h>

namespace SerializationHelper {

    /**
     * Serializes a message consisting of a header and an object. Automatically inserts
     * serialized object size after the header (as string).
     * @tparam T Type of object to be serialized
     * @param header Header of the message
     * @param object Instance of class T
     * @return String that represents serialized data (header-size-object)
     */
    template<typename T>
    static std::string serialize(char header, T object) {
        std::string headerAsString = ProtocolUtils::encodeHeader(header);
        std::ostringstream archive_stream;
        {
            boost::archive::text_oarchive archive(archive_stream);
            archive << object;
        }
        uint64_t serializedStringSize = (uint64_t) archive_stream.str().size();
        std::string sizeAsString = ProtocolUtils::encodeSize(serializedStringSize);
        //LOG(DEBUG) << "Header + size size: " + std::to_string(headerAsString.size() + sizeAsString.size());
        //LOG(DEBUG) << "Object size " + std::to_string(serializedStringSize);

        return (headerAsString + sizeAsString + archive_stream.str());
    }

    /**
     * Deserializes an object from a given string
     * @tparam T Type of object to be deserialized
     * @param serializedObject String to be deserialized
     * @return Returns an instance of class T obtained from deserialization
     */
    template<typename T>
    static T deserialize(std::string &serializedObject) {
        T deserializedObject;
        std::stringstream archive_stream;
        archive_stream << serializedObject;
        {
            boost::archive::text_iarchive archive(archive_stream);
            archive >> deserializedObject;
        }
        return deserializedObject;
    }
}

#endif //FILESHARER_SERIALIZATIONHELPER_H
