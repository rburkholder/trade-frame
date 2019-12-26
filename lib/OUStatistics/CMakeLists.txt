# trade-frame/lib/OUStatistics

PROJECT(OUStatistics)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER_ARCHITECTURE_ID, "x64")
#set(CMAKE_EXE_LINKER_FLAGS "--trace --verbose")
#set(CMAKE_VERBOSE_MAKEFILE ON)

set(
  file_h
    ADF.h
    NewMat/controlw.h
    NewMat/include.h
    NewMat/myexcept.h
    NewMat/newmatap.h
    NewMat/newmat.h
    NewMat/newmatio.h
    NewMat/newmatnl.h
    NewMat/newmatrc.h
    NewMat/newmatrm.h
    NewMat/precisio.h
  )

set(
  file_cpp
    ADF.cpp
    NewMat/bandmat.cpp
    NewMat/myexcept.cpp
    NewMat/newmat1.cpp
    NewMat/newmat2.cpp
    NewMat/newmat3.cpp
    NewMat/newmat4.cpp
    NewMat/newmat5.cpp
    NewMat/newmat6.cpp
    NewMat/newmat7.cpp
    NewMat/newmat8.cpp
    NewMat/newmat9.cpp
    NewMat/newmatex.cpp
    NewMat/newmatnl.cpp
    NewMat/newmatrm.cpp
    NewMat/submat.cpp
  )

add_library(
  ${PROJECT_NAME} STATIC
  ${file_h}
  ${file_cpp}
  )


