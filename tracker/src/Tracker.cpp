#include <easylogging++.h>
#include <TrackerHeader.h>

INITIALIZE_EASYLOGGINGPP

int main() {
    exampleHeader();
    LOG(INFO) << "Client initialized successfully";
    return 0;
}