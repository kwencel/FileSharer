#include "MainWindow.h"
#include <QApplication>
#include <easylogging++.h>
#include <boost/filesystem/operations.hpp>
#include <SerializationHelper.h>

INITIALIZE_EASYLOGGINGPP

int main(int argc, char *argv[]) {

    if (argc < 3) {
        std::cerr << "FileSharer Client - simple P2P file-sharing program." << std::endl;
        std::cerr << "Usage: " << argv[0] << " <tracker_public_ip> <tracker_port> <client_bind_ip> <client_bind_port>" << std::endl;
        return -1;
    }

    std::string clientBindIP = argv[3];
    uint16_t clientBindPort = static_cast<uint16_t>(atoi(argv[4]));

    std::string trackerIP = argv[1];
    uint16_t trackerPort = static_cast<uint16_t>(atoi(argv[2]));

    ConnectionManager::getInstance(clientBindIP, clientBindPort);
    ConnectionManager::getInstance().setTrackerDetails(trackerIP, trackerPort);

    std::cout << QT_VERSION_STR;
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}