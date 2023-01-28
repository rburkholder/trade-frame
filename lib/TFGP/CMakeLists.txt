# trade-frame/lib/TFGP
cmake_minimum_required (VERSION 3.13)

PROJECT(TFGP)

#set(CMAKE_EXE_LINKER_FLAGS "--trace --verbose")
#set(CMAKE_VERBOSE_MAKEFILE ON)

set(
  file_h
    NodeTimeSeries.h
    TimeSeriesForNode.h
    TimeSeriesRegistration.h
  )

set(
  file_cpp
    NodeTimeSeries.cpp
    TimeSeriesRegistration.cpp
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
