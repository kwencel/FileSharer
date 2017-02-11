#include <easylogging++.h>
#include <File.h>

INITIALIZE_EASYLOGGINGPP

int main() {

    File sinewave("sinewave_440hz.wav");

    File kopia("kopia_chunked.wav", 48428);

    std::vector<Chunk*> kopiaChunks = kopia.getChunks();

    for (long i = 1; i >= 0; --i) {
        kopiaChunks[i]->setData(sinewave.readChunk(i));
    }


//    // SEEK - FROM BACK
//    // File operations example
//    std::fstream source("sinewave_440hz.wav", std::ios::in | std::ios::out | std::ios::binary);
//    std::fstream dest("kopia.wav", std::ios::in | std::ios::out | std::ios::trunc | std::ios_base::binary);
//
//    source.seekg(0, std::ios::end);
//    std::ifstream::pos_type size = source.tellg();
//
//    source.seekg(0, std::ios::end);
//    dest.seekg(0, std::ios::end);
//
//    // allocate memory for buffer
//    char *buffer = new char[bufsize];
//    std::streamsize readBytes;
//
//    while (source) {
//        source.read(buffer, bufsize);
//        readBytes = source.gcount();
//        dest.write(buffer, readBytes);
//        source.seekg(-bufsize, std::ios::end);
//        dest.seekg(-bufsize, std::ios::end);
//    }
//
//    // clean up
//    delete[] buffer;
//    source.close();
//    dest.close();

//    //WORKING
//    // File operations example
//    std::fstream source("sinewave_440hz.wav", std::ios::in | std::ios::out | std::ios::binary);
//    std::fstream dest("kopia.wav", std::ios::in | std::ios::out | std::ios::trunc | std::ios_base::binary);
//
//    source.seekg(0, std::ios::end);
//    std::ifstream::pos_type size = source.tellg();
//    source.seekg(0);
//
//    // allocate memory for buffer
//    char *buffer = new char[bufsize];
//    std::streamsize readBytes;
//
//    while (source) {
//        source.read(buffer, bufsize);
//        readBytes = source.gcount();
//        dest.write(buffer, readBytes);
//    }
//
//    // clean up
//    delete[] buffer;
//    source.close();
//    dest.close();

//    source.seekg(0, std::ios::end);
//    std::ifstream::pos_type size = source.tellg();
//    source.seekg(0);
//    // allocate memory for buffer
//    char *buffer = new char[size];
//
//    // copy file
//    source.read(buffer, size);
//    dest.write(buffer, size);
//
//    // clean up
//    delete[] buffer;
//    source.close();
//    dest.close();


    LOG(INFO) << "Client initialized successfully";
    return 0;
}