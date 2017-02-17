#include <easylogging++.h>
#include <File.h>
#include <boost/filesystem/operations.hpp>
#include <ConnectionManager.h>

INITIALIZE_EASYLOGGINGPP

int main() {
    File sinewave("example_file.flac");

    File kopia("kopia_chunked.flac", sinewave.getRealSize(), sinewave.getHash(), sinewave.getChunksHashes());

    std::vector<Chunk *> kopiaChunks = kopia.getChunks();

    for (unsigned long i = 0; i < kopia.getChunksAmount(); ++i) {
        kopiaChunks[i]->setData(sinewave.readChunkData(i));
    }

    std::cout << sinewave.getHash();

    Connection connection("127.0.0.1", 2048);
    connection.send(sinewave.getHash());
    connection.send("test");

    return 0;
}