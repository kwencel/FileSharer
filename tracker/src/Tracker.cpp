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

    sockaddr_in serverSocket;
    socklen_t socketSize = sizeof(serverSocket);
    size_t bufferSize = 1024;
    char buffer[bufferSize];
    memset(&serverSocket, '\0', socketSize);
    memset(&buffer, '\0', bufferSize);

    serverSocket.sin_family = AF_INET;
    serverSocket.sin_port = htons(TRACKER_BIND_PORT);
    serverSocket.sin_addr.s_addr = inet_addr(TRACKER_BIND_IP);
    int serverSocketDescriptor = socket(AF_INET, SOCK_STREAM, 0);

    bind(serverSocketDescriptor, (struct sockaddr *) &serverSocket, socketSize);
    listen(serverSocketDescriptor, 10);

    while (1) {
        int clientSocketDescriptor = accept(serverSocketDescriptor, (struct sockaddr *) &serverSocket, &socketSize);
        int enable = 1;
        setsockopt(clientSocketDescriptor, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
        Connection conn = Connection(clientSocketDescriptor, serverSocket);
        std::string headerAndSize = conn.read(9);
        char header = ProtocolUtils::decodeHeader(headerAndSize.substr(0, 1));
        LOG(DEBUG) << "Header: " + std::to_string(header);
        uint64_t size = ProtocolUtils::decodeSize(headerAndSize.substr(1, 8));
        LOG(DEBUG) << "Size: " + std::to_string(size);
        std::string message = conn.read(size);
        std::string response = protocolTranslator.generateResponse(header, message, conn);
        conn.write(response);
    }

    return 0;
}