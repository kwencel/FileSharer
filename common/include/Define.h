#ifndef FILESHARER_DEFINE_H
#define FILESHARER_DEFINE_H

//CLIENT

#define CHUNK_SIZE_KB               128
#define MD5_BUFFER_SIZE             1024
#define RECEIVE_BUFFER              1024
#define CHUNK_SIZE                  (1024 * CHUNK_SIZE_KB)
#define FILES_PATH_PREFIX           "files/"
#define READ_RETRIES                3
#define DEFAULT_TIMEOUT             20
#define MEMORY_PER_FILE_LIMIT_MB    5

//PROTOCOL

#define PROTOCOL_HEADER_REGISTER            0b00000010
#define PROTOCOL_HEADER_REGISTER_ACCEPT     0b00000011
#define PROTOCOL_HEADER_REGISTER_DUPLICATE  0b00000001
#define PROTOCOL_HEADER_PEERS_WITH_FILE     0b00000100
#define PROTOCOL_HEADER_LIST_FILES          0b00001000
#define PROTOCOL_PEER_INIT_HASH             0b00011000
#define PROTOCOL_PEER_INIT_ACK              0b00100000
#define PROTOCOL_PEER_REQUEST_CHUNK         0b00110000
#define PROTOCOL_PEER_SEND_CHUNK            0b01000000
#define PROTOCOL_PEER_REQUEST_HASHES        0b01100000
#define PROTOCOL_PEER_SEND_HASHES           0b01110000
#define PROTOCOL_PEER_CONNECTION_CLOSE      0b00000000

#endif //FILESHARER_DEFINE_H
