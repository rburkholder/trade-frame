# trade-frame/lib/OUGP
cmake_minimum_required (VERSION 3.13)

PROJECT(OUGP)

#set(CMAKE_EXE_LINKER_FLAGS "--trace --verbose")
#set(CMAKE_VERBOSE_MAKEFILE ON)

set(
  file_h
    Individual.h
    NodeBoolean.h
    NodeCompare.h
    NodeDouble.h
    Node.h
    Population.h
    RootNode.h
    TreeBuilder.h
  )

set(
  file_cpp
    Individual.cpp
    NodeBoolean.cpp
    NodeCompare.cpp
    Node.cpp
    NodeDouble.cpp
    Population.cpp
    RootNode.cpp
    TreeBuilder.cpp
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
