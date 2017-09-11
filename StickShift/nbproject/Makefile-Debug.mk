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
	${OBJECTDIR}/StickShift.o \
	${OBJECTDIR}/stdafx.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=`/usr/local/bin/wx-config --cxxflags` 
CXXFLAGS=`/usr/local/bin/wx-config --cxxflags` 

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=../lib/TFVuTrading/dist/Debug/GNU-Linux/libtfvutrading.a ../lib/TFSimulation/dist/Debug/GNU-Linux/libtfsimulation.a ../lib/TFIQFeed/dist/Debug/GNU-Linux/libtfiqfeed.a ../lib/TFInteractiveBrokers/dist/Debug/GNU-Linux/libtfinteractivebrokers.a ../lib/TFTrading/dist/Debug/GNU-Linux/libtftrading.a ../lib/TFOptions/dist/Debug/GNU-Linux/libtfoptions.a ../lib/TFHDF5TimeSeries/dist/Debug/GNU-Linux/libtfhdf5timeseries.a ../lib/TFTimeSeries/dist/Debug/GNU-Linux/libtftimeseries.a ../lib/OUCommon/dist/Debug/GNU-Linux/liboucommon.a ../lib/OUSQL/dist/Debug/GNU-Linux/libousql.a ../lib/OUSqlite/dist/Debug/GNU-Linux/libousqlite.a -lhdf5_cpp -lhdf5 -lsz -lpthread -ldl -lz -lcurl -lboost_date_time-gcc63-mt-1_61 -lboost_filesystem-gcc63-mt-1_61 -lboost_regex-gcc63-mt-1_61 -lboost_serialization-gcc63-mt-1_61 -lboost_system-gcc63-mt-1_61 -lboost_thread-gcc63-mt-1_61

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/stickshift

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/stickshift: ../lib/TFVuTrading/dist/Debug/GNU-Linux/libtfvutrading.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/stickshift: ../lib/TFSimulation/dist/Debug/GNU-Linux/libtfsimulation.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/stickshift: ../lib/TFIQFeed/dist/Debug/GNU-Linux/libtfiqfeed.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/stickshift: ../lib/TFInteractiveBrokers/dist/Debug/GNU-Linux/libtfinteractivebrokers.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/stickshift: ../lib/TFTrading/dist/Debug/GNU-Linux/libtftrading.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/stickshift: ../lib/TFOptions/dist/Debug/GNU-Linux/libtfoptions.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/stickshift: ../lib/TFHDF5TimeSeries/dist/Debug/GNU-Linux/libtfhdf5timeseries.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/stickshift: ../lib/TFTimeSeries/dist/Debug/GNU-Linux/libtftimeseries.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/stickshift: ../lib/OUCommon/dist/Debug/GNU-Linux/liboucommon.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/stickshift: ../lib/OUSQL/dist/Debug/GNU-Linux/libousql.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/stickshift: ../lib/OUSqlite/dist/Debug/GNU-Linux/libousqlite.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/stickshift: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/stickshift ${OBJECTFILES} ${LDLIBSOPTIONS} `/usr/local/bin/wx-config --libs`

${OBJECTDIR}/StickShift.o: StickShift.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../lib -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/StickShift.o StickShift.cpp

${OBJECTDIR}/stdafx.o: stdafx.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../lib -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/stdafx.o stdafx.cpp

# Subprojects
.build-subprojects:
	cd ../lib/TFVuTrading && ${MAKE} -j3 -f Makefile CONF=Debug
	cd ../lib/TFSimulation && ${MAKE} -j3 -f Makefile CONF=Debug
	cd ../lib/TFIQFeed && ${MAKE} -j3 -f Makefile CONF=Debug
	cd ../lib/TFInteractiveBrokers && ${MAKE} -j3 -f Makefile CONF=Debug
	cd ../lib/TFTrading && ${MAKE} -j3 -f Makefile CONF=Debug
	cd ../lib/TFOptions && ${MAKE} -j3 -f Makefile CONF=Debug
	cd ../lib/TFHDF5TimeSeries && ${MAKE} -j3 -f Makefile CONF=Debug
	cd ../lib/TFTimeSeries && ${MAKE} -j3 -f Makefile CONF=Debug
	cd ../lib/OUCommon && ${MAKE} -j3 -f Makefile CONF=Debug
	cd ../lib/OUSQL && ${MAKE} -j3 -f Makefile CONF=Debug
	cd ../lib/OUSqlite && ${MAKE} -j3 -f Makefile CONF=Debug

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:
	cd ../lib/TFVuTrading && ${MAKE} -j3 -f Makefile CONF=Debug clean
	cd ../lib/TFSimulation && ${MAKE} -j3 -f Makefile CONF=Debug clean
	cd ../lib/TFIQFeed && ${MAKE} -j3 -f Makefile CONF=Debug clean
	cd ../lib/TFInteractiveBrokers && ${MAKE} -j3 -f Makefile CONF=Debug clean
	cd ../lib/TFTrading && ${MAKE} -j3 -f Makefile CONF=Debug clean
	cd ../lib/TFOptions && ${MAKE} -j3 -f Makefile CONF=Debug clean
	cd ../lib/TFHDF5TimeSeries && ${MAKE} -j3 -f Makefile CONF=Debug clean
	cd ../lib/TFTimeSeries && ${MAKE} -j3 -f Makefile CONF=Debug clean
	cd ../lib/OUCommon && ${MAKE} -j3 -f Makefile CONF=Debug clean
	cd ../lib/OUSQL && ${MAKE} -j3 -f Makefile CONF=Debug clean
	cd ../lib/OUSqlite && ${MAKE} -j3 -f Makefile CONF=Debug clean

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
