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
	${OBJECTDIR}/ChartDVBasics.o \
	${OBJECTDIR}/ChartDataView.o \
	${OBJECTDIR}/ChartEntryBars.o \
	${OBJECTDIR}/ChartEntryBase.o \
	${OBJECTDIR}/ChartEntryIndicator.o \
	${OBJECTDIR}/ChartEntryMark.o \
	${OBJECTDIR}/ChartEntrySegments.o \
	${OBJECTDIR}/ChartEntryShape.o \
	${OBJECTDIR}/ChartEntryVolume.o \
	${OBJECTDIR}/ChartMaster.o


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
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/liboucharting.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/liboucharting.a: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/liboucharting.a
	${AR} -rv ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/liboucharting.a ${OBJECTFILES} 
	$(RANLIB) ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/liboucharting.a

${OBJECTDIR}/ChartDVBasics.o: ChartDVBasics.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ChartDVBasics.o ChartDVBasics.cpp

${OBJECTDIR}/ChartDataView.o: ChartDataView.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ChartDataView.o ChartDataView.cpp

${OBJECTDIR}/ChartEntryBars.o: ChartEntryBars.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ChartEntryBars.o ChartEntryBars.cpp

${OBJECTDIR}/ChartEntryBase.o: ChartEntryBase.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ChartEntryBase.o ChartEntryBase.cpp

${OBJECTDIR}/ChartEntryIndicator.o: ChartEntryIndicator.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ChartEntryIndicator.o ChartEntryIndicator.cpp

${OBJECTDIR}/ChartEntryMark.o: ChartEntryMark.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ChartEntryMark.o ChartEntryMark.cpp

${OBJECTDIR}/ChartEntrySegments.o: ChartEntrySegments.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ChartEntrySegments.o ChartEntrySegments.cpp

${OBJECTDIR}/ChartEntryShape.o: ChartEntryShape.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ChartEntryShape.o ChartEntryShape.cpp

${OBJECTDIR}/ChartEntryVolume.o: ChartEntryVolume.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ChartEntryVolume.o ChartEntryVolume.cpp

${OBJECTDIR}/ChartMaster.o: ChartMaster.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ChartMaster.o ChartMaster.cpp

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
