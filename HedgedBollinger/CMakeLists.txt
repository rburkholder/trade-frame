# trade-frame/HedgedBollinger
cmake_minimum_required (VERSION 3.13)

PROJECT(HedgedBollinger)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER_ARCHITECTURE_ID, "x64")
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

find_package(Boost 1.77.0 REQUIRED COMPONENTS system date_time program_options thread filesystem serialization regex log log_setup)

#message("boost lib: ${Boost_LIBRARIES}")

set(
  file_h
    EventUpdateOptionTree.h
    HedgedBollinger.h
    PositionState.h
    Strategy1.h
  )

set(
  file_cpp
    EventUpdateOptionTree.cpp
    HedgedBollinger.cpp
    PositionState.cpp
    Strategy1.cpp
  )

add_executable(
  ${PROJECT_NAME}
    ${file_h}
    ${file_cpp}
  )

target_compile_definitions(${PROJECT_NAME} PUBLIC BOOST_LOG_DYN_LINK )
target_compile_definitions(${PROJECT_NAME} PUBLIC wxUSE_GUI )
# need to figure out how to make this work
#add_compile_options(`/usr/local/bin/wx-config --cxxflags`)
target_compile_definitions(${PROJECT_NAME} PUBLIC -D_FILE_OFFSET_BITS=64 )
target_compile_definitions(${PROJECT_NAME} PUBLIC -DWXUSINGDLL )
target_compile_definitions(${PROJECT_NAME} PUBLIC -D__WXGTK__ )

target_include_directories(
  ${PROJECT_NAME} PUBLIC
    "../lib"
    "/usr/local/lib/wx/include/gtk3-unicode-3.1"
    "/usr/local/include/wx-3.1"
  )

target_link_directories(
  ${PROJECT_NAME} PUBLIC
    /usr/local/lib
  )

target_link_libraries(
  ${PROJECT_NAME}
      TFBitsNPieces
      TFInteractiveBrokers
      ib_client
      bidgcc000
      TFIQFeed
      TFSimulation
      TFVuTrading
      TFTrading
      TFOptions
      TFHDF5TimeSeries
      TFTimeSeries
      OUCommon
      OUSQL
      OUSqlite
      OUCharting
      TFIndicators
      hdf5_cpp
      hdf5
      sz
      dl
      z
      curl
      chartdir
      ${Boost_LIBRARIES}
      wx_gtk3u_adv-3.1
      wx_gtk3u_core-3.1
      wx_baseu-3.1
      pthread
  )

