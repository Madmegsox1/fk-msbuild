#include "packet.h"
#include <alloca.h>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>
#include "file.h"
#include "network.h"

std::vector<unsigned char> Packet::compile(){
  auto packet = std::vector<unsigned char>();
  packet.push_back(opcode);
  auto size = data.size();

  if(size > 0){
    auto encoded = encode_size(size);
    packet.insert(packet.end(), encoded.begin(), encoded.end());

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
uint64_t Packet::decode_u64(int sock) {
  unsigned char buff[8] = {0};
  long r = read(sock, buff, 8);

  if(r != 8) return 0;

  uint64_t u64i = 0;

  for(int i =0; i < 8; i++){
    u64i |= static_cast<uint64_t>(buff[i] & 0xff) << (56-(i*8)); 
  }

  return u64i;
}

std::array<unsigned char, 4> Packet::encode_size(size_t size){
  std::array<unsigned char, 4> i32 = {0};

  for(int i = 0; i < 4; i++){
    i32[i] = static_cast<unsigned char>((size >> (i * 8)) & 0xff);
  }

  return i32;
}

std::array<unsigned char, 8> Packet::encode_u64(uint64_t value){
  std::array<unsigned char, 8> i64 = {0};

  for(int i = 0; i < 8; i++){
    i64[i] = static_cast<unsigned char>((value >> (i * 8)) & 0xff);
  }

  return i64;
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

  if(pk_len != 5){
    throw std::runtime_error("Handshake packet was an unexpected size");
  }

  unsigned char * buff = new unsigned char[pk_len];

  long r = read(sock, buff, pk_len);

  if(r != 5){
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
      std::cerr << "Client is on a diffrent version of fk_msbuild";
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

  long r = read(sock, buff, pk_len);

  if(r != 5){
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

C_checksum_lst::C_checksum_lst(FileProccessor fp){
  opcode = 0x0A;
  data = std::vector<unsigned char>();
  this->fileProc = fp;

  fp.init_f_scan();

  auto file_len = encode_size(fp.files.size());
  data.insert(data.end(), file_len.begin(), file_len.end());

  for(auto file : fp.files){
    auto file_name_len = encode_size(file.file_name.length());
    data.insert(data.end(), file_name_len.begin(), file_name_len.end());
    data.insert(data.end(), file.file_name.begin(), file.file_name.end());

    auto file_path_len = encode_size(file.local_path.length());
    data.insert(data.end(), file_path_len.begin(), file_path_len.end());
    data.insert(data.end(), file.local_path.begin(), file.local_path.end());

    auto checksum = encode_u64(file.hash);
    data.insert(data.end(), checksum.begin(), checksum.end());
  }

  std::cout << "SIZE: " << data.size();
}

S_checksum_lst::S_checksum_lst(FileProccessor fp){
  opcode = 0x0A;
  data = std::vector<unsigned char>();
  this->fileProc = fp;
}

void C_checksum_lst::recive(int sock){
  auto nf = NetworkFlags::handshake_flag;

  if(!nf) return;

}

// Server executes this once C_checksum_lst is recived by server
void S_checksum_lst::recive(int sock){

  auto nf = NetworkFlags::handshake_flag;

  if(!nf) return;

  auto _= decode_size(sock);

  auto num_files = decode_size(sock);
  std::cout << num_files;

  for(size_t i = 0; i < num_files; i++){
    auto f_name_len = decode_size(sock);
    if(f_name_len == 0) close(sock);
    unsigned char * f_name = new unsigned char [f_name_len];

    auto f_path_len = decode_size(sock);
    if(f_path_len == 0)close (sock);
    unsigned char * f_path = new unsigned char [f_name_len];

    auto f_checksum = decode_u64(sock);

    std::cout<<"File: " << i << " " << f_name << " " << f_path << " " << f_checksum;

    delete [] f_path;
    delete [] f_name;

  }
  close(sock);
}



