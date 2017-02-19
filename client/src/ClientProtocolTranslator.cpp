#include <ClientProtocolTranslator.h>

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