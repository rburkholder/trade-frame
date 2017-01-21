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
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/EventIBInstrument.o \
	${OBJECTDIR}/IBSymbol.o \
	${OBJECTDIR}/IBTWS.o \
	${OBJECTDIR}/linux/EClientSocketBase.o \
	${OBJECTDIR}/linux/EPosixClientSocket.o \
	${OBJECTDIR}/linux/StdAfx.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-m64 `/usr/local/bin/wx-config --cxxflags`  
CXXFLAGS=-m64 `/usr/local/bin/wx-config --cxxflags`  

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libtfinteractivebrokers.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libtfinteractivebrokers.a: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libtfinteractivebrokers.a
	${AR} -rv ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libtfinteractivebrokers.a ${OBJECTFILES} 
	$(RANLIB) ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libtfinteractivebrokers.a

${OBJECTDIR}/EventIBInstrument.o: EventIBInstrument.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/EventIBInstrument.o EventIBInstrument.cpp

${OBJECTDIR}/IBSymbol.o: IBSymbol.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/IBSymbol.o IBSymbol.cpp

${OBJECTDIR}/IBTWS.o: IBTWS.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/IBTWS.o IBTWS.cpp

${OBJECTDIR}/linux/EClientSocketBase.o: linux/EClientSocketBase.cpp
	${MKDIR} -p ${OBJECTDIR}/linux
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/linux/EClientSocketBase.o linux/EClientSocketBase.cpp

${OBJECTDIR}/linux/EPosixClientSocket.o: linux/EPosixClientSocket.cpp
	${MKDIR} -p ${OBJECTDIR}/linux
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/linux/EPosixClientSocket.o linux/EPosixClientSocket.cpp

${OBJECTDIR}/linux/StdAfx.o: linux/StdAfx.cpp
	${MKDIR} -p ${OBJECTDIR}/linux
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/linux/StdAfx.o linux/StdAfx.cpp

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
