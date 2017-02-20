#include "MainWindow.h"
#include <QApplication>
#include <easylogging++.h>
#include <boost/filesystem/operations.hpp>
#include <SerializationHelper.h>

INITIALIZE_EASYLOGGINGPP

int main(int argc, char *argv[]) {
//    File sinewave("example_file.flac");
//
//    File kopia("kopia_chunked.flac", sinewave.getRealSize(), sinewave.getHash(), sinewave.getChunksHashes());
//
//    std::vector<Chunk *> kopiaChunks = kopia.getChunks();
//
//    for (unsigned long i = 0; i < kopia.getChunksAmount(); ++i) {
//        kopiaChunks[i]->setData(sinewave.readChunkData(i));
//    }

    std::string ip = argv[1];
    uint16_t port = (uint16_t) atoi(argv[2]);
    std::cout << port << std::endl;
    ConnectionManager &cm = ConnectionManager::getInstance(ip, port);

    std::cout << QT_VERSION_STR;
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}