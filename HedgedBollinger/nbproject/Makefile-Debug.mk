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
	${OBJECTDIR}/EventUpdateOptionTree.o \
	${OBJECTDIR}/HedgedBollinger.o \
	${OBJECTDIR}/PositionState.o \
	${OBJECTDIR}/Strategy1.o \
	${OBJECTDIR}/stdafx.o


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
LDLIBSOPTIONS=-L/usr/local/lib ../lib/TFBitsNPieces/dist/Debug/GNU-Linux/libtfbitsnpieces.a ../lib/TFVuTrading/dist/Debug/GNU-Linux/libtfvutrading.a ../lib/TFSimulation/dist/Debug/GNU-Linux/libtfsimulation.a ../lib/TFIQFeed/dist/Debug/GNU-Linux/libtfiqfeed.a ../lib/TFInteractiveBrokers/dist/Debug/GNU-Linux/libtfinteractivebrokers.a ../lib/TFTrading/dist/Debug/GNU-Linux/libtftrading.a ../lib/TFOptions/dist/Debug/GNU-Linux/libtfoptions.a ../lib/TFHDF5TimeSeries/dist/Debug/GNU-Linux/libtfhdf5timeseries.a ../lib/TFTimeSeries/dist/Debug/GNU-Linux/libtftimeseries.a ../lib/OUCommon/dist/Debug/GNU-Linux/liboucommon.a ../lib/OUSQL/dist/Debug/GNU-Linux/libousql.a ../lib/OUSqlite/dist/Debug/GNU-Linux/libousqlite.a ../lib/OUCharting/dist/Debug/GNU-Linux/liboucharting.a ../lib/TFIndicators/dist/Debug/GNU-Linux/libtfindicators.a -lboost_system-gcc61-mt-1_61 -lboost_date_time-gcc61-mt-1_61 -lboost_filesystem-gcc61-mt-1_61 -lboost_serialization-gcc61-mt-1_61 -lboost_thread-gcc61-mt-1_61 -lboost_regex-gcc61-mt-1_61 -lhdf5_cpp -lhdf5 -lsz -lcurl -lpthread -ldl -lz -lchartdir

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/hedgedbollinger

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/hedgedbollinger: ../lib/TFBitsNPieces/dist/Debug/GNU-Linux/libtfbitsnpieces.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/hedgedbollinger: ../lib/TFVuTrading/dist/Debug/GNU-Linux/libtfvutrading.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/hedgedbollinger: ../lib/TFSimulation/dist/Debug/GNU-Linux/libtfsimulation.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/hedgedbollinger: ../lib/TFIQFeed/dist/Debug/GNU-Linux/libtfiqfeed.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/hedgedbollinger: ../lib/TFInteractiveBrokers/dist/Debug/GNU-Linux/libtfinteractivebrokers.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/hedgedbollinger: ../lib/TFTrading/dist/Debug/GNU-Linux/libtftrading.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/hedgedbollinger: ../lib/TFOptions/dist/Debug/GNU-Linux/libtfoptions.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/hedgedbollinger: ../lib/TFHDF5TimeSeries/dist/Debug/GNU-Linux/libtfhdf5timeseries.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/hedgedbollinger: ../lib/TFTimeSeries/dist/Debug/GNU-Linux/libtftimeseries.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/hedgedbollinger: ../lib/OUCommon/dist/Debug/GNU-Linux/liboucommon.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/hedgedbollinger: ../lib/OUSQL/dist/Debug/GNU-Linux/libousql.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/hedgedbollinger: ../lib/OUSqlite/dist/Debug/GNU-Linux/libousqlite.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/hedgedbollinger: ../lib/OUCharting/dist/Debug/GNU-Linux/liboucharting.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/hedgedbollinger: ../lib/TFIndicators/dist/Debug/GNU-Linux/libtfindicators.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/hedgedbollinger: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/hedgedbollinger ${OBJECTFILES} ${LDLIBSOPTIONS} `/usr/local/bin/wx-config --libs`

${OBJECTDIR}/EventUpdateOptionTree.o: EventUpdateOptionTree.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../lib -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/EventUpdateOptionTree.o EventUpdateOptionTree.cpp

${OBJECTDIR}/HedgedBollinger.o: HedgedBollinger.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../lib -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/HedgedBollinger.o HedgedBollinger.cpp

${OBJECTDIR}/PositionState.o: PositionState.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../lib -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/PositionState.o PositionState.cpp

${OBJECTDIR}/Strategy1.o: Strategy1.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../lib -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Strategy1.o Strategy1.cpp

${OBJECTDIR}/stdafx.o: stdafx.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../lib -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/stdafx.o stdafx.cpp

# Subprojects
.build-subprojects:
	cd ../lib/TFBitsNPieces && ${MAKE}  -f Makefile CONF=Debug
	cd ../lib/TFVuTrading && ${MAKE}  -f Makefile CONF=Debug
	cd ../lib/TFSimulation && ${MAKE}  -f Makefile CONF=Debug
	cd ../lib/TFIQFeed && ${MAKE}  -f Makefile CONF=Debug
	cd ../lib/TFInteractiveBrokers && ${MAKE}  -f Makefile CONF=Debug
	cd ../lib/TFTrading && ${MAKE}  -f Makefile CONF=Debug
	cd ../lib/TFOptions && ${MAKE}  -f Makefile CONF=Debug
	cd ../lib/TFHDF5TimeSeries && ${MAKE}  -f Makefile CONF=Debug
	cd ../lib/TFTimeSeries && ${MAKE}  -f Makefile CONF=Debug
	cd ../lib/OUCommon && ${MAKE}  -f Makefile CONF=Debug
	cd ../lib/OUSQL && ${MAKE}  -f Makefile CONF=Debug
	cd ../lib/OUSqlite && ${MAKE}  -f Makefile CONF=Debug
	cd ../lib/OUCharting && ${MAKE}  -f Makefile CONF=Debug
	cd ../lib/TFIndicators && ${MAKE}  -f Makefile CONF=Debug

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:
	cd ../lib/TFBitsNPieces && ${MAKE}  -f Makefile CONF=Debug clean
	cd ../lib/TFVuTrading && ${MAKE}  -f Makefile CONF=Debug clean
	cd ../lib/TFSimulation && ${MAKE}  -f Makefile CONF=Debug clean
	cd ../lib/TFIQFeed && ${MAKE}  -f Makefile CONF=Debug clean
	cd ../lib/TFInteractiveBrokers && ${MAKE}  -f Makefile CONF=Debug clean
	cd ../lib/TFTrading && ${MAKE}  -f Makefile CONF=Debug clean
	cd ../lib/TFOptions && ${MAKE}  -f Makefile CONF=Debug clean
	cd ../lib/TFHDF5TimeSeries && ${MAKE}  -f Makefile CONF=Debug clean
	cd ../lib/TFTimeSeries && ${MAKE}  -f Makefile CONF=Debug clean
	cd ../lib/OUCommon && ${MAKE}  -f Makefile CONF=Debug clean
	cd ../lib/OUSQL && ${MAKE}  -f Makefile CONF=Debug clean
	cd ../lib/OUSqlite && ${MAKE}  -f Makefile CONF=Debug clean
	cd ../lib/OUCharting && ${MAKE}  -f Makefile CONF=Debug clean
	cd ../lib/TFIndicators && ${MAKE}  -f Makefile CONF=Debug clean

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
