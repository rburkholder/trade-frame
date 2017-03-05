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
	${OBJECTDIR}/BundleTracking.o \
	${OBJECTDIR}/ComboTrading.o \
	${OBJECTDIR}/InstrumentActions.o \
	${OBJECTDIR}/PanelCharts.o \
	${OBJECTDIR}/Process.o \
	${OBJECTDIR}/TreeItem.o \
	${OBJECTDIR}/TreeItemGroup.o \
	${OBJECTDIR}/TreeItemInstrument.o \
	${OBJECTDIR}/TreeItemPortfolio.o \
	${OBJECTDIR}/TreeItemPosition.o \
	${OBJECTDIR}/TreeOps.o


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
LDLIBSOPTIONS=-L/usr/local/lib -Wl,-rpath,'/usr/local/lib' ../lib/TFBitsNPieces/dist/Release/GNU-Linux/libtfbitsnpieces.a ../lib/TFIQFeed/dist/Release/GNU-Linux/libtfiqfeed.a ../lib/TFInteractiveBrokers/dist/Release/GNU-Linux/libtfinteractivebrokers.a ../lib/TFSimulation/dist/Release/GNU-Linux/libtfsimulation.a ../lib/TFVuTrading/dist/Release/GNU-Linux/libtfvutrading.a ../lib/TFOptions/dist/Release/GNU-Linux/libtfoptions.a ../lib/TFTrading/dist/Release/GNU-Linux/libtftrading.a ../lib/TFTimeSeries/dist/Release/GNU-Linux/libtftimeseries.a ../lib/TFTimeSeries/dist/Release/GNU-Linux/libtftimeseries.a ../lib/TFIndicators/dist/Release/GNU-Linux/libtfindicators.a ../lib/TFHDF5TimeSeries/dist/Release/GNU-Linux/libtfhdf5timeseries.a ../lib/TFTimeSeries/dist/Release/GNU-Linux/libtftimeseries.a ../lib/OUStatistics/dist/Release/GNU-Linux/liboustatistics.a ../lib/OUSqlite/dist/Release/GNU-Linux/libousqlite.a ../lib/OUSQL/dist/Release/GNU-Linux/libousql.a ../lib/OUCommon/dist/Release/GNU-Linux/liboucommon.a ../lib/OUCharting/dist/Release/GNU-Linux/liboucharting.a ../lib/ExcelFormat/dist/Release/GNU-Linux/libexcelformat.a ../lib/OUFormulas/dist/Release/GNU-Linux/libouformulas.a -lhdf5_cpp -lhdf5 -lsz -lpthread -ldl -lz -lcurl -lchartdir -lboost_date_time-gcc61-mt-1_61 -lboost_filesystem-gcc61-mt-1_61 -lboost_regex-gcc61-mt-1_61 -lboost_serialization-gcc61-mt-1_61 -lboost_system-gcc61-mt-1_61 -lboost_thread-gcc61-mt-1_61

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/combotrading

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/combotrading: ../lib/TFBitsNPieces/dist/Release/GNU-Linux/libtfbitsnpieces.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/combotrading: ../lib/TFIQFeed/dist/Release/GNU-Linux/libtfiqfeed.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/combotrading: ../lib/TFInteractiveBrokers/dist/Release/GNU-Linux/libtfinteractivebrokers.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/combotrading: ../lib/TFSimulation/dist/Release/GNU-Linux/libtfsimulation.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/combotrading: ../lib/TFVuTrading/dist/Release/GNU-Linux/libtfvutrading.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/combotrading: ../lib/TFOptions/dist/Release/GNU-Linux/libtfoptions.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/combotrading: ../lib/TFTrading/dist/Release/GNU-Linux/libtftrading.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/combotrading: ../lib/TFTimeSeries/dist/Release/GNU-Linux/libtftimeseries.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/combotrading: ../lib/TFTimeSeries/dist/Release/GNU-Linux/libtftimeseries.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/combotrading: ../lib/TFIndicators/dist/Release/GNU-Linux/libtfindicators.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/combotrading: ../lib/TFHDF5TimeSeries/dist/Release/GNU-Linux/libtfhdf5timeseries.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/combotrading: ../lib/TFTimeSeries/dist/Release/GNU-Linux/libtftimeseries.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/combotrading: ../lib/OUStatistics/dist/Release/GNU-Linux/liboustatistics.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/combotrading: ../lib/OUSqlite/dist/Release/GNU-Linux/libousqlite.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/combotrading: ../lib/OUSQL/dist/Release/GNU-Linux/libousql.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/combotrading: ../lib/OUCommon/dist/Release/GNU-Linux/liboucommon.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/combotrading: ../lib/OUCharting/dist/Release/GNU-Linux/liboucharting.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/combotrading: ../lib/ExcelFormat/dist/Release/GNU-Linux/libexcelformat.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/combotrading: ../lib/OUFormulas/dist/Release/GNU-Linux/libouformulas.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/combotrading: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/combotrading ${OBJECTFILES} ${LDLIBSOPTIONS} `/usr/local/bin/wx-config --libs`

${OBJECTDIR}/BundleTracking.o: BundleTracking.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DwxUSE_GUI -I../lib -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/BundleTracking.o BundleTracking.cpp

${OBJECTDIR}/ComboTrading.o: ComboTrading.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DwxUSE_GUI -I../lib -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ComboTrading.o ComboTrading.cpp

