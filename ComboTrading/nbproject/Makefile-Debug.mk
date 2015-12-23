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
	${OBJECTDIR}/BundleTracking.o \
	${OBJECTDIR}/ComboTrading.o \
	${OBJECTDIR}/PanelCharts.o \
	${OBJECTDIR}/Process.o \
	${OBJECTDIR}/TreeItem.o \
	${OBJECTDIR}/TreeItemGroup.o \
	${OBJECTDIR}/TreeItemInstrument.o \
	${OBJECTDIR}/TreeItemPortfolio.o \
	${OBJECTDIR}/TreeItemPosition.o \
	${OBJECTDIR}/TreeOps.o \
	${OBJECTDIR}/UnderlyingSelection.o


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
LDLIBSOPTIONS=-L/usr/local/lib -Wl,-rpath,/usr/local/lib ../lib/TFBitsNPieces/dist/Debug/GNU-Linux-x86/libtfbitsnpieces.a ../lib/TFIQFeed/dist/Debug/GNU-Linux-x86/libtfiqfeed.a ../lib/TFInteractiveBrokers/dist/Debug/GNU-Linux-x86/libtfinteractivebrokers.a ../lib/TFSimulation/dist/Debug/GNU-Linux-x86/libtfsimulation.a ../lib/TFVuTrading/dist/Debug/GNU-Linux-x86/libtfvutrading.a ../lib/TFOptions/dist/Debug/GNU-Linux-x86/libtfoptions.a ../lib/TFTrading/dist/Debug/GNU-Linux-x86/libtftrading.a ../lib/TFTimeSeries/dist/Debug/GNU-Linux-x86/libtftimeseries.a ../lib/TFIndicators/dist/Debug/GNU-Linux-x86/libtfindicators.a ../lib/TFHDF5TimeSeries/dist/Debug/GNU-Linux-x86/libtfhdf5timeseries.a ../lib/TFTimeSeries/dist/Debug/GNU-Linux-x86/libtftimeseries.a ../lib/OUStatistics/dist/Debug/GNU-Linux-x86/liboustatistics.a ../lib/OUSqlite/dist/Debug/GNU-Linux-x86/libousqlite.a ../lib/OUSQL/dist/Debug/GNU-Linux-x86/libousql.a ../lib/OUCommon/dist/Debug/GNU-Linux-x86/liboucommon.a ../lib/OUCharting/dist/Debug/GNU-Linux-x86/liboucharting.a ../lib/ExcelFormat/dist/Debug/GNU-Linux-x86/libexcelformat.a ../lib/OUFormulas/dist/Debug/GNU-Linux-x86/libouformulas.a -lhdf5_cpp -lhdf5 -lsz -lpthread -ldl -lz -lcurl -lboost_system-gcc52-mt-d-1_59 -lboost_filesystem-gcc52-mt-d-1_59 -lboost_date_time-gcc52-mt-d-1_59 -lboost_serialization-gcc52-mt-d-1_59 -lboost_thread-gcc52-mt-d-1_59 -lboost_regex-gcc52-mt-d-1_59 -lchartdir

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/combotrading

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/combotrading: ../lib/TFBitsNPieces/dist/Debug/GNU-Linux-x86/libtfbitsnpieces.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/combotrading: ../lib/TFIQFeed/dist/Debug/GNU-Linux-x86/libtfiqfeed.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/combotrading: ../lib/TFInteractiveBrokers/dist/Debug/GNU-Linux-x86/libtfinteractivebrokers.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/combotrading: ../lib/TFSimulation/dist/Debug/GNU-Linux-x86/libtfsimulation.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/combotrading: ../lib/TFVuTrading/dist/Debug/GNU-Linux-x86/libtfvutrading.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/combotrading: ../lib/TFOptions/dist/Debug/GNU-Linux-x86/libtfoptions.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/combotrading: ../lib/TFTrading/dist/Debug/GNU-Linux-x86/libtftrading.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/combotrading: ../lib/TFTimeSeries/dist/Debug/GNU-Linux-x86/libtftimeseries.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/combotrading: ../lib/TFIndicators/dist/Debug/GNU-Linux-x86/libtfindicators.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/combotrading: ../lib/TFHDF5TimeSeries/dist/Debug/GNU-Linux-x86/libtfhdf5timeseries.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/combotrading: ../lib/TFTimeSeries/dist/Debug/GNU-Linux-x86/libtftimeseries.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/combotrading: ../lib/OUStatistics/dist/Debug/GNU-Linux-x86/liboustatistics.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/combotrading: ../lib/OUSqlite/dist/Debug/GNU-Linux-x86/libousqlite.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/combotrading: ../lib/OUSQL/dist/Debug/GNU-Linux-x86/libousql.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/combotrading: ../lib/OUCommon/dist/Debug/GNU-Linux-x86/liboucommon.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/combotrading: ../lib/OUCharting/dist/Debug/GNU-Linux-x86/liboucharting.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/combotrading: ../lib/ExcelFormat/dist/Debug/GNU-Linux-x86/libexcelformat.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/combotrading: ../lib/OUFormulas/dist/Debug/GNU-Linux-x86/libouformulas.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/combotrading: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/combotrading ${OBJECTFILES} ${LDLIBSOPTIONS} `/usr/local/bin/wx-config --libs` 

