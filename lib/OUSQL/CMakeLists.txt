# trade-frame/lib/OUSQL
cmake_minimum_required(VERSION 3.13)

PROJECT(OUSQL)

#set(CMAKE_EXE_LINKER_FLAGS "--trace --verbose")
#set(CMAKE_VERBOSE_MAKEFILE ON)

set(
  file_h
    Actions.h
    Constants.h
    Functions.h
    GKV.h
    SessionBase.h
    SessionImpl.h
  )

set(
  file_cpp
    Actions.cpp
  )

add_library(
  ${PROJECT_NAME} STATIC
  ${file_h}
  ${file_cpp}
  )


