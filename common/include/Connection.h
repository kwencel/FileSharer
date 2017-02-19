#ifndef FILESHARER_CONNECTION_H
#define FILESHARER_CONNECTION_H


#include <netinet/in.h>
#include <string>
#include <vector>
#include <Define.h>
#include <forward_list>
#include "../../client/include/Observer.h"
//TODO change CMakeLists to include this with just the header name

class ConnectionManager;
//class Observer;

class Connection {
    friend class ConnectionManager;
    public:
        /**
         * Constructs an outgoing connection
         * @param peerIP IP address of the peer
         * @param peerPort IP adress of the peer
         */
        Connection(std::string peerIP, uint16_t peerPort);

        /**
         * Constructs an incoming connection
         * @param peerSocketDescriptor Descriptor of peer's socket used for communication
         * @param peerSocket Structure containing information about peer's IP address and port
         */
        Connection(int peerSocketDescriptor, sockaddr_in peerSocket);

        /**
         * Closes the connection and destructs the object
         */
        ~Connection();

        /**
         * @return Peer's IPv4 address
         */
        std::string getPeerIP();

        /**
         * @return Peer's port
         */
        uint16_t getPeerPort();

        /**
         * Sends given data to peer
         * @param data Information to be sent
         * @return Number of bytes that was sent
         */
        ssize_t write(std::string data);

        /**
         * Receives data from peer
         * @param howMany Number of bytes to be read
         * @return Received information
         */
        std::string read(size_t howMany = RECEIVE_BUFFER);

        void notify();

        void registerObserver(Observer *observer);

        void unregisterObserver(Observer *observer);

    private:
        /**
         * Structure containing information about peer's IP address and port
         */
        sockaddr_in peerSocket;

        /**
         * Descriptor of peer's socket used for communication
         */
        int peerSocketDescriptor;

        std::forward_list<class Observer *> observers;
};


#endif //FILESHARER_CONNECTION_H