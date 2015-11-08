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
CND_PLATFORM=GNU-Linux-x86
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
	${OBJECTDIR}/ArmsChart.o \
	${OBJECTDIR}/ArmsIndex.o \
	${OBJECTDIR}/EventProviderConnected.o \
	${OBJECTDIR}/IndicatorPackage.o \
	${OBJECTDIR}/PanelArmsIndex.o \
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
LDLIBSOPTIONS=-L/usr/local/lib -Wl,-rpath,/usr/local/lib ../lib/TFBitsNPieces/dist/Debug/GNU-Linux-x86/libtfbitsnpieces.a ../lib/TFSimulation/dist/Debug/GNU-Linux-x86/libtfsimulation.a ../lib/TFOptions/dist/Debug/GNU-Linux-x86/libtfoptions.a ../lib/TFIQFeed/dist/Debug/GNU-Linux-x86/libtfiqfeed.a ../lib/TFInteractiveBrokers/dist/Debug/GNU-Linux-x86/libtfinteractivebrokers.a ../lib/TFIndicators/dist/Debug/GNU-Linux-x86/libtfindicators.a ../lib/TFHDF5TimeSeries/dist/Debug/GNU-Linux-x86/libtfhdf5timeseries.a ../lib/TFTrading/dist/Debug/GNU-Linux-x86/libtftrading.a ../lib/TFTimeSeries/dist/Debug/GNU-Linux-x86/libtftimeseries.a ../lib/TFVuTrading/dist/Debug/GNU-Linux-x86/libtfvutrading.a ../lib/OUCommon/dist/Debug/GNU-Linux-x86/liboucommon.a ../lib/OUSqlite/dist/Debug/GNU-Linux-x86/libousqlite.a ../lib/OUSQL/dist/Debug/GNU-Linux-x86/libousql.a ../lib/OUCharting/dist/Debug/GNU-Linux-x86/liboucharting.a -lhdf5_cpp -lhdf5 -lsz -lpthread -ldl -lz -lcurl -lboost_system-gcc52-mt-d-1_59 -lboost_filesystem-gcc52-mt-d-1_59 -lboost_date_time-gcc52-mt-d-1_59 -lboost_serialization-gcc52-mt-d-1_59 -lboost_thread-gcc52-mt-d-1_59 -lboost_regex-gcc52-mt-d-1_59 -lchartdir

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/armsindex

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/armsindex: ../lib/TFBitsNPieces/dist/Debug/GNU-Linux-x86/libtfbitsnpieces.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/armsindex: ../lib/TFSimulation/dist/Debug/GNU-Linux-x86/libtfsimulation.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/armsindex: ../lib/TFOptions/dist/Debug/GNU-Linux-x86/libtfoptions.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/armsindex: ../lib/TFIQFeed/dist/Debug/GNU-Linux-x86/libtfiqfeed.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/armsindex: ../lib/TFInteractiveBrokers/dist/Debug/GNU-Linux-x86/libtfinteractivebrokers.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/armsindex: ../lib/TFIndicators/dist/Debug/GNU-Linux-x86/libtfindicators.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/armsindex: ../lib/TFHDF5TimeSeries/dist/Debug/GNU-Linux-x86/libtfhdf5timeseries.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/armsindex: ../lib/TFTrading/dist/Debug/GNU-Linux-x86/libtftrading.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/armsindex: ../lib/TFTimeSeries/dist/Debug/GNU-Linux-x86/libtftimeseries.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/armsindex: ../lib/TFVuTrading/dist/Debug/GNU-Linux-x86/libtfvutrading.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/armsindex: ../lib/OUCommon/dist/Debug/GNU-Linux-x86/liboucommon.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/armsindex: ../lib/OUSqlite/dist/Debug/GNU-Linux-x86/libousqlite.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/armsindex: ../lib/OUSQL/dist/Debug/GNU-Linux-x86/libousql.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/armsindex: ../lib/OUCharting/dist/Debug/GNU-Linux-x86/liboucharting.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/armsindex: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/armsindex ${OBJECTFILES} ${LDLIBSOPTIONS} `/usr/local/bin/wx-config --libs` 

