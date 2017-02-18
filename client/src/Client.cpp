#include "MainWindow.h"
#include <QApplication>
#include <easylogging++.h>
#include <File.h>
#include <boost/filesystem/operations.hpp>
#include <ConnectionManager.h>

INITIALIZE_EASYLOGGINGPP

int main(int argc, char *argv[]) {
    File sinewave("example_file.flac");

    File kopia("kopia_chunked.flac", sinewave.getRealSize(), sinewave.getHash(), sinewave.getChunksHashes());

    std::vector<Chunk *> kopiaChunks = kopia.getChunks();

    for (unsigned long i = 0; i < kopia.getChunksAmount(); ++i) {
        kopiaChunks[i]->setData(sinewave.readChunkData(i));
    }

    std::cout << QT_VERSION_STR;
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return 0;
}