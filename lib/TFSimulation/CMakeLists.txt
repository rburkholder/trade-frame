# trade-frame/lib/TFSimulation
cmake_minimum_required (VERSION 3.13)

PROJECT(TFSimulation)

#set(CMAKE_EXE_LINKER_FLAGS "--trace --verbose")
#set(CMAKE_VERBOSE_MAKEFILE ON)

set(
  file_h
#    CrossThreadMerge.h
    MergeDatedDatumCarrier.h
    MergeDatedDatums.h    
    SimulateOrderExecution.h
    SimulationProvider.h
    SimulationSymbol.h
  )

set(
  file_cpp
#    CrossThreadMerge.cpp
    MergeDatedDatums.cpp
    SimulateOrderExecution.cpp
    SimulationProvider.cpp
    SimulationSymbol.cpp
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

#target_link_libraries(
#  ${PROJECT_NAME}
    
#  )
