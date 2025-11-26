#ifndef NETWORK_H
#define NETWORK_H


class NetworkFlags {
  public:
    static bool server_flag;
    static bool client_flag;
    static unsigned char VERSION;
    static unsigned char up_packet[5];
};


#endif
