# trade-frame/lib/TFTimeSeries
cmake_minimum_required (VERSION 3.13)

PROJECT(TFTimeSeries)

#set(CMAKE_EXE_LINKER_FLAGS "--trace --verbose")
#set(CMAKE_VERBOSE_MAKEFILE ON)

set(Boost_ARCHITECTURE "-x64")
#set(BOOST_LIBRARYDIR "/usr/local/lib")
set(BOOST_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(BOOST_USE_STATIC_RUNTIME OFF)
#set(Boost_DEBUG 1)
#set(Boost_REALPATH ON)
#set(BOOST_ROOT "/usr/local")
#set(Boost_DETAILED_FAILURE_MSG ON)
set(BOOST_INCLUDEDIR "/usr/local/include/boost")

find_package(Boost 1.77.0 COMPONENTS system REQUIRED)

set(
  file_h
    Adapters.h
    BarFactory.h
    DatedDatum.h
    DoubleBuffer.h
    ExchangeHolidays.h
#    MergeDatedDatumCarrier.h
#    MergeDatedDatums.h
    TimeSeries.h
    TSAllocator.h
    TSMicrostructure.h
  )

set(
  file_cpp
    BarFactory.cpp
    DatedDatum.cpp
    DoubleBuffer.cpp
    ExchangeHolidays.cpp
 #   MergeDatedDatums.cpp
    TimeSeries.cpp
    TSAllocator.cpp
    TSMicrostructure.cpp
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
#    ${Boost_LIBRARIES}
#  )

install(
  TARGETS ${PROJECT_NAME}
#    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
  )


