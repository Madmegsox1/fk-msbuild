#ifndef PACKET_H
#define PACKET_H

#include <cstddef>
#include <vector>

class Packet {
  public:
    unsigned char opcode;
    std::vector<unsigned char> data;

    size_t decode_size(int sock);
    
    virtual std::vector<unsigned char> compile();
    virtual void recive(int sock);
    virtual void send();
    virtual void send(std::vector<unsigned char> data);
    virtual void send(int sock);
};


class C_checksum_lst : public Packet {
  public:
    C_checksum_lst();
};

class C_handshake : public Packet {
  public:
    C_handshake();

    // When the client recives it
    void recive(int sock) override;
    void send() override;
    void send(std::vector<unsigned char> data) override;
    void send(int sock)override;
};

#endif
