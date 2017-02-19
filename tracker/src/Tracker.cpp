#include <easylogging++.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <Define.h>
#include "TrackerProtocolTranslator.h"
#include <boost/serialization/vector.hpp>
#include <ProtocolUtils.h>
#include <Connection.h>

INITIALIZE_EASYLOGGINGPP

int main() {
    TrackerProtocolTranslator protocolTranslator;

    sockaddr_in socket;
    socklen_t socketSize = sizeof(socket);
    size_t bufferSize = 1024;
    char buffer[bufferSize];
    memset(&socket, '\0', socketSize);
    memset(&buffer, '\0', bufferSize);

    socket.sin_family = AF_INET;
    socket.sin_port = htons(TRACKER_BIND_PORT);
    socket.sin_addr.s_addr = inet_addr(TRACKER_BIND_IP);
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    bind(serverSocket, (struct sockaddr *) &socket, socketSize);
    listen(serverSocket, 10);

    while (1) {
        int clientSocketDescriptor = accept(serverSocket, (struct sockaddr *) &socket, &socketSize);
        int enable = 1;
        setsockopt(clientSocketDescriptor, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
        Connection conn = Connection(clientSocketDescriptor, socket);
        std::string headerAndSize = conn.read(9);
        char header = ProtocolUtils::decodeHeader(headerAndSize.substr(0, 1));
        uint64_t size = ProtocolUtils::decodeSize(headerAndSize.substr(1, 8));
        std::string message = conn.read(size);
        std::string response = protocolTranslator.generateResponse(header, message);
        conn.write(response);
    }

    return 0;
}