${OBJECTDIR}/InstrumentActions.o: InstrumentActions.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DwxUSE_GUI -I../lib -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/InstrumentActions.o InstrumentActions.cpp

${OBJECTDIR}/PanelCharts.o: PanelCharts.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DwxUSE_GUI -I../lib -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/PanelCharts.o PanelCharts.cpp

${OBJECTDIR}/Process.o: Process.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DwxUSE_GUI -I../lib -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Process.o Process.cpp

${OBJECTDIR}/TreeItem.o: TreeItem.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DwxUSE_GUI -I../lib -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TreeItem.o TreeItem.cpp

${OBJECTDIR}/TreeItemGroup.o: TreeItemGroup.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DwxUSE_GUI -I../lib -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TreeItemGroup.o TreeItemGroup.cpp

${OBJECTDIR}/TreeItemInstrument.o: TreeItemInstrument.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DwxUSE_GUI -I../lib -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TreeItemInstrument.o TreeItemInstrument.cpp

${OBJECTDIR}/TreeItemPortfolio.o: TreeItemPortfolio.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DwxUSE_GUI -I../lib -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TreeItemPortfolio.o TreeItemPortfolio.cpp

${OBJECTDIR}/TreeItemPosition.o: TreeItemPosition.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DwxUSE_GUI -I../lib -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TreeItemPosition.o TreeItemPosition.cpp

${OBJECTDIR}/TreeOps.o: TreeOps.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DwxUSE_GUI -I../lib -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TreeOps.o TreeOps.cpp

# Subprojects
.build-subprojects:
	cd ../lib/TFBitsNPieces && ${MAKE}  -f Makefile CONF=Release
	cd ../lib/TFIQFeed && ${MAKE}  -f Makefile CONF=Release
	cd ../lib/TFInteractiveBrokers && ${MAKE}  -f Makefile CONF=Release
	cd ../lib/TFSimulation && ${MAKE}  -f Makefile CONF=Release
	cd ../lib/TFVuTrading && ${MAKE}  -f Makefile CONF=Release
	cd ../lib/TFOptions && ${MAKE}  -f Makefile CONF=Release
	cd ../lib/TFTrading && ${MAKE}  -f Makefile CONF=Release
	cd ../lib/TFTimeSeries && ${MAKE}  -f Makefile CONF=Release
	cd ../lib/TFTimeSeries && ${MAKE}  -f Makefile CONF=Release
	cd ../lib/TFIndicators && ${MAKE}  -f Makefile CONF=Release
	cd ../lib/TFHDF5TimeSeries && ${MAKE}  -f Makefile CONF=Release
	cd ../lib/TFTimeSeries && ${MAKE}  -f Makefile CONF=Release
	cd ../lib/OUStatistics && ${MAKE}  -f Makefile CONF=Release
	cd ../lib/OUSqlite && ${MAKE}  -f Makefile CONF=Release
	cd ../lib/OUSQL && ${MAKE}  -f Makefile CONF=Release
	cd ../lib/OUCommon && ${MAKE}  -f Makefile CONF=Release
	cd ../lib/OUCharting && ${MAKE}  -f Makefile CONF=Release
	cd ../lib/ExcelFormat && ${MAKE}  -f ExcelFormat-Makefile.mk CONF=Release
	cd ../lib/OUFormulas && ${MAKE}  -f Makefile CONF=Release

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:
	cd ../lib/TFBitsNPieces && ${MAKE}  -f Makefile CONF=Release clean
	cd ../lib/TFIQFeed && ${MAKE}  -f Makefile CONF=Release clean
	cd ../lib/TFInteractiveBrokers && ${MAKE}  -f Makefile CONF=Release clean
	cd ../lib/TFSimulation && ${MAKE}  -f Makefile CONF=Release clean
	cd ../lib/TFVuTrading && ${MAKE}  -f Makefile CONF=Release clean
	cd ../lib/TFOptions && ${MAKE}  -f Makefile CONF=Release clean
	cd ../lib/TFTrading && ${MAKE}  -f Makefile CONF=Release clean
	cd ../lib/TFTimeSeries && ${MAKE}  -f Makefile CONF=Release clean
	cd ../lib/TFTimeSeries && ${MAKE}  -f Makefile CONF=Release clean
	cd ../lib/TFIndicators && ${MAKE}  -f Makefile CONF=Release clean
	cd ../lib/TFHDF5TimeSeries && ${MAKE}  -f Makefile CONF=Release clean
	cd ../lib/TFTimeSeries && ${MAKE}  -f Makefile CONF=Release clean
	cd ../lib/OUStatistics && ${MAKE}  -f Makefile CONF=Release clean
	cd ../lib/OUSqlite && ${MAKE}  -f Makefile CONF=Release clean
	cd ../lib/OUSQL && ${MAKE}  -f Makefile CONF=Release clean
	cd ../lib/OUCommon && ${MAKE}  -f Makefile CONF=Release clean
	cd ../lib/OUCharting && ${MAKE}  -f Makefile CONF=Release clean
	cd ../lib/ExcelFormat && ${MAKE}  -f ExcelFormat-Makefile.mk CONF=Release clean
	cd ../lib/OUFormulas && ${MAKE}  -f Makefile CONF=Release clean

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
