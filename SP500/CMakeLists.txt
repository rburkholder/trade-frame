# trade-frame/SP500
cmake_minimum_required (VERSION 3.18 FATAL_ERROR)

cmake_policy(SET CMP0104 NEW)
cmake_policy(SET CMP0105 NEW)

PROJECT(SP500)

#set(CMAKE_EXE_LINKER_FLAGS "--trace --verbose")
#set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CUDA_STANDARD 17)
set(CMAKE_CUDA_ARCHITECTURES native )
set(CMAKE_CUDA_COMPILER "/etc/alternatives/cuda/bin/nvcc" )
set(nvtx3_dir "/usr/local/cuda/targets/x86_64-linux/include/nvtx3" )

set(CMAKE_PREFIX_PATH "/usr/local/share/libtorch")
find_package(Torch REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")

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
    AppSP500_gui.hpp
    Config.hpp
    Features.hpp
    HyperParameters.hpp
    LSTM.hpp
    Model.hpp
    Strategy.hpp
    StrategyManager.hpp
    StrategyManager_impl.hpp
    Torch.hpp
  )

set(
  file_cpp
    AppSP500_gui.cpp
    Config.cpp
    Features.cpp
    LSTM.cpp
    Model.cpp
    Strategy.cpp
    StrategyManager.cpp
    StrategyManager_impl.cpp
    Torch.cpp
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
    "/usr/local/lib/wx/include/gtk3-unicode-${WX_VERSION_MAJOR}"
    "/usr/local/include/wx-${WX_VERSION_MAJOR}"
  )

target_link_directories(
  ${PROJECT_NAME} PUBLIC
    /usr/local/lib
  )

target_link_libraries(
  ${PROJECT_NAME}
      ${TORCH_LIBRARIES}
      TFBitsNPieces
      TFInteractiveBrokers
      TFSimulation
      TFVuTrading
      TFTrading
      TFOptions
      TFHDF5TimeSeries
      TFIQFeed
      TFIndicators
      TFTimeSeries
      TFTrading
      OUSQL
      OUSqlite
      OUCharting
      OUCommon
      dl
      z
      curl
      ${Boost_LIBRARIES}
#      wx_gtk3u_adv-3.1
      wx_gtk3u_core-${WX_VERSION_MAJOR}
      wx_baseu-${WX_VERSION_MAJOR}
      pthread
  )

