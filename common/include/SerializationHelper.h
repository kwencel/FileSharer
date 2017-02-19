#ifndef FILESHARER_SERIALIZATIONHELPER_H
#define FILESHARER_SERIALIZATIONHELPER_H

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/unordered_set.hpp>
#include <boost/serialization/utility.hpp>

namespace SerializationHelper {

    /**
     * Serializes an object of type given as template argument to std::string
     * @tparam T Type of object to be serialized
     * @param object Object to be serialized
     * @return Serialized object in the form of std::string
     */
    template<typename T>
    static std::string serialize(char header, T object) {
        std::string headerAsString(&header, 1);
        std::ostringstream archive_stream;
        {
            boost::archive::text_oarchive archive(archive_stream);
            archive << object;
        }
        uint64_t serializedStringSize = (uint64_t) archive_stream.str().size();
        std::string sizeAsString((char*) &serializedStringSize, 8);
        //LOG(DEBUG) << "Header + size size: " + std::to_string(headerAsString.size() + sizeAsString.size());
        //LOG(DEBUG) << "Object size " + std::to_string(serializedStringSize);

        return (headerAsString + sizeAsString + archive_stream.str());
    }

    /**
     * Deserializes an std::string to a object of type given as template argument
     * @tparam T Type of object that will be the result of deserialization
     * @param serializedObject String to deserialize
     * @return Deserialized object of type T
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
