# trade-frame/lib/OUCommon
cmake_minimum_required(VERSION 3.13)

PROJECT(OUCommon)

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

find_package(Boost 1.77.0 COMPONENTS system REQUIRED)

set(
  file_h
    CharBuffer.h
    Colour.h
    ConsoleStream.h
    CountryCode.h
    CurrencyCode.h
    Debug.h
    Decimal.h
    Delegate.h
    FastDelegate.h
    KeyWordMatch.h
#    Log.h
    ManagerBase.h
    MinHeap.h
    MSWindows.h
    MultiKeyCompare.h
    Network.h
    ReadCodeListCommon.h
    ReadNaicsToSicCodeList.h
    ReadSicCodeList.h
    ReadSicToNaicsCodeList.h
    ReadSymbolFile.h
    ReusableBuffers.h
    Singleton.h
    SmartVar.h
    SpinLock.h
    TimeSource.h
    Worker.h
    WuManber.h
  )

set(
  file_cpp
    CharBuffer.cpp
    ConsoleStream.cpp
    CountryCode.cpp
    CurrencyCode.cpp
#    Log.cpp
    ReadCodeListCommon.cpp
    ReadNaicsToSicCodeList.cpp
    ReadSicCodeList.cpp
    ReadSicToNaicsCodeList.cpp
    ReadSymbolFile.cpp
    Singleton.cpp
    SmartVar.cpp
    TimeSource.cpp
    WuManber.cpp
  )

add_library(
  ${PROJECT_NAME} STATIC
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

#add_library(libcommon WuManber.cpp ConsoleStream.cpp CharBuffer.cpp)
#set_target_properties(libcommon PROPERTIES LIBRARY_OUTPUT_NAME common )

#install (TARGETS libcommon DESTINATION lib)
