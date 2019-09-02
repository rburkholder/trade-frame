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
	${OBJECTDIR}/Capture.o \
	${OBJECTDIR}/IntervalSampler.o \
	${OBJECTDIR}/ReadSymbolFile.o


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
LDLIBSOPTIONS=../lib/TFBitsNPieces/dist/Debug/GNU-Linux/libtfbitsnpieces.a ../lib/TFIQFeed/dist/Debug/GNU-Linux/libtfiqfeed.a ../lib/TFTimeSeries/dist/Debug/GNU-Linux/libtftimeseries.a ../lib/TFVuTrading/dist/Debug/GNU-Linux/libtfvutrading.a ../lib/TFTrading/dist/Debug/GNU-Linux/libtftrading.a ../lib/TFHDF5TimeSeries/dist/Debug/GNU-Linux/libtfhdf5timeseries.a ../lib/OUCommon/dist/Debug/GNU-Linux/liboucommon.a -lboost_system-gcc8-mt-x64-1_69 -lboost_date_time-gcc8-mt-x64-1_69 -lboost_filesystem-gcc8-mt-x64-1_69 -lboost_serialization-gcc8-mt-x64-1_69 -lboost_thread-gcc8-mt-x64-1_69 -lboost_regex-gcc8-mt-x64-1_69 -lhdf5_cpp -lhdf5 -lpthread -ldl -lz -lboost_program_options-gcc8-mt-x64-1_69

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/intervalsampler

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/intervalsampler: ../lib/TFBitsNPieces/dist/Debug/GNU-Linux/libtfbitsnpieces.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/intervalsampler: ../lib/TFIQFeed/dist/Debug/GNU-Linux/libtfiqfeed.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/intervalsampler: ../lib/TFTimeSeries/dist/Debug/GNU-Linux/libtftimeseries.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/intervalsampler: ../lib/TFVuTrading/dist/Debug/GNU-Linux/libtfvutrading.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/intervalsampler: ../lib/TFTrading/dist/Debug/GNU-Linux/libtftrading.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/intervalsampler: ../lib/TFHDF5TimeSeries/dist/Debug/GNU-Linux/libtfhdf5timeseries.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/intervalsampler: ../lib/OUCommon/dist/Debug/GNU-Linux/liboucommon.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/intervalsampler: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/intervalsampler ${OBJECTFILES} ${LDLIBSOPTIONS} `/usr/local/bin/wx-config --libs`

${OBJECTDIR}/Capture.o: Capture.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../lib -std=c++17 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Capture.o Capture.cpp

${OBJECTDIR}/IntervalSampler.o: IntervalSampler.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../lib -std=c++17 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/IntervalSampler.o IntervalSampler.cpp

${OBJECTDIR}/ReadSymbolFile.o: ReadSymbolFile.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../lib -std=c++17 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ReadSymbolFile.o ReadSymbolFile.cpp

# Subprojects
.build-subprojects:
	cd ../lib/TFBitsNPieces && ${MAKE} -j2 -f Makefile CONF=Debug
	cd ../lib/TFIQFeed && ${MAKE} -j2 -f Makefile CONF=Debug
	cd ../lib/TFTimeSeries && ${MAKE} -j2 -f Makefile CONF=Debug
	cd ../lib/TFVuTrading && ${MAKE} -j2 -f Makefile CONF=Debug
	cd ../lib/TFTrading && ${MAKE} -j2 -f Makefile CONF=Debug
	cd ../lib/TFHDF5TimeSeries && ${MAKE} -j2 -f Makefile CONF=Debug
	cd ../lib/OUCommon && ${MAKE} -j2 -f Makefile CONF=Debug

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:
	cd ../lib/TFBitsNPieces && ${MAKE} -j2 -f Makefile CONF=Debug clean
	cd ../lib/TFIQFeed && ${MAKE} -j2 -f Makefile CONF=Debug clean
	cd ../lib/TFTimeSeries && ${MAKE} -j2 -f Makefile CONF=Debug clean
	cd ../lib/TFVuTrading && ${MAKE} -j2 -f Makefile CONF=Debug clean
	cd ../lib/TFTrading && ${MAKE} -j2 -f Makefile CONF=Debug clean
	cd ../lib/TFHDF5TimeSeries && ${MAKE} -j2 -f Makefile CONF=Debug clean
	cd ../lib/OUCommon && ${MAKE} -j2 -f Makefile CONF=Debug clean

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
