# trade-frame/lib/TFHDF5TimeSeries
cmake_minimum_required (VERSION 3.13)

PROJECT(TFHDF5TimeSeries)

#set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_COMPILER_ARCHITECTURE_ID, "x64")
#set(CMAKE_EXE_LINKER_FLAGS "--trace --verbose")
#set(CMAKE_VERBOSE_MAKEFILE ON)

set(
  file_h
    HDF5Attribute.h
    HDF5DataManager.h
    HDF5IterateGroups.h
    HDF5TimeSeriesAccessor.h
    HDF5TimeSeriesContainer.h
    HDF5TimeSeriesIterator.h
    HDF5WriteTimeSeries.h
  )

set(
  file_cpp
    HDF5Attribute.cpp
    HDF5DataManager.cpp
  )

add_library(
  ${PROJECT_NAME}
  ${file_h}
  ${file_cpp}
  )

target_compile_definitions(${PROJECT_NAME} PUBLIC BOOST_LOG_DYN_LINK )

target_include_directories(
  ${PROJECT_NAME} PRIVATE
    ".."
  )

target_link_libraries(
  ${PROJECT_NAME} PRIVATE
    hdf5_cpp
    hdf5
    sz
)
