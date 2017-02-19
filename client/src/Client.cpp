#include "MainWindow.h"
#include <QApplication>
#include <easylogging++.h>
#include <File.h>
#include <boost/filesystem/operations.hpp>
#include <ConnectionManager.h>

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
//
//    std::cout << QT_VERSION_STR;
//    QApplication a(argc, argv);
//    MainWindow w;
//    w.show();

    ConnectionManager &cm = ConnectionManager::getInstance("127.0.0.1", 2048);
    cm.listenLoop();
    while(1) {
        for (auto &&connection : cm.getActiveConnections()) {
            std::string response = connection.get()->read(4);
            std::cout << *(uint32_t *)(response.c_str()) << std::endl;
        }

    }

    while(1);

    return 0;
}