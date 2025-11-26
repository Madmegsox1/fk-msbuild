#include <asm-generic/socket.h>
#include <cstddef>
#include <cstdio>
#include <filesystem>
#include <iostream>
#include <string.h>
#include <fstream>
#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "file.h"
#include "network.h"


void start_server(){
  auto net_f = NetworkFlags();
  int server_fd, new_socket;
  long valread;
  struct sockaddr_in address;
  int addrlen = sizeof(address);

  if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
    perror("Socket creation failed");
    return;
  }

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(8090);

  int opt = 1;

  if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
    perror("setsockopt failed");
    return;
  }

  if(bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0){
    perror("Bind Failed");
    return;
  }

  if(listen(server_fd, 16) < 0){
    perror("Listen Failed");
    return;
  }

  std::cout << "Server Started on port 8090...\n";

  if((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0){
    perror("Failed to Accept clients");
    return;
  }

  
  const size_t data_len = sizeof(net_f.up_packet);

  unsigned char buff[5] = {0};

  valread = read(new_socket, buff, 5);

  // Server
  if(valread > 0) {
    if((buff[0] | buff[1]) == 0xFF && (buff[2] & buff[3]) == 0x00 && buff[4] == net_f.VERSION) {
      std::cout << "Intro Packet Read \n";
      send(new_socket, net_f.up_packet, data_len, 0);
      close(new_socket);
    }
    else {
      std::cout << "Intro Packet Failed to read\n";
      close(new_socket);
    }
  }
}


void start_client(){

  auto clientHandler = ClientNetworkHandler();

  clientHandler.init_handshake();

  FileProccessor fp = FileProccessor();
  fp.init_f_scan();
  
}










int main(int argc, char **argv){
  auto net_f = NetworkFlags();
  std::cout << "Fk_msbuild remote build util\n";

  if(argc < 2){
    std::cout << "--help - for help\n";
    return 0;    
  }

  if(strcmp("--help", argv[1]) == 0){
    std::cout << "--helo - for help\n-s - for server mode\n-c - for client mode";
  }

  if(strcmp("-c", argv[1]) == 0){
    net_f.client_flag = true;
  }

  if(strcmp("-s", argv[1]) == 0){
    net_f.server_flag = true;
  }

  if(!std::filesystem::exists("./fk_msbuild.cfg")){
    std::cout << "Please Create fk_msbuild.cfg\n";
    return 0;
  }

  std::string config;
  std::ifstream configFile("./fk_msbuild.cfg");

  while(std::getline(configFile, config)){

    
  }

  configFile.close();

  if(net_f.server_flag) {
    start_server();
  }

  if(net_f.client_flag){
    start_client();
  }

  return 0;
}


