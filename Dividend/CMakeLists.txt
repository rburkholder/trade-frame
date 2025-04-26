# trade-frame/Dividend
cmake_minimum_required (VERSION 3.13)

PROJECT(Dividend)

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

find_package(Boost ${TF_BOOST_VERSION} CONFIG REQUIRED COMPONENTS system date_time program_options thread filesystem serialization regex log log_setup)

#message("boost lib: ${Boost_LIBRARIES}")

set(
  file_h
    Config.hpp
    DbRecordDividend.hpp
    Process.hpp
  )

set(
  file_cpp
    main.cpp
    Config.cpp
    DbRecordDividend.cpp
    Process.cpp
  )

add_executable(
  ${PROJECT_NAME}
    ${file_h}
    ${file_cpp}
  )

# from https://www.foonathan.net/2018/10/cmake-warnings/ (-Werror turns warnings into errors)
#target_compile_options( ${PROJECT_NAME} PRIVATE -Werror -Wall -Wextra -Wpedantic -Wconversion )
#target_compile_options( ${PROJECT_NAME} PRIVATE         -Wall -Wextra -Wpedantic -Wconversion )
target_compile_definitions(${PROJECT_NAME} PUBLIC BOOST_LOG_DYN_LINK )
#target_compile_definitions(${PROJECT_NAME} PUBLIC wxUSE_GUI )
# need to figure out how to make this work
#add_compile_options(`/usr/local/bin/wx-config --cxxflags`)
target_compile_definitions(${PROJECT_NAME} PUBLIC -D_FILE_OFFSET_BITS=64 )
#target_compile_definitions(${PROJECT_NAME} PUBLIC -DWXUSINGDLL )
#target_compile_definitions(${PROJECT_NAME} PUBLIC -D__WXGTK__ )

# SYSTEM turns the include directory into a system include directory. 
# Compilers will not issue warnings from header files originating from there.
target_include_directories(
  ${PROJECT_NAME} SYSTEM PUBLIC
    "../lib"
  )

target_link_directories(
  ${PROJECT_NAME} PUBLIC
    /usr/local/lib
  )

target_link_libraries(
  ${PROJECT_NAME}
      TFTrading
      TFInteractiveBrokers
      TFHDF5TimeSeries
      TFIQFeed
      TFTimeSeries
      TFTrading
      OUCommon
      dl
      z
      ${Boost_LIBRARIES}
      pthread
  )

