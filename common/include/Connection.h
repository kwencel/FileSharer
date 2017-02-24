#ifndef FILESHARER_CONNECTION_H
#define FILESHARER_CONNECTION_H


#include <netinet/in.h>
#include <string>
#include <vector>
#include <Define.h>
#include <forward_list>
#include "../../client/include/Observer.h"

class ConnectionManager;

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
         * @return Returns Peer's IP and port in pretty format IP:Port
         */
        std::string getPeerIPandPort();

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
        std::string read(size_t howMany = RECEIVE_BUFFER, time_t timeout = DEFAULT_TIMEOUT);

        /**
         * Notifies observers by calling update on them
         */
        void notify();

        /**
         * Registers an observer
         * @param observer Observer to be registered
         */
        void registerObserver(Observer *observer);

        /**
         * Unregister an observer
         * @param observer Observer to be unregistered
         */
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

        /**
         * List of observers of an instance of this class
         */
        std::forward_list<class Observer *> observers;
};


#endif //FILESHARER_CONNECTION_H
