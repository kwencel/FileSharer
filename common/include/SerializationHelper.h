#ifndef FILESHARER_SERIALIZATIONHELPER_H
#define FILESHARER_SERIALIZATIONHELPER_H

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/unordered_set.hpp>

namespace SerializationHelper {
    template<typename T>
    static std::string serialize(T object) {
        std::stringstream archive_stream;
        boost::archive::text_oarchive archive(archive_stream);
        archive << object;
        return archive_stream.str();
    }

    template<typename T>
    static T deserialize(std::string &serializedObject) {
        std::stringstream archive_stream;
        archive_stream << serializedObject;
        boost::archive::text_iarchive archive(archive_stream);
        T deserializedObject;
        archive >> deserializedObject;
    }
}

#endif //FILESHARER_SERIALIZATIONHELPER_H
