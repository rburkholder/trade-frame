# trade-frame/rdaf/l1
cmake_minimum_required (VERSION 3.13)

PROJECT(rdaf_l1)

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

find_package(Boost ${TF_BOOST_VERSION} REQUIRED COMPONENTS system date_time program_options thread filesystem serialization regex log log_setup)

#message("boost lib: ${Boost_LIBRARIES}")

set(
  file_h
    l1.h
    Config.h
    ChartData.h
  )

set(
  file_cpp
    l1.cpp
    Config.cpp
    ChartData.cpp
  )

add_executable(
  ${PROJECT_NAME}
    ${file_h}
    ${file_cpp}
  )

target_compile_definitions(${PROJECT_NAME} PUBLIC BOOST_LOG_DYN_LINK )
target_compile_definitions(${PROJECT_NAME} PUBLIC FILE_OFFSET_BITS=64 )
target_compile_definitions(${PROJECT_NAME} PUBLIC wxUSE_GUI )
target_compile_definitions(${PROJECT_NAME} PUBLIC WXUSINGDLL )
target_compile_definitions(${PROJECT_NAME} PUBLIC __WXGTK__ )

# need to figure out how to make this work
# add_compile_options(`/usr/local/bin/wx-config --cxxflags`)

target_include_directories(
  ${PROJECT_NAME} PUBLIC
    "../lib"
    "/usr/local/lib/wx/include/gtk3-unicode-${WX_VERSION_MAJOR}"
    "/usr/local/include/wx-${WX_VERSION_MAJOR}"
    "/usr/local/include/rdaf"
  )

target_link_directories(
  ${PROJECT_NAME} PUBLIC
    /usr/local/lib
    /usr/local/lib/rdaf
  )

target_link_libraries(
  ${PROJECT_NAME}
      TFBitsNPieces
      TFInteractiveBrokers
      TFIQFeed
      TFSimulation
      TFVuTrading
      TFTrading
      TFOptions
      TFHDF5TimeSeries
      OUCommon
      OUSQL
      OUSqlite
      OUCharting
      TFIndicators
      TFTimeSeries
      dl
      z
      curl
      ${Boost_LIBRARIES}
#      wx_gtk3u_adv-3.1
      wx_gtk3u_core-${WX_VERSION_MAJOR}
      wx_baseu-${WX_VERSION_MAJOR}
      pthread
# rdaf components:
      Core
      Gpad
      Hist
      Rint
      Thread
      Tree
      RIO
  )

