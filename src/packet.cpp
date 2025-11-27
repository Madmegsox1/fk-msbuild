#include "packet.h"
#include <alloca.h>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>
#include "network.h"

std::vector<unsigned char> Packet::compile(){
  auto packet = std::vector<unsigned char>();
  packet.push_back(opcode);
  auto size = data.size();

  if(size > 0){
    packet.push_back(static_cast<unsigned char>(size & 0xff));
    packet.push_back(static_cast<unsigned char>((size >> 8) & 0xff));
    packet.push_back(static_cast<unsigned char>((size >> 16) & 0xff));
    packet.push_back(static_cast<unsigned char>((size >> 24) & 0xff));

    for(auto b : data){
      packet.push_back(b);
    }
  }


  return packet;
}

void Packet::send_p(int sock){
  auto packet = compile();
  send(sock, packet.data(), packet.size(), 0);
}

bool Packet::check_opcode(int sock){
  unsigned char buff[1] = {0};

  long r = read(sock, buff, 1);
  if(r != 1) return false;
  
  return (opcode == buff[0]);
}

size_t Packet::decode_size(int sock) {
  unsigned char buff[4] = {0};
  long r = read(sock, buff, 4);

  if(r != 4) return 0;

  size_t len = static_cast<size_t>(((buff[3] & 0xff) << 24) | ((buff[2] & 0xff) << 16) | ((buff[1] & 0xff) << 8) | (buff[0] & 0xff));

  return len;
}

C_checksum_lst::C_checksum_lst(){
  opcode = 0x0A;
  data = std::vector<unsigned char>();
}

/// Server Handshake handler
S_handshake::S_handshake(){
  auto nf = NetworkFlags();
  opcode = 0x0B;
  data = std::vector<unsigned char>();

  data.push_back(0xAA);
  data.push_back(0x55);
  data.push_back(0x00);
  data.push_back(0xFF);
  data.push_back(nf.VERSION);
}

void S_handshake::recive(int sock){
  auto nf = NetworkFlags();
  size_t pk_len = decode_size(sock);

  unsigned char * buff = new unsigned char[pk_len];

  read(sock, buff, pk_len);

  if(sizeof(buff) != 5){
    throw std::runtime_error("Handshake packet was an unexpected size");
  }

  if(nf.handshake_flag){
    throw std::runtime_error("Handshake already completed yet was resent");
  }

  if((buff[0] | buff[1]) == 0xFF && (buff[2] & buff[3]) == 0x00 && buff[4] == nf.VERSION) {
    nf.handshake_flag = true;
  }
  else {
    std::cerr << "Handshake packet failed verficiation";
    if(buff[4] != nf.VERSION){
      std::cerr << "Server is on a diffrent version of fk_msbuild";
    }
  }

  delete []  buff;

  send_p(sock);
}


/// Client Handshake handler

C_handshake::C_handshake(){
  auto nf = NetworkFlags();
  opcode = 0x0B;
  data = std::vector<unsigned char>();

  data.push_back(0xAA);
  data.push_back(0x55);
  data.push_back(0x00);
  data.push_back(0xFF);
  data.push_back(nf.VERSION);
}

void C_handshake::recive(int sock) {
  auto nf = NetworkFlags();
  if(!check_opcode(sock)) return;

  size_t pk_len = decode_size(sock);

  unsigned char * buff = new unsigned char[pk_len];

  read(sock, buff, pk_len);

  if(sizeof(buff) != 5){
    throw std::runtime_error("Handshake packet was an unexpected size");
  }

  if(nf.handshake_flag){
    throw std::runtime_error("Handshake already completed yet was resent");
  }

  if((buff[0] | buff[1]) == 0xFF && (buff[2] & buff[3]) == 0x00 && buff[4] == nf.VERSION) {
    nf.handshake_flag = true;
  }
  else {
    std::cerr << "Handshake packet failed verficiation";
    if(buff[4] != nf.VERSION){
      std::cerr << "Server is on a diffrent version of fk_msbuild";
    }
  }

  delete [] buff;
}



