#ifndef FILESHARER_TRACKERHANDLER_H
#define FILESHARER_TRACKERHANDLER_H

#include <PeerFile.h>
#include <FileInfo.h>

namespace TrackerHandler {
    /**
     * Helper function for registering to tracker
     * @param ownIp Ip of local client
     * @param ownPort Port of local client
     * @param fileInfoVector Vector detailing the files available to local client
     * @return Confirmation from tracker as string
     */
    std::string registerToTracker(std::string ownIp, uint16_t ownPort, std::vector<FileInfo> fileInfoVector);

    /**
     * Helper function for asking tracker for available files
     * @return Vector detailing available files across all peers
     */
    std::vector<FileInfo> getAvailableFiles();

    /**
     * Helper function for asking tracker for list of peers with a particular file
     * @param hash Hash of file in question
     * @return Vector of PeerFile for all peers that have the file in question
     */
    std::vector<PeerFile> getPeersWithFileByHash(const std::string &hash);
}


#endif //FILESHARER_TRACKERHANDLER_H
