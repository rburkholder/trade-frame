# trade-frame/lib/extras
cmake_minimum_required (VERSION 3.13)

PROJECT(extras)

# header only
add_library(
  ${PROJECT_NAME} INTERFACE
  )

target_sources(
  ${PROJECT_NAME} INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/FXMacroDataClient.hpp
  )

target_include_directories(
  ${PROJECT_NAME} INTERFACE
    ".."
  )
