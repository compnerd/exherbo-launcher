# Copyright 2019 Saleem Abdulrasool <compnerd@compnerd.org>

cmake_minimum_required(VERSION 3.14)
project(exherbo-launcher LANGUAGES CXX RC)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

# NOTE(compnerd) MC is currently unavailable as a default rule
set(CMAKE_MC_COMPILE_OBJECT "mc -r <CMAKE_CURRENT_BINARY_DIR> <SOURCE>")

add_executable(exherbo
  Source/console.cc
  Source/distribution.cc
  Source/main.cc
  Source/wslapi.cc)
target_sources(exherbo PRIVATE
  messages.h
  messages.rc)
set_source_files_properties(messages.rc PROPERTIES
  GENERATED TRUE)
set_source_files_properties(messages.h PROPERTIES
  GENERATED TRUE)
add_custom_command(OUTPUT messages.rc messages.h
  COMMAND mc -r ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/Source/messages.mc)
target_compile_definitions(exherbo PRIVATE
  _CONSOLE)
target_link_libraries(exherbo PRIVATE
  onecore)

