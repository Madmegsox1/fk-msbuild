#ifndef FILE_H
#define FILE_H

#include <cstddef>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <cstdint>
#include <vector>

constexpr uint64_t FNV_PRIME = 1099511628211ULL;
constexpr uint64_t FNV_OFFSET_BASIS = 14695981039346656037ULL;
constexpr int BUFFER_SIZE = 4096;


struct File {
  std::string file_name;
  std::string local_path;
  size_t file_id;

  uint64_t hash;

  /// @return if the file has changed then this will return false 
  bool file_change() {
    uint64_t n_hash = calc_fnv1a_64_file();
    return n_hash != hash;
  }


  /// @return the hash of the current file path
  uint64_t calc_fnv1a_64_file(){
    std::ifstream fileStream(local_path, std::ios::binary);
    if (!fileStream.is_open()) {
        throw std::runtime_error("Error opening file: " + local_path);
    }

    uint64_t hash = FNV_OFFSET_BASIS;
    char buffer[BUFFER_SIZE];

    while (fileStream.read(buffer, BUFFER_SIZE)) {
      size_t bytesRead = fileStream.gcount();

      for(size_t i = 0; i < bytesRead; ++i){
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(buffer[i]));
        hash *= FNV_PRIME;
      }
    }

    return hash;
  }
};


enum FILE_DIFF_OPT {
  EDIT,
  NEW,
};


struct FileDiff {
  File file;
  FILE_DIFF_OPT option;
};


class FileProccessor {

  public:
    std::vector<File> files;
    std::vector<File> remote_files;
    std::vector<FileDiff> diff_files;

    void init_f_scan();
    std::vector<File> f_diff();
    void f_remote_diff();
    File find_file_id(std::vector<File> files, size_t id); 

  private:
};

#endif
