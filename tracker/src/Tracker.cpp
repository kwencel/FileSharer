#include <easylogging++.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "TrackerProtocolTranslator.h"
#include <boost/serialization/vector.hpp>
#include <ProtocolUtils.h>
#include <ErrorCheckUtils.h>

INITIALIZE_EASYLOGGINGPP

int main(int argc, char *argv[]) {

    if (argc < 3) {
        std::cerr << "FileSharer Tracker - simple P2P file-sharing program." << std::endl;
        std::cerr << "Usage: " << argv[0] << " <tracker_bind_ip> <tracker_bind_port>" << std::endl;
        return -1;
    }

    std::string trackerBindIP = argv[1];
    uint16_t trackerBindPort = static_cast<uint16_t>(atoi(argv[2]));

    TrackerProtocolTranslator protocolTranslator;

    sockaddr_in serverSocket;
    socklen_t socketSize = sizeof(serverSocket);
    size_t bufferSize = 1024;
    char buffer[bufferSize];
    memset(&serverSocket, '\0', socketSize);
    memset(&buffer, '\0', bufferSize);

    serverSocket.sin_family = AF_INET;
    serverSocket.sin_port = htons(trackerBindPort);
    serverSocket.sin_addr.s_addr = inet_addr(trackerBindIP.c_str());
    int serverSocketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocketDescriptor == -1) {
        perror("Error during socket creation");
    }

    CHK_EX(bind(serverSocketDescriptor, (struct sockaddr *) &serverSocket, socketSize));
    CHK_EX(listen(serverSocketDescriptor, 5));

    while (1) {
        int clientSocketDescriptor = accept(serverSocketDescriptor, (struct sockaddr *) &serverSocket, &socketSize);
        int enable = 1;
        setsockopt(clientSocketDescriptor, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
        Connection conn = Connection(clientSocketDescriptor, serverSocket);
        LOG(INFO) << "Connected peer: " << conn.getPeerIP() << ":" << conn.getPeerPort();
        std::string headerAndSize = conn.read(9);
        char header = ProtocolUtils::decodeHeader(headerAndSize.substr(0, 1));
        LOG(INFO) << "Header: " + std::to_string(header);
        uint64_t size = ProtocolUtils::decodeSize(headerAndSize.substr(1, 8));
        LOG(INFO) << "Size: " + std::to_string(size);
        std::string message = conn.read(size);
        std::string response = protocolTranslator.generateResponse(header, message, conn);
        conn.write(response);
    }

    return 0;
}