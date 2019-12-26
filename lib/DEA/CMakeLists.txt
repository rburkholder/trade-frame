# trade-frame/lib/DEA
cmake_minimum_required (VERSION 3.13)

PROJECT(DEA)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER_ARCHITECTURE_ID, "x64")
#set(CMAKE_EXE_LINKER_FLAGS "--trace --verbose")
#set(CMAKE_VERBOSE_MAKEFILE ON)

set(
  file_h
    Account.h
    AccountsManager.h
    GeneralJournalEntry.h
    KeyTypes.h
  )

set(
  file_cpp
    Account.cpp
    AccountsManager.cpp
    GeneralJournalEntry.cpp
    KeyTypes.cpp
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
