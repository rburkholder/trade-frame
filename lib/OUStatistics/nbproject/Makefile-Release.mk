#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux
CND_DLIB_EXT=so
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/ADF.o \
	${OBJECTDIR}/NewMat/bandmat.o \
	${OBJECTDIR}/NewMat/myexcept.o \
	${OBJECTDIR}/NewMat/newmat1.o \
	${OBJECTDIR}/NewMat/newmat2.o \
	${OBJECTDIR}/NewMat/newmat3.o \
	${OBJECTDIR}/NewMat/newmat4.o \
	${OBJECTDIR}/NewMat/newmat5.o \
	${OBJECTDIR}/NewMat/newmat6.o \
	${OBJECTDIR}/NewMat/newmat7.o \
	${OBJECTDIR}/NewMat/newmat8.o \
	${OBJECTDIR}/NewMat/newmat9.o \
	${OBJECTDIR}/NewMat/newmatex.o \
	${OBJECTDIR}/NewMat/newmatnl.o \
	${OBJECTDIR}/NewMat/newmatrm.o \
	${OBJECTDIR}/NewMat/submat.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-m64
CXXFLAGS=-m64

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/liboustatistics.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/liboustatistics.a: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/liboustatistics.a
	${AR} -rv ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/liboustatistics.a ${OBJECTFILES} 
	$(RANLIB) ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/liboustatistics.a

${OBJECTDIR}/ADF.o: ADF.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Duse_namespace -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ADF.o ADF.cpp

${OBJECTDIR}/NewMat/bandmat.o: NewMat/bandmat.cpp
	${MKDIR} -p ${OBJECTDIR}/NewMat
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Duse_namespace -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/NewMat/bandmat.o NewMat/bandmat.cpp

${OBJECTDIR}/NewMat/myexcept.o: NewMat/myexcept.cpp
	${MKDIR} -p ${OBJECTDIR}/NewMat
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Duse_namespace -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/NewMat/myexcept.o NewMat/myexcept.cpp

${OBJECTDIR}/NewMat/newmat1.o: NewMat/newmat1.cpp
	${MKDIR} -p ${OBJECTDIR}/NewMat
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Duse_namespace -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/NewMat/newmat1.o NewMat/newmat1.cpp

${OBJECTDIR}/NewMat/newmat2.o: NewMat/newmat2.cpp
	${MKDIR} -p ${OBJECTDIR}/NewMat
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Duse_namespace -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/NewMat/newmat2.o NewMat/newmat2.cpp

${OBJECTDIR}/NewMat/newmat3.o: NewMat/newmat3.cpp
	${MKDIR} -p ${OBJECTDIR}/NewMat
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Duse_namespace -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/NewMat/newmat3.o NewMat/newmat3.cpp

${OBJECTDIR}/NewMat/newmat4.o: NewMat/newmat4.cpp
	${MKDIR} -p ${OBJECTDIR}/NewMat
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Duse_namespace -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/NewMat/newmat4.o NewMat/newmat4.cpp

${OBJECTDIR}/NewMat/newmat5.o: NewMat/newmat5.cpp
	${MKDIR} -p ${OBJECTDIR}/NewMat
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Duse_namespace -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/NewMat/newmat5.o NewMat/newmat5.cpp

${OBJECTDIR}/NewMat/newmat6.o: NewMat/newmat6.cpp
	${MKDIR} -p ${OBJECTDIR}/NewMat
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Duse_namespace -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/NewMat/newmat6.o NewMat/newmat6.cpp

${OBJECTDIR}/NewMat/newmat7.o: NewMat/newmat7.cpp
	${MKDIR} -p ${OBJECTDIR}/NewMat
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Duse_namespace -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/NewMat/newmat7.o NewMat/newmat7.cpp

${OBJECTDIR}/NewMat/newmat8.o: NewMat/newmat8.cpp
	${MKDIR} -p ${OBJECTDIR}/NewMat
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Duse_namespace -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/NewMat/newmat8.o NewMat/newmat8.cpp

${OBJECTDIR}/NewMat/newmat9.o: NewMat/newmat9.cpp
	${MKDIR} -p ${OBJECTDIR}/NewMat
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Duse_namespace -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/NewMat/newmat9.o NewMat/newmat9.cpp

${OBJECTDIR}/NewMat/newmatex.o: NewMat/newmatex.cpp
	${MKDIR} -p ${OBJECTDIR}/NewMat
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Duse_namespace -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/NewMat/newmatex.o NewMat/newmatex.cpp

${OBJECTDIR}/NewMat/newmatnl.o: NewMat/newmatnl.cpp
	${MKDIR} -p ${OBJECTDIR}/NewMat
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Duse_namespace -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/NewMat/newmatnl.o NewMat/newmatnl.cpp

${OBJECTDIR}/NewMat/newmatrm.o: NewMat/newmatrm.cpp
	${MKDIR} -p ${OBJECTDIR}/NewMat
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Duse_namespace -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/NewMat/newmatrm.o NewMat/newmatrm.cpp

${OBJECTDIR}/NewMat/submat.o: NewMat/submat.cpp
	${MKDIR} -p ${OBJECTDIR}/NewMat
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Duse_namespace -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/NewMat/submat.o NewMat/submat.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
