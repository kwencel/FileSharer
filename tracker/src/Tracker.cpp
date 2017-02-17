#include <easylogging++.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <Define.h>
#include "ProtocolTranslator.h"
#include "ErrorCheckUtils.h"
#include <boost/serialization/vector.hpp>

INITIALIZE_EASYLOGGINGPP

int main() {
    ProtocolTranslator protocolTranslator;

    struct sockaddr_in address;
    socklen_t addressSize = sizeof(address);
    size_t bufferSize = 128;
    char buffer[bufferSize];
    memset(&address, '\0', addressSize);
    memset(&buffer, '\0', bufferSize);

    int serverSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    address.sin_family = AF_INET;
    address.sin_port = htons(TRACKER_BIND_PORT);
    address.sin_addr.s_addr = TRACKER_BIND_IP;

    bind(serverSocket, (struct sockaddr *) &address, addressSize);
    listen(serverSocket, 10);

    while (1) {
        int clientSocket = accept(serverSocket, (struct sockaddr *) &address, &addressSize);
        CHK_POS(read(clientSocket, buffer, bufferSize));
        std::string response = protocolTranslator.generateResponse(buffer);
        send(clientSocket, response.c_str(), strlen(response.c_str()), MSG_PEEK);
        close(clientSocket);
    }

    return 0;
}