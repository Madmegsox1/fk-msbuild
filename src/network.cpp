#include "network.h"
#include "file.h"
#include <arpa/inet.h>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <utility>
#include "packet.h"


bool NetworkFlags::client_flag = false;
bool NetworkFlags::server_flag = false;
bool NetworkFlags::handshake_flag = false;
unsigned char NetworkFlags::VERSION = 100;

unsigned char NetworkFlags::up_packet[5] = { 
    0xAA, 0x55, 0x00,
    0xFF, VERSION
};


void ClientNetworkHandler::init_handshake(){
  flags.handshake_flag = false;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port);

  connect_socket();

  auto handshake = C_handshake();
  handshake.send_p(sock);
  handshake.recive(sock);

  connect_socket();

  auto checksum = C_checksum_lst(fProc);
  checksum.send_p(sock);
}


void ClientNetworkHandler::connect_socket(){
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    throw std::runtime_error("Failed to create socket");
  }

  if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0) {
    throw std::runtime_error("Invaild Address");
  }

  if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
      throw std::runtime_error("Connection failed");
  }
}


void ServerNetworkHandler::init() {
  if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
    perror("Socket creation failed");
    return;
  }

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port);

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

  fProc.init_f_scan();
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

    for (const auto& packet : packet_register) {
      if(opcode == packet->opcode){
        if(opcode == handshake_pk.opcode && flags.handshake_flag) {
          std::cerr << "Handshake already completed\n";
          close(sock);
          continue;
        }
        std::cout << "Recived Packet : opcode : " << std::to_string(packet->opcode) << "\n";
        packet->recive(sock);
        close(sock);
      }
    }
  }
}

void ServerNetworkHandler::register_packets(){
  // Request checksums of files from host

  auto handshake_ptr = std::make_unique<S_handshake>();
  // Handshake packet to check server version
  packet_register.push_back(std::move(handshake_ptr));
  auto checksum_ptr = std::make_unique<S_checksum_lst>(fProc);
  packet_register.push_back(std::move(checksum_ptr));
}
