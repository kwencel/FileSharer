# FileSharer
A P2P file sharing program. 

# Build prerequisites
    CMake 3.6 (it will probably compile using older versions too, see the last paragraph)
    Boost 1.63.0 (filesystem and serialization)
    OpenSSL 1.0.2 (used for calculating md5 hashes)
    QT 5.8 (for GUI)

# Build instructions
```
git clone --recursive https://github.com/kwencel/FileSharer
cd FileSharer
cmake .
make
```

# Configuration
**Pass command line parameters to client and tracker to configure them**

**Client**
```
TRACKER_IP Public IP of tracker  
TRACKER_PORT Port on which tracker is bound  
CLIENT_IP IP client binds to  
CLIENT_PORT Port client binds to  
Usage: ./client <TRACKER_IP> <TRACKER_PORT> <CLIENT_IP> <CLIENT_PORT>
```

**Tracker**
```
TRACKER_BIND_IP IP tracker binds to
TRACKER_BIND_PORT Port tracker binds to
Usage: ./tracker <TRACKER_BIND_IP> <TRACKER_BIND_PORT>
```

# How to use
On startup client tries to communicate with tracker to register local files and request signatures
of files available from all registered peers. LocalFiles table lists all local files and displays
their names, hashes and sizes. TrackerFiles table lists all files from registered peers and displays
download progress and number of peers for each file in addition to the same information as in LocalFiles
table.

Users can use two buttons for registering to tracker and requesting available files after startup.

Progress is updated as the files are downloaded. Bold rows indicate files that are not fully downloaded.
To start download double click on a row. If the file is downloaded already nothing happens.

Program supports renewing download of partially downloaded files and downloads chunks of files from
all available peers.

# Older CMake version?
Try to change the minimum required version in CMakeLists.txt to match the version you have installed. There shouldn't be any issues.
