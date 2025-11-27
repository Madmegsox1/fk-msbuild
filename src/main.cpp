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
  auto serverHandler = ServerNetworkHandler();
  serverHandler.init();
  serverHandler.s_loop();
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


