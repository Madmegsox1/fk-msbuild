#ifndef PACKET_H
#define PACKET_H

#include "file.h"
#include <cstddef>
#include <iostream>
#include <vector>

class Packet {
  public:
    unsigned char opcode;
    std::vector<unsigned char> data;

    // @pram the socket to read 4 bytes from to decode the size value
    // @return size type but max size is a 32bit int
    // we aint going over 2 billion bytes so should be enough
    size_t decode_size(int sock);

    // @return encoded 32bit int
    std::array<unsigned char, 4> encode_size(size_t size);

    // @return encoded 64bit int
    std::array<unsigned char, 8> encode_u64(uint64_t value);

    bool check_opcode(int sock);
    
    virtual ~Packet(){}
    virtual std::vector<unsigned char> compile();
    virtual void recive(int _){std::cerr << "This Shouldnt be called";}
    virtual void send_p(int sock);
};


class C_checksum_lst : public Packet {
  FileProccessor fileProc;
  public:
    C_checksum_lst(FileProccessor fp);

    void recive(int sock) override;
};

class S_checksum_lst : public Packet {
  FileProccessor fileProc;
  public:
    S_checksum_lst(FileProccessor fp);

    void recive(int sock) override;
 };

class S_handshake : public Packet {
  public:
    S_handshake();
    // Called when the server recives a handshake packet
    void recive(int sock) override;
};

class C_handshake : public Packet {
  public:
    C_handshake();
    // Called when the client recives a handshake packet 
    void recive(int sock) override;
};

#endif
