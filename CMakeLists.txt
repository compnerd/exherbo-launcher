# Copyright 2019 Saleem Abdulrasool <compnerd@compnerd.org>

cmake_minimum_required(VERSION 3.14)

project(exherbo-launcher VERSION 1.0.0 LANGUAGES CXX RC)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

add_executable(exherbo
  Source/distribution.cc
  Source/main.cc
  Source/message.cc
  Source/wslapi.cc)
target_sources(exherbo PRIVATE
  messages.h
  messages.rc)
set_source_files_properties(messages.h messages.rc PROPERTIES
  GENERATED TRUE)
add_custom_command(OUTPUT messages.rc messages.h
  COMMAND mc -r ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/Source/messages.mc)
target_compile_definitions(exherbo PRIVATE
  _CONSOLE)
target_link_libraries(exherbo PRIVATE
  onecore)

# image.tar.xz downoad and conversion to image.tar.xz
if(EXISTS ${CMAKE_BINARY_DIR}/image.tar.gz)
  message(STATUS "${CMAKE_BINARY_DIR}/image.tar.gz exists; not downloading")
else()
  if(EXISTS ${CMAKE_BINARY_DIR}/image.tar.xz)
    message(STATUS "${CMAKE_BINARY_DIR}/image.tar.xz exists; not downloading")
  else()
    file(DOWNLOAD https://dev.exherbo.org/stages/exherbo-x86_64-pc-linux-gnu-current.tar.xz
      ${CMAKE_BINARY_DIR}/image.tar.xz
      SHOW_PROGRESS)
  endif()

  add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/image.tar.gz
    COMMAND xz -d ${CMAKE_BINARY_DIR}/image.tar.xz
    COMMAND gzip ${CMAKE_BINARY_DIR}/image.tar
    DEPENDS ${CMAKE_BINARY_DIR}/image.tar.xz
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

add_custom_target(image ALL DEPENDS ${CMAKE_BINARY_DIR}/image.tar.gz)

if(NOT EXISTS ${CMAKE_SOURCE_DIR}/exherbo_TemporaryKey.pfx)
  # https://docs.microsoft.com/en-us/windows/desktop/appxpkg/how-to-create-a-package-signing-certificate
  add_custom_command(OUTPUT ${CMAKE_SOURCE_DIR}/exherbo_TemporaryKey.pfx
    COMMAND makecert -n "CN=exherbo" -r -h 0 -eku 1.3.6.1.5.5.7.3.3,1.3.6.1.4.1.311.10.3.13 -sv ${CMAKE_SOURCE_DIR}/exherbo_TemporaryKey.pvk ${CMAKE_SOURCE_DIR}/exherbo_TemporaryKey.cer
    COMMAND pvk2pfx -pvk ${CMAKE_SOURCE_DIR}/exherbo_TemporaryKey.pvk -spc ${CMAKE_SOURCE_DIR}/exherbo_TemporaryKey.cer -pfx ${CMAKE_SOURCE_DIR}/exherbo_TemporaryKey.pfx)
endif()

add_custom_target(cert ALL DEPENDS ${CMAKE_SOURCE_DIR}/exherbo_TemporaryKey.pfx)

# appx construction
add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/exherbo_${PROJECT_VERSION}_${CMAKE_SYSTEM_PROCESSOR}.appx
  COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/appx
  COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:exherbo> ${CMAKE_BINARY_DIR}/appx/
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/Assets ${CMAKE_BINARY_DIR}/appx/Assets/
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/Package.appxmanifest ${CMAKE_BINARY_DIR}/appx/AppxManifest.xml
  COMMAND makeappx pack /d ${CMAKE_BINARY_DIR}/appx /h SHA256 /overwrite /p ${CMAKE_BINARY_DIR}/exherbo_${PROJECT_VERSION}_${CMAKE_SYSTEM_PROCESSOR}.appx
  COMMAND signtool sign /fd SHA256 /f ${CMAKE_SOURCE_DIR}/exherbo_TemporaryKey.pfx ${CMAKE_BINARY_DIR}/exherbo_${PROJECT_VERSION}_${CMAKE_SYSTEM_PROCESSOR}.appx
  DEPENDS exherbo image cert)

add_custom_target(appx ALL DEPENDS ${CMAKE_BINARY_DIR}/exherbo_${PROJECT_VERSION}_${CMAKE_SYSTEM_PROCESSOR}.appx)

