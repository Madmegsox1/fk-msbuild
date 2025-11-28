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

    size_t decode_size(int sock);
    bool check_opcode(int sock);
    
    virtual std::vector<unsigned char> compile();
    virtual void recive(int sock){std::cerr << "This Shouldnt be called";}
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
