# trade-frame/lib/TFBitsNPieces
cmake_minimum_required (VERSION 3.13)

PROJECT(TFBitsNPieces)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER_ARCHITECTURE_ID, "x64")
#set(CMAKE_EXE_LINKER_FLAGS "--trace --verbose")
#set(CMAKE_VERBOSE_MAKEFILE ON)

set(
  file_h
#    CalcAboveBelow.h
    FirstOrDefaultCombiner.h
    FrameWork01.h
    GridColumnSizer.h
    InstrumentFilter.h
    InstrumentSelection.h
    IQFeedInstrumentBuild.h
    IQFeedSymbolFileToSqlite.h
    IQFeedSymbolListOps.h
    LiborFromIQFeed.h
    ReadCboeWeeklyOptionsCsv.h
    ReadCboeWeeklyOptionsXls.h
    TreeOps.h
    TreeOpsItems.h
  )

set(
  file_cpp
#    CalcAboveBelow.cpp
    FrameWork01.cpp
    GridColumnSizer.cpp
    InstrumentSelection.cpp
    IQFeedInstrumentBuild.cpp
    IQFeedSymbolListOps.cpp
    LiborFromIQFeed.cpp
    ReadCboeWeeklyOptionsCsv.cpp
    ReadCboeWeeklyOptionsXls.cpp
    TreeOps.cpp
    TreeOpsItems.cpp
  )

add_library(
  ${PROJECT_NAME} STATIC
  ${file_h}
  ${file_cpp}
  )

target_compile_definitions(${PROJECT_NAME} PUBLIC wxUSE_GUI )
#add_compile_options(`/usr/local/bin/wx-config --cxxflags`)
target_compile_definitions(${PROJECT_NAME} PUBLIC -D_FILE_OFFSET_BITS=64 )
target_compile_definitions(${PROJECT_NAME} PUBLIC -DWXUSINGDLL )
target_compile_definitions(${PROJECT_NAME} PUBLIC -D__WXGTK__ )

target_include_directories(
  ${PROJECT_NAME} PUBLIC
    ".."
    "/usr/local/lib/wx/include/gtk3-unicode-3.0"
    "/usr/local/include/wx-3.0"
  )

