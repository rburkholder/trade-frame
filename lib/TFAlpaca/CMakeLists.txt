# trade-frame/lib/TFAlpaca
cmake_minimum_required (VERSION 3.13)

PROJECT(TFAlpaca)

#set(CMAKE_EXE_LINKER_FLAGS "--trace --verbose")
#set(CMAKE_VERBOSE_MAKEFILE ON)

set(
  file_h
    root_certificates.hpp
    one_shot.hpp
    web_socket.hpp
    Asset.hpp
    Order.hpp
    Provider.hpp
  )

set(
  file_cpp
    one_shot.cpp
    web_socket.cpp
    Asset.cpp
    Order.cpp
    Provider.cpp
  )

add_library(
  ${PROJECT_NAME}
  ${file_h}
  ${file_cpp}
  )

target_compile_definitions(${PROJECT_NAME} PUBLIC BOOST_LOG_DYN_LINK )

target_include_directories(
  ${PROJECT_NAME} PUBLIC
    ".."
  )

