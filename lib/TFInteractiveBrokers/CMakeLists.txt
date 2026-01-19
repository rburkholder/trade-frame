# trade-frame/lib/TFInteractiveBrokers
cmake_minimum_required (VERSION 3.13)

PROJECT(TFInteractiveBrokers)

#set(CMAKE_EXE_LINKER_FLAGS "--trace --verbose")
#set(CMAKE_VERBOSE_MAKEFILE ON)

add_subdirectory(client)

set(
  file_h
    Bag.hpp
    IBTWS.h
    IBSymbol.h
  )

set(
  file_cpp
    IBTWS.cpp
    IBSymbol.cpp
  )

add_library(
  ${PROJECT_NAME}
  ${file_h}
  ${file_cpp}
  )

target_compile_definitions(${PROJECT_NAME} PUBLIC BOOST_LOG_DYN_LINK )

# need to figure out how to make this work
#add_compile_options(`/usr/local/bin/wx-config --cxxflags`)
# used in EventIBInstrument.cpp - redo this somehow?

target_compile_definitions(${PROJECT_NAME} PRIVATE -D_FILE_OFFSET_BITS=64 )
target_compile_definitions(${PROJECT_NAME} PRIVATE -DWXUSINGDLL )
target_compile_definitions(${PROJECT_NAME} PRIVATE -D__WXGTK__ )

target_include_directories(
  ${PROJECT_NAME} PRIVATE
    ".."
  )

target_link_libraries(
  ${PROJECT_NAME} PRIVATE
    ib_client
    bidgcc000
)
