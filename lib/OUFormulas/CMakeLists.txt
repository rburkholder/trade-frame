# trade-frame/lib/OUFormulas
cmake_minimum_required(VERSION 3.13)

PROJECT(OUFormulas)

#set(CMAKE_EXE_LINKER_FLAGS "--trace --verbose")
#set(CMAKE_VERBOSE_MAKEFILE ON)

set(
  file_h
    InterestRates.h
  )

set(
  file_cpp
    InterestRates.cpp
  )

add_library(
  ${PROJECT_NAME} STATIC
  ${file_h}
  ${file_cpp}
  )


