#include "file.h"
#include <vector>
#include <filesystem>

void FileProccessor::init_f_scan() {
  files = std::vector<File>();
  remote_files = std::vector<File>();

  size_t id_counter = 0;

  for(std::filesystem::recursive_directory_iterator i("."), end; i != end; i++){
    if(!std::filesystem::is_directory(i->path())) {
      File f = {
        .file_name = i -> path().filename(),
        .local_path = i -> path(),
        .file_id = id_counter, 
        .hash = 0,
      };

      f.hash = f.calc_fnv1a_64_file();
      files.push_back(f);
      id_counter++;

      std::cout << i -> path().filename() << " | " << f.hash << "\n";
    }
  }
}

std::vector<File> FileProccessor::f_diff() {
  auto changed_files = std::vector<File>();

  for (auto f : files){
    if(f.file_change()) changed_files.push_back(f);
  }

  return changed_files;
}


void FileProccessor::f_remote_diff() {
  auto diff = std::vector<FileDiff>();

  for(auto local : files){
    for(auto remote : remote_files){
      if(local.file_name == remote.file_name && local.hash != remote.hash) {
        local.file_id = remote.file_id;
        diff.push_back({.file = local, .option = EDIT});
      }
    }
  }

  for(auto remote : remote_files) {
    bool flag = false;
    for(auto local : files){
      if(local.file_name == remote.file_name) {
        flag = true;
        break;
      }
    }

    if(!flag) diff.push_back({.file = remote , .option = NEW });
  }

  diff_files = diff;
}

