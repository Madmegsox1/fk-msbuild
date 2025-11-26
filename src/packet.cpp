#include "packet.h"
#include <vector>


C_checksum_lst::C_checksum_lst(){
  opcode = 0x0A;
  packet_size = 1;
  data = std::vector<unsigned char>();
}

