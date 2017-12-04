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
	${OBJECTDIR}/BasketTrading.o \
	${OBJECTDIR}/ManagePortfolio.o \
	${OBJECTDIR}/PanelBasketTradingMain.o \
	${OBJECTDIR}/PanelPortfolioStats.o \
	${OBJECTDIR}/Position.o \
	${OBJECTDIR}/SymbolSelection.o \
	${OBJECTDIR}/Worker.o \
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
LDLIBSOPTIONS=-L/usr/local/lib -Wl,-rpath,'/usr/local/lib' ../lib/TFBitsNPieces/dist/Debug/GNU-Linux/libtfbitsnpieces.a ../lib/TFIQFeed/dist/Debug/GNU-Linux/libtfiqfeed.a ../lib/TFInteractiveBrokers/dist/Debug/GNU-Linux/libtfinteractivebrokers.a ../lib/TFSimulation/dist/Debug/GNU-Linux/libtfsimulation.a ../lib/TFVuTrading/dist/Debug/GNU-Linux/libtfvutrading.a ../lib/TFOptions/dist/Debug/GNU-Linux/libtfoptions.a ../lib/TFTrading/dist/Debug/GNU-Linux/libtftrading.a ../lib/TFTimeSeries/dist/Debug/GNU-Linux/libtftimeseries.a ../lib/OUCharting/dist/Debug/GNU-Linux/liboucharting.a ../lib/TFIndicators/dist/Debug/GNU-Linux/libtfindicators.a ../lib/TFHDF5TimeSeries/dist/Debug/GNU-Linux/libtfhdf5timeseries.a ../lib/TFTimeSeries/dist/Debug/GNU-Linux/libtftimeseries.a ../lib/OUStatistics/dist/Debug/GNU-Linux/liboustatistics.a ../lib/OUSqlite/dist/Debug/GNU-Linux/libousqlite.a ../lib/OUSQL/dist/Debug/GNU-Linux/libousql.a ../lib/OUCommon/dist/Debug/GNU-Linux/liboucommon.a ../lib/ExcelFormat/dist/Debug/GNU-Linux/libexcelformat.a ../lib/OUFormulas/dist/Debug/GNU-Linux/libouformulas.a -lhdf5_cpp -lhdf5 -lsz -lpthread -ldl -lz -lcurl -lchartdir -lboost_date_time-gcc-mt-1_61 -lboost_filesystem-gcc-mt-1_61 -lboost_regex-gcc-mt-1_61 -lboost_serialization-gcc-mt-1_61 -lboost_system-gcc-mt-1_61 -lboost_thread-gcc-mt-1_61

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/baskettrading

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/baskettrading: ../lib/TFBitsNPieces/dist/Debug/GNU-Linux/libtfbitsnpieces.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/baskettrading: ../lib/TFIQFeed/dist/Debug/GNU-Linux/libtfiqfeed.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/baskettrading: ../lib/TFInteractiveBrokers/dist/Debug/GNU-Linux/libtfinteractivebrokers.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/baskettrading: ../lib/TFSimulation/dist/Debug/GNU-Linux/libtfsimulation.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/baskettrading: ../lib/TFVuTrading/dist/Debug/GNU-Linux/libtfvutrading.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/baskettrading: ../lib/TFOptions/dist/Debug/GNU-Linux/libtfoptions.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/baskettrading: ../lib/TFTrading/dist/Debug/GNU-Linux/libtftrading.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/baskettrading: ../lib/TFTimeSeries/dist/Debug/GNU-Linux/libtftimeseries.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/baskettrading: ../lib/OUCharting/dist/Debug/GNU-Linux/liboucharting.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/baskettrading: ../lib/TFIndicators/dist/Debug/GNU-Linux/libtfindicators.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/baskettrading: ../lib/TFHDF5TimeSeries/dist/Debug/GNU-Linux/libtfhdf5timeseries.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/baskettrading: ../lib/TFTimeSeries/dist/Debug/GNU-Linux/libtftimeseries.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/baskettrading: ../lib/OUStatistics/dist/Debug/GNU-Linux/liboustatistics.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/baskettrading: ../lib/OUSqlite/dist/Debug/GNU-Linux/libousqlite.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/baskettrading: ../lib/OUSQL/dist/Debug/GNU-Linux/libousql.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/baskettrading: ../lib/OUCommon/dist/Debug/GNU-Linux/liboucommon.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/baskettrading: ../lib/ExcelFormat/dist/Debug/GNU-Linux/libexcelformat.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/baskettrading: ../lib/OUFormulas/dist/Debug/GNU-Linux/libouformulas.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/baskettrading: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/baskettrading ${OBJECTFILES} ${LDLIBSOPTIONS} `/usr/local/bin/wx-config --libs`

${OBJECTDIR}/BasketTrading.o: BasketTrading.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../lib -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/BasketTrading.o BasketTrading.cpp

