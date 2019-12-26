# trade-frame/lib/ExcelFormat (library)
cmake_minimum_required (VERSION 3.13)

project (ExcelFormat)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER_ARCHITECTURE_ID, "x64")
#set(CMAKE_EXE_LINKER_FLAGS "--trace --verbose")
#set(CMAKE_VERBOSE_MAKEFILE ON)

set(
  file_h
    BasicExcel.hpp
    ExcelFormat.h
  )

set(
  file_cpp
    BasicExcel.cpp
    ExcelFormat.cpp
  )

add_library(
  ${PROJECT_NAME} STATIC
  ${file_h}
  ${file_cpp}
  )


