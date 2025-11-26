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