${OBJECTDIR}/ManagePortfolio.o: ManagePortfolio.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../lib -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ManagePortfolio.o ManagePortfolio.cpp

${OBJECTDIR}/PanelBasketTradingMain.o: PanelBasketTradingMain.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../lib -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/PanelBasketTradingMain.o PanelBasketTradingMain.cpp

${OBJECTDIR}/PanelPortfolioStats.o: PanelPortfolioStats.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../lib -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/PanelPortfolioStats.o PanelPortfolioStats.cpp

${OBJECTDIR}/Position.o: Position.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../lib -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Position.o Position.cpp

${OBJECTDIR}/SymbolSelection.o: SymbolSelection.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../lib -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/SymbolSelection.o SymbolSelection.cpp

${OBJECTDIR}/Worker.o: Worker.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../lib -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Worker.o Worker.cpp

${OBJECTDIR}/stdafx.o: stdafx.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../lib -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/stdafx.o stdafx.cpp

# Subprojects
.build-subprojects:
	cd ../lib/TFBitsNPieces && ${MAKE} -j3 -f Makefile CONF=Debug
	cd ../lib/TFIQFeed && ${MAKE} -j3 -f Makefile CONF=Debug
	cd ../lib/TFInteractiveBrokers && ${MAKE} -j3 -f Makefile CONF=Debug
	cd ../lib/TFSimulation && ${MAKE} -j3 -f Makefile CONF=Debug
	cd ../lib/TFVuTrading && ${MAKE} -j3 -f Makefile CONF=Debug
	cd ../lib/TFOptions && ${MAKE} -j3 -f Makefile CONF=Debug
	cd ../lib/TFTrading && ${MAKE} -j3 -f Makefile CONF=Debug
	cd ../lib/TFTimeSeries && ${MAKE} -j3 -f Makefile CONF=Debug
	cd ../lib/OUCharting && ${MAKE} -j3 -f Makefile CONF=Debug
	cd ../lib/TFIndicators && ${MAKE} -j3 -f Makefile CONF=Debug
	cd ../lib/TFHDF5TimeSeries && ${MAKE} -j3 -f Makefile CONF=Debug
	cd ../lib/TFTimeSeries && ${MAKE} -j3 -f Makefile CONF=Debug
	cd ../lib/OUStatistics && ${MAKE} -j3 -f Makefile CONF=Debug
	cd ../lib/OUSqlite && ${MAKE} -j3 -f Makefile CONF=Debug
	cd ../lib/OUSQL && ${MAKE} -j3 -f Makefile CONF=Debug
	cd ../lib/OUCommon && ${MAKE} -j3 -f Makefile CONF=Debug
	cd ../lib/ExcelFormat && ${MAKE} -j3 -f ExcelFormat-Makefile.mk CONF=Debug
	cd ../lib/OUFormulas && ${MAKE} -j3 -f Makefile CONF=Debug

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:
	cd ../lib/TFBitsNPieces && ${MAKE} -j3 -f Makefile CONF=Debug clean
	cd ../lib/TFIQFeed && ${MAKE} -j3 -f Makefile CONF=Debug clean
	cd ../lib/TFInteractiveBrokers && ${MAKE} -j3 -f Makefile CONF=Debug clean
	cd ../lib/TFSimulation && ${MAKE} -j3 -f Makefile CONF=Debug clean
	cd ../lib/TFVuTrading && ${MAKE} -j3 -f Makefile CONF=Debug clean
	cd ../lib/TFOptions && ${MAKE} -j3 -f Makefile CONF=Debug clean
	cd ../lib/TFTrading && ${MAKE} -j3 -f Makefile CONF=Debug clean
	cd ../lib/TFTimeSeries && ${MAKE} -j3 -f Makefile CONF=Debug clean
	cd ../lib/OUCharting && ${MAKE} -j3 -f Makefile CONF=Debug clean
	cd ../lib/TFIndicators && ${MAKE} -j3 -f Makefile CONF=Debug clean
	cd ../lib/TFHDF5TimeSeries && ${MAKE} -j3 -f Makefile CONF=Debug clean
	cd ../lib/TFTimeSeries && ${MAKE} -j3 -f Makefile CONF=Debug clean
	cd ../lib/OUStatistics && ${MAKE} -j3 -f Makefile CONF=Debug clean
	cd ../lib/OUSqlite && ${MAKE} -j3 -f Makefile CONF=Debug clean
	cd ../lib/OUSQL && ${MAKE} -j3 -f Makefile CONF=Debug clean
	cd ../lib/OUCommon && ${MAKE} -j3 -f Makefile CONF=Debug clean
	cd ../lib/ExcelFormat && ${MAKE} -j3 -f ExcelFormat-Makefile.mk CONF=Debug clean
	cd ../lib/OUFormulas && ${MAKE} -j3 -f Makefile CONF=Debug clean

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
