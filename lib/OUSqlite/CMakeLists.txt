# trade-frame/lib/OUSqlite
cmake_minimum_required(VERSION 3.13)

project (OUSqlite)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER_ARCHITECTURE_ID, "x64")
#set(CMAKE_EXE_LINKER_FLAGS "--trace --verbose")
#set(CMAKE_VERBOSE_MAKEFILE ON)

set(
  file_h
    Actions.h
    ISqlite3.h
    Session.h
    sqlite3.h
    StatementState.h
  )

set(
  file_c
    sqlite3.c
  )

set(
  file_cpp
    Actions.cpp
    ISqlite3.cpp
    Session.cpp
  )

add_library(
  ${PROJECT_NAME} STATIC
  ${file_h}
  ${file_c}
  ${file_cpp}
  )

target_include_directories(
  ${PROJECT_NAME} PUBLIC
    ".."
  )

