#ifndef FILESHARER_DEFINE_H
#define FILESHARER_DEFINE_H

#define CHUNK_SIZE_KB 128
#define MD5_BUFFER_SIZE 1024

#define CHUNK_SIZE (1024 * CHUNK_SIZE_KB)

//TRACKER

#define TRACKER_BIND_IP INADDR_ANY
#define TRACKER_BIND_PORT 1024

//PROTOCOL

#define PROTOCOL_HEADER_REGISTER 'A'
#define PROTOCOL_HEADER_FILE_REQUEST 'B'
#define PROTOCOL_TEST 'TEST'

#endif //FILESHARER_DEFINE_H
