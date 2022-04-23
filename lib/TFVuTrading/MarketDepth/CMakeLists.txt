# trade-frame/lib/TFVuTrading/MarketDepth
cmake_minimum_required (VERSION 3.13)

PROJECT(MarketDepth)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER_ARCHITECTURE_ID, "x64")
#set(CMAKE_EXE_LINKER_FLAGS "--trace --verbose")
#set(CMAKE_VERBOSE_MAKEFILE ON)

set(
  file_h
    Ladder/DataRowElement.hpp
    Ladder/DataRow.hpp
    Ladder/DataRows.hpp
    PanelTrade.hpp
    WinRowElement.hpp
    WinRow.hpp
  )

set(
  file_cpp
    Ladder/DataRowElement.cpp
    Ladder/DataRow.cpp
    Ladder/DataRows.cpp
    PanelTrade.cpp
    WinRowElement.cpp
    WinRow.cpp
  )

add_library(
  ${PROJECT_NAME}
  ${file_h}
  ${file_cpp}
  )

target_compile_definitions(${PROJECT_NAME} PUBLIC BOOST_LOG_DYN_LINK )
# need to figure out how to make this work
#add_compile_options(`/usr/local/bin/wx-config --cxxflags`)
target_compile_definitions(${PROJECT_NAME} PUBLIC -D_FILE_OFFSET_BITS=64 )
target_compile_definitions(${PROJECT_NAME} PUBLIC -DWXUSINGDLL )
target_compile_definitions(${PROJECT_NAME} PUBLIC -D__WXGTK__ )

target_include_directories(
  ${PROJECT_NAME} PUBLIC
    ".."
    "../.."
    "/usr/local/lib/wx/include/gtk3-unicode-3.0"
    "/usr/local/include/wx-3.0"
  )

