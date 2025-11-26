#include "file.h"
#include <vector>
#include <filesystem>
#include "network.h"

void FileProccessor::init_f_scan() {
  files = std::vector<File>();

  for(std::filesystem::recursive_directory_iterator i("."), end; i != end; i++){
    if(!std::filesystem::is_directory(i->path())) {
      File f = {
        .file_name = i -> path().filename(),
        .local_path = i -> path(),
        .hash = 0,
      };

      f.hash = f.calc_fnv1a_64_file();
      files.push_back(f);

      //std::cout << i -> path().filename() << " | " << f.hash << "\n";
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

