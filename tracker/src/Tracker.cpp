#include <easylogging++.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <define.h>
#include "ProtocolTranslator.h"
#include "ErrorCheckUtils.h"
#include "Peer.h"
#include "FileInfo.h"
#include <boost/serialization/vector.hpp>

INITIALIZE_EASYLOGGINGPP

int main() {
    ProtocolTranslator protocolTranslator;

    FileInfo testFileInfo("file", "HASH", std::vector<bool>{1,0,0,1,0,0,1,0,0,1});
    FileInfo testFileInfo2("file", "HASH", std::vector<bool>{0,1,1,0,1,1,0,1,1,0});
    std::vector<FileInfo> testVector;
    testVector.push_back(testFileInfo);
    testVector.clear();
    testVector.push_back(testFileInfo2);
    Peer testPeer("1.2.3.4", 1024, testVector);
    Peer testPeer2("2.3.4.5", 1024, testVector);
    protocolTranslator.addPeer(testPeer);
    protocolTranslator.addPeer(testPeer2);

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
        //serialization test
        std::stringstream archive_stream;
        archive_stream << response;
        boost::archive::text_iarchive archive(archive_stream);
        Peer deserializedPeer;

        archive >> deserializedPeer;
        deserializedPeer.printSerializedInfo();
        archive >> deserializedPeer;
        deserializedPeer.printSerializedInfo();

        //end test
        send(clientSocket, response.c_str(), strlen(response.c_str()), MSG_PEEK);
        close(clientSocket);
    }

    return 0;
}