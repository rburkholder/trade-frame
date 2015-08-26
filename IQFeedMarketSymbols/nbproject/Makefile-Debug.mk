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
	${OBJECTDIR}/IQFeedMarketSymbols.o \
	${OBJECTDIR}/stdafx.o


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
LDLIBSOPTIONS=-L/usr/local/lib -L/usr/lib/x86_64-linux-gnu -L/lib/x86_64-linux-gnu ../lib/TFBitsNPieces/dist/Debug/GNU-Linux-x86/libtfbitsnpieces.a ../lib/TFVuTrading/dist/Debug/GNU-Linux-x86/libtfvutrading.a ../lib/TFSimulation/dist/Debug/GNU-Linux-x86/libtfsimulation.a ../lib/TFIQFeed/dist/Debug/GNU-Linux-x86/libtfiqfeed.a ../lib/TFInteractiveBrokers/dist/Debug/GNU-Linux-x86/libtfinteractivebrokers.a ../lib/TFTrading/dist/Debug/GNU-Linux-x86/libtftrading.a ../lib/TFOptions/dist/Debug/GNU-Linux-x86/libtfoptions.a ../lib/TFHDF5TimeSeries/dist/Debug/GNU-Linux-x86/libtfhdf5timeseries.a ../lib/TFTimeSeries/dist/Debug/GNU-Linux-x86/libtftimeseries.a ../lib/OUCommon/dist/Debug/GNU-Linux-x86/liboucommon.a ../lib/OUSQL/dist/Debug/GNU-Linux-x86/libousql.a ../lib/OUSqlite/dist/Debug/GNU-Linux-x86/libousqlite.a ../lib/ExcelFormat/dist/Debug/GNU-Linux-x86/libexcelformat.a -lboost_system -lboost_date_time -lboost_filesystem -lboost_serialization -lboost_thread -lboost_regex -lhdf5_cpp -lhdf5 -lz -lsz -lwx_baseu-3.0 -lwx_gtk2u_adv-3.0 -lwx_gtk2u_core-3.0 -lcurl -lpthread -ldl

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/iqfeedmarketsymbols

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/iqfeedmarketsymbols: ../lib/TFBitsNPieces/dist/Debug/GNU-Linux-x86/libtfbitsnpieces.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/iqfeedmarketsymbols: ../lib/TFVuTrading/dist/Debug/GNU-Linux-x86/libtfvutrading.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/iqfeedmarketsymbols: ../lib/TFSimulation/dist/Debug/GNU-Linux-x86/libtfsimulation.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/iqfeedmarketsymbols: ../lib/TFIQFeed/dist/Debug/GNU-Linux-x86/libtfiqfeed.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/iqfeedmarketsymbols: ../lib/TFInteractiveBrokers/dist/Debug/GNU-Linux-x86/libtfinteractivebrokers.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/iqfeedmarketsymbols: ../lib/TFTrading/dist/Debug/GNU-Linux-x86/libtftrading.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/iqfeedmarketsymbols: ../lib/TFOptions/dist/Debug/GNU-Linux-x86/libtfoptions.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/iqfeedmarketsymbols: ../lib/TFHDF5TimeSeries/dist/Debug/GNU-Linux-x86/libtfhdf5timeseries.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/iqfeedmarketsymbols: ../lib/TFTimeSeries/dist/Debug/GNU-Linux-x86/libtftimeseries.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/iqfeedmarketsymbols: ../lib/OUCommon/dist/Debug/GNU-Linux-x86/liboucommon.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/iqfeedmarketsymbols: ../lib/OUSQL/dist/Debug/GNU-Linux-x86/libousql.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/iqfeedmarketsymbols: ../lib/OUSqlite/dist/Debug/GNU-Linux-x86/libousqlite.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/iqfeedmarketsymbols: ../lib/ExcelFormat/dist/Debug/GNU-Linux-x86/libexcelformat.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/iqfeedmarketsymbols: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/iqfeedmarketsymbols ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/IQFeedMarketSymbols.o: IQFeedMarketSymbols.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -D__WXGTK__ -DwxUSE_GUI -I../lib -I/usr/lib/x86_64-linux-gnu/wx/include/gtk2-unicode-3.0 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/IQFeedMarketSymbols.o IQFeedMarketSymbols.cpp

${OBJECTDIR}/stdafx.o: stdafx.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -D__WXGTK__ -DwxUSE_GUI -I../lib -I/usr/lib/x86_64-linux-gnu/wx/include/gtk2-unicode-3.0 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/stdafx.o stdafx.cpp

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
	cd ../lib/ExcelFormat && ${MAKE}  -f ExcelFormat-Makefile.mk CONF=Debug

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/iqfeedmarketsymbols

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
	cd ../lib/ExcelFormat && ${MAKE}  -f ExcelFormat-Makefile.mk CONF=Debug clean

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
