# AppWebTrader(app)
cmake_minimum_required (VERSION 3.13)

cmake_policy(SET CMP0167 NEW)

project (AppWebTrader)

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

find_package(Boost ${TF_BOOST_VERSION} REQUIRED COMPONENTS program_options system log log_setup)

#message("boost lib: ${Boost_LIBRARIES}")

set(
  file_h
    AppManager.hpp
    AppWebTrader.hpp
    Config.hpp
    Server.hpp
    Server_impl.hpp
  )

set(
  file_cpp
    AppManager.cpp
    AppWebTrader.cpp
    Config.cpp
    Server.cpp
    Server_impl.cpp
    main.cpp
  )

add_executable(
  ${PROJECT_NAME}
    ${file_h}
    ${file_cpp}
  )

target_compile_definitions(${PROJECT_NAME} PUBLIC BOOST_LOG_DYN_LINK )

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
#      TFBitsNPieces
    TFTrading
    TFInteractiveBrokers
    TFSimulation
#      TFVuTrading
    TFOptions
    TFHDF5TimeSeries
    OUSQL
    OUSqlite
#      TFIndicators
    TFIQFeed
    TFTimeSeries
    TFTrading
    OUCommon
    TFTrading
    fmt
    dl
    z
    pthread
#    crypto
#    ssl
#    crypto++

    wt
    wthttp
    ${Boost_LIBRARIES}
  )

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)

# https://cmake.org/cmake/help/v3.4/manual/cmake-properties.7.html#target-properties
# https://stackoverflow.com/questions/tagged/cmake?sort=votes&pagesize=50
# https://github.com/Kitware/CMake/blob/master/Modules/FindBoost.cmake
# https://cmake.org/cmake/help/git-master/module/FindBoost.html

#target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
#add_definitions(${Boost_LIB_DIAGNOSTIC_DEFINITIONS})
#target_include_directories(${PROJECT_NAME} PUBLIC ".." ${Boost_INCLUDE_DIR})
#include_directories(".." ${Boost_INCLUDE_DIR})
#rm add_link_options( "--trace" )
#target_link_options( ${PROJECT_NAME} PUBLIC "--verbose" "--trace" "--stats")
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --verbose --trace --stats")
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --verbose --trace")
#set( CMAKE_EXPORT_COMPILE_COMMANDS ON )

#SET_TARGET_PROPERTIES(${PROJECT} PROPERTIES LINK_FLAGS "--verbose --trace")
#SET_TARGET_PROPERTIES(${PROJECT} PROPERTIES COMPILE_FLAGS "-DBOOST_LOG_DYN_LINK" )

