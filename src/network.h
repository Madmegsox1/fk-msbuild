#ifndef NETWORK_H
#define NETWORK_H

#include "packet.h"
#include <netinet/in.h>
#include <sys/socket.h>
#include <vector>

class NetworkFlags {
  public:
    static bool server_flag;
    static bool client_flag;
    static bool handshake_flag;
    static unsigned char VERSION;
    static unsigned char up_packet[5];
};

class ServerNetworkHandler {
  const int port = 8090;
  int server_fd;
  int sock;
  long val_read;
  struct sockaddr_in address;
  int addrlen = sizeof(address);
  NetworkFlags flags = NetworkFlags();

  std::vector<Packet> packet_register;

  public:
    void init();
    void register_packets();
    void s_loop();
};

class ClientNetworkHandler {
  const int port = 8090;
  const char *ip = "127.0.0.1";
  int sock = 0;
  long val_read;
  struct sockaddr_in serv_addr;
  NetworkFlags flags = NetworkFlags();

  public:
    void init_handshake();

  
};


#endif
