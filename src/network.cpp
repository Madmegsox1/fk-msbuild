#include "network.h"


bool NetworkFlags::client_flag = false;
bool NetworkFlags::server_flag = false;
unsigned char NetworkFlags::VERSION = 100;

unsigned char NetworkFlags::up_packet[5] = { 
    0xAA, 0x55, 0x00,
    0xFF, VERSION
};
