# trade-frame/lib/TFFreeRadicals
cmake_minimum_required (VERSION 3.13)

PROJECT(TFFreeRadicals)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER_ARCHITECTURE_ID, "x64")
#set(CMAKE_EXE_LINKER_FLAGS "--trace --verbose")
#set(CMAKE_VERBOSE_MAKEFILE ON)

set(
  file_h
#    HistoryDailyTick.h
    MarketStates.h
  )

set(
  file_cpp
#    HistoryDailyTick.cpp
    MarketStates.cpp
  )

add_library(
  ${PROJECT_NAME} STATIC
  ${file_h}
  ${file_cpp}
  )

target_include_directories(
  ${PROJECT_NAME} PUBLIC
    ".."
  )

