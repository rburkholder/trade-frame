# trade-frame/lib/DBDividend
cmake_minimum_required (VERSION 3.13)

PROJECT(DBDividend)

#set(CMAKE_EXE_LINKER_FLAGS "--trace --verbose")
#set(CMAKE_VERBOSE_MAKEFILE ON)

set(
  file_h
    TableTag.hpp
    TableSymbol.hpp
    TableDividend.hpp
  )

set(
  file_cpp
    TableTag.cpp
    TableSymbol.cpp
    TableDividend.cpp
  )

add_library(
  ${PROJECT_NAME}
    ${file_h}
    ${file_cpp}
  )

  #target_compile_definitions(${PROJECT_NAME} PUBLIC BOOST_LOG_DYN_LINK )

target_include_directories(
  ${PROJECT_NAME} PUBLIC
    ".."
  )
