#ifndef PACKET_H
#define PACKET_H

#include <cstddef>
#include <vector>

class Packet {
  public:
    unsigned char opcode;
    size_t packet_size;
    std::vector<unsigned char> data;

    void recive();
    void compile();
    void send();
};


class C_checksum_lst : public Packet {
  public:
    C_checksum_lst();

};

#endif
