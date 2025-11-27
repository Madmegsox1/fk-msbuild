#include "network.h"
#include <arpa/inet.h>
#include <iostream>
#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>
#include "packet.h"


bool NetworkFlags::client_flag = false;
bool NetworkFlags::server_flag = false;
unsigned char NetworkFlags::VERSION = 100;

unsigned char NetworkFlags::up_packet[5] = { 
    0xAA, 0x55, 0x00,
    0xFF, VERSION
};


void ClientNetworkHandler::init_handshake(){
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    throw std::runtime_error("Failed to create socket");
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port);
  
  if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0) {
    throw std::runtime_error("Invaild Address");
  }

  if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
      throw std::runtime_error("Connection failed");
  }

  const size_t data_len = sizeof(flags.up_packet);

  send(sock, flags.up_packet, data_len, 0);

  unsigned char buff[5] = {0};

  val_read = read(sock, buff, 5);

  
  if(val_read > 0 && val_read == 5){
    if((buff[0] | buff[1]) == 0xFF && (buff[2] & buff[3]) == 0x00 && buff[4] == flags.VERSION) {
      std::cout << "Server Handshake\n";
    }
    else {
      std::cerr << "Server Handshake Failed\n";
      if(buff[4] != flags.VERSION){
        std::cerr << "WRONG SERVER VERSION (CLIENT) v" << flags.VERSION << " (SERVER) v" << buff[4];
      }
    }
  }
  else {
    throw std::runtime_error("Unexpected Value recived");
  }
  
}


void ServerNetworkHandler::init() {
  if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
    perror("Socket creation failed");
    return;
  }

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(8090);

  int opt = 1;

  if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
    throw std::runtime_error("setsockopt failed");
  }

  if(bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0){
    throw std::runtime_error("Bind Failed");
  }

  if(listen(server_fd, 16) < 0){
    throw std::runtime_error("Listen Failed");
  }

  std::cout << "Server Started on port 8090...\n";


}

void ServerNetworkHandler::s_loop() {
  register_packets();
  flags.handshake_flag = false;

  while(true) {
    if ((sock = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0){
      perror("Failed to Accept clients");
      return;
    }

    unsigned char buff[1] = {0};
    val_read = read(sock, buff, 1);

    if(val_read != 1) {
      std::cerr << "Failed to get packet opcode";
      return;
    }

    auto opcode = buff[0];

    C_handshake handshake_pk = C_handshake();

    for (auto packet : packet_register) {
      if(opcode == packet.opcode){
        if(opcode == handshake_pk.opcode && !flags.handshake_flag){
          flags.handshake_flag = true;
        }
        else if(opcode == handshake_pk.opcode && flags.handshake_flag) {
          std::cerr << "Handshake already completed";
          return;
        }
        packet.recive(sock);
        packet.send(sock);
      }
    }
  }
}

void ServerNetworkHandler::register_packets(){
  // Request checksums of files from host
  packet_register.push_back(C_checksum_lst());

  // Handshake packet to check server version
  packet_register.push_back(C_handshake());
}
