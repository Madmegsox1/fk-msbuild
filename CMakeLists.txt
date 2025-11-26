cmake_minimum_required(VERSION 3.10)

project(fk_msbuild VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(APP_SOURCES
  src/main.cpp 
  src/file.cpp
  src/network.cpp
)

add_executable(${PROJECT_NAME} ${APP_SOURCES})

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