${OBJECTDIR}/ArmsChart.o: ArmsChart.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../lib -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ArmsChart.o ArmsChart.cpp

${OBJECTDIR}/ArmsIndex.o: ArmsIndex.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../lib -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ArmsIndex.o ArmsIndex.cpp

${OBJECTDIR}/EventProviderConnected.o: EventProviderConnected.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../lib -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/EventProviderConnected.o EventProviderConnected.cpp

${OBJECTDIR}/IndicatorPackage.o: IndicatorPackage.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../lib -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/IndicatorPackage.o IndicatorPackage.cpp

${OBJECTDIR}/PanelArmsIndex.o: PanelArmsIndex.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../lib -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/PanelArmsIndex.o PanelArmsIndex.cpp

${OBJECTDIR}/stdafx.o: stdafx.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../lib -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/stdafx.o stdafx.cpp

# Subprojects
.build-subprojects:
	cd ../lib/TFBitsNPieces && ${MAKE}  -f Makefile CONF=Debug
	cd ../lib/TFSimulation && ${MAKE}  -f Makefile CONF=Debug
	cd ../lib/TFOptions && ${MAKE}  -f Makefile CONF=Debug
	cd ../lib/TFIQFeed && ${MAKE}  -f Makefile CONF=Debug
	cd ../lib/TFInteractiveBrokers && ${MAKE}  -f Makefile CONF=Debug
	cd ../lib/TFIndicators && ${MAKE}  -f Makefile CONF=Debug
	cd ../lib/TFHDF5TimeSeries && ${MAKE}  -f Makefile CONF=Debug
	cd ../lib/TFTrading && ${MAKE}  -f Makefile CONF=Debug
	cd ../lib/TFTimeSeries && ${MAKE}  -f Makefile CONF=Debug
	cd ../lib/TFVuTrading && ${MAKE}  -f Makefile CONF=Debug
	cd ../lib/OUCommon && ${MAKE}  -f Makefile CONF=Debug
	cd ../lib/OUSqlite && ${MAKE}  -f Makefile CONF=Debug
	cd ../lib/OUSQL && ${MAKE}  -f Makefile CONF=Debug
	cd ../lib/OUCharting && ${MAKE}  -f Makefile CONF=Debug

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/armsindex

# Subprojects
.clean-subprojects:
	cd ../lib/TFBitsNPieces && ${MAKE}  -f Makefile CONF=Debug clean
	cd ../lib/TFSimulation && ${MAKE}  -f Makefile CONF=Debug clean
	cd ../lib/TFOptions && ${MAKE}  -f Makefile CONF=Debug clean
	cd ../lib/TFIQFeed && ${MAKE}  -f Makefile CONF=Debug clean
	cd ../lib/TFInteractiveBrokers && ${MAKE}  -f Makefile CONF=Debug clean
	cd ../lib/TFIndicators && ${MAKE}  -f Makefile CONF=Debug clean
	cd ../lib/TFHDF5TimeSeries && ${MAKE}  -f Makefile CONF=Debug clean
	cd ../lib/TFTrading && ${MAKE}  -f Makefile CONF=Debug clean
	cd ../lib/TFTimeSeries && ${MAKE}  -f Makefile CONF=Debug clean
	cd ../lib/TFVuTrading && ${MAKE}  -f Makefile CONF=Debug clean
	cd ../lib/OUCommon && ${MAKE}  -f Makefile CONF=Debug clean
	cd ../lib/OUSqlite && ${MAKE}  -f Makefile CONF=Debug clean
	cd ../lib/OUSQL && ${MAKE}  -f Makefile CONF=Debug clean
	cd ../lib/OUCharting && ${MAKE}  -f Makefile CONF=Debug clean

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