${OBJECTDIR}/BundleTracking.o: BundleTracking.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../lib -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/BundleTracking.o BundleTracking.cpp

${OBJECTDIR}/ComboTrading.o: ComboTrading.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../lib -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ComboTrading.o ComboTrading.cpp

${OBJECTDIR}/PanelCharts.o: PanelCharts.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../lib -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/PanelCharts.o PanelCharts.cpp

${OBJECTDIR}/Process.o: Process.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../lib -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Process.o Process.cpp

${OBJECTDIR}/TreeItem.o: TreeItem.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../lib -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TreeItem.o TreeItem.cpp

${OBJECTDIR}/TreeItemGroup.o: TreeItemGroup.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../lib -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TreeItemGroup.o TreeItemGroup.cpp

${OBJECTDIR}/TreeItemInstrument.o: TreeItemInstrument.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../lib -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TreeItemInstrument.o TreeItemInstrument.cpp

${OBJECTDIR}/TreeItemPortfolio.o: TreeItemPortfolio.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../lib -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TreeItemPortfolio.o TreeItemPortfolio.cpp

${OBJECTDIR}/TreeItemPosition.o: TreeItemPosition.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../lib -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TreeItemPosition.o TreeItemPosition.cpp

${OBJECTDIR}/TreeOps.o: TreeOps.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../lib -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TreeOps.o TreeOps.cpp

${OBJECTDIR}/UnderlyingSelection.o: UnderlyingSelection.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../lib -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/UnderlyingSelection.o UnderlyingSelection.cpp

# Subprojects
.build-subprojects:
	cd ../lib/TFBitsNPieces && ${MAKE}  -f Makefile CONF=Debug
	cd ../lib/TFIQFeed && ${MAKE}  -f Makefile CONF=Debug
	cd ../lib/TFInteractiveBrokers && ${MAKE}  -f Makefile CONF=Debug
	cd ../lib/TFSimulation && ${MAKE}  -f Makefile CONF=Debug
	cd ../lib/TFVuTrading && ${MAKE}  -f Makefile CONF=Debug
	cd ../lib/TFOptions && ${MAKE}  -f Makefile CONF=Debug
	cd ../lib/TFTrading && ${MAKE}  -f Makefile CONF=Debug
	cd ../lib/TFTimeSeries && ${MAKE}  -f Makefile CONF=Debug
	cd ../lib/TFIndicators && ${MAKE}  -f Makefile CONF=Debug
	cd ../lib/TFHDF5TimeSeries && ${MAKE}  -f Makefile CONF=Debug
	cd ../lib/TFTimeSeries && ${MAKE}  -f Makefile CONF=Debug
	cd ../lib/OUStatistics && ${MAKE}  -f Makefile CONF=Debug
	cd ../lib/OUSqlite && ${MAKE}  -f Makefile CONF=Debug
	cd ../lib/OUSQL && ${MAKE}  -f Makefile CONF=Debug
	cd ../lib/OUCommon && ${MAKE}  -f Makefile CONF=Debug
	cd ../lib/OUCharting && ${MAKE}  -f Makefile CONF=Debug
	cd ../lib/ExcelFormat && ${MAKE}  -f ExcelFormat-Makefile.mk CONF=Debug
	cd ../lib/OUFormulas && ${MAKE}  -f Makefile CONF=Debug

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/combotrading

# Subprojects
.clean-subprojects:
	cd ../lib/TFBitsNPieces && ${MAKE}  -f Makefile CONF=Debug clean
	cd ../lib/TFIQFeed && ${MAKE}  -f Makefile CONF=Debug clean
	cd ../lib/TFInteractiveBrokers && ${MAKE}  -f Makefile CONF=Debug clean
	cd ../lib/TFSimulation && ${MAKE}  -f Makefile CONF=Debug clean
	cd ../lib/TFVuTrading && ${MAKE}  -f Makefile CONF=Debug clean
	cd ../lib/TFOptions && ${MAKE}  -f Makefile CONF=Debug clean
	cd ../lib/TFTrading && ${MAKE}  -f Makefile CONF=Debug clean
	cd ../lib/TFTimeSeries && ${MAKE}  -f Makefile CONF=Debug clean
	cd ../lib/TFIndicators && ${MAKE}  -f Makefile CONF=Debug clean
	cd ../lib/TFHDF5TimeSeries && ${MAKE}  -f Makefile CONF=Debug clean
	cd ../lib/TFTimeSeries && ${MAKE}  -f Makefile CONF=Debug clean
	cd ../lib/OUStatistics && ${MAKE}  -f Makefile CONF=Debug clean
	cd ../lib/OUSqlite && ${MAKE}  -f Makefile CONF=Debug clean
	cd ../lib/OUSQL && ${MAKE}  -f Makefile CONF=Debug clean
	cd ../lib/OUCommon && ${MAKE}  -f Makefile CONF=Debug clean
	cd ../lib/OUCharting && ${MAKE}  -f Makefile CONF=Debug clean
	cd ../lib/ExcelFormat && ${MAKE}  -f ExcelFormat-Makefile.mk CONF=Debug clean
	cd ../lib/OUFormulas && ${MAKE}  -f Makefile CONF=Debug clean

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
