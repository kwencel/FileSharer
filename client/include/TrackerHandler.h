#ifndef FILESHARER_TRACKERHANDLER_H
#define FILESHARER_TRACKERHANDLER_H

#include <PeerFile.h>
#include <FileInfo.h>

namespace TrackerHandler {
    std::string registerToTracker(std::string ownIp, uint16_t ownPort, std::vector<FileInfo> fileInfoVector);
    std::vector<FileInfo> getAvailableFiles();
    std::vector<PeerFile> getPeersWithFileByHash(std::string hash);
}


#endif //FILESHARER_TRACKERHANDLER_H
