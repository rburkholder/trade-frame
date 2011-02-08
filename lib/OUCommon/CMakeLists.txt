cmake_minimum_required(VERSION 2.8)

PROJECT(libcommon)

#ADD_EXECUTABLE(libcommon)
add_library(libcommon WuManber.cpp ConsoleStream.cpp CharBuffer.cpp)
set_target_properties(libcommon PROPERTIES LIBRARY_OUTPUT_NAME common )

install (TARGETS libcommon DESTINATION lib)