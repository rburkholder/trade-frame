# trade-frame/lib/TFStatistics
cmake_minimum_required (VERSION 3.13)

PROJECT(TFStatistics)

#set(CMAKE_EXE_LINKER_FLAGS "--trace --verbose")
#set(CMAKE_VERBOSE_MAKEFILE ON)

set(
  file_h
    HistoricalVolatility.h
    Pivot.h
  )

set(
  file_cpp
    HistoricalVolatility.cpp
    Pivot.cpp
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
