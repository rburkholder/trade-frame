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
	${OBJECTDIR}/Crossing.o \
	${OBJECTDIR}/PivotGroup.o \
	${OBJECTDIR}/Pivots.o \
	${OBJECTDIR}/RunningMinMax.o \
	${OBJECTDIR}/RunningStats.o \
	${OBJECTDIR}/SlidingWindow.o \
	${OBJECTDIR}/StatsInSlidingWindow.o \
	${OBJECTDIR}/TSDifferential.o \
	${OBJECTDIR}/TSEMA.o \
	${OBJECTDIR}/TSHomogenization.o \
	${OBJECTDIR}/TSMA.o \
	${OBJECTDIR}/TSNorm.o \
	${OBJECTDIR}/TSReturns.o \
	${OBJECTDIR}/TSSWEfficiencyRatio.o \
	${OBJECTDIR}/TSSWRateOfChange.o \
	${OBJECTDIR}/TSSWRealizedVolatility.o \
	${OBJECTDIR}/TSSWRunningTally.o \
	${OBJECTDIR}/TSSWStats.o \
	${OBJECTDIR}/TSSWStochastic.o \
	${OBJECTDIR}/TSSWTickFrequency.o \
	${OBJECTDIR}/TSVariance.o \
	${OBJECTDIR}/TSVolatility.o \
	${OBJECTDIR}/ZigZag.o \
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
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libtfindicators.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libtfindicators.a: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libtfindicators.a
	${AR} -rv ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libtfindicators.a ${OBJECTFILES} 
	$(RANLIB) ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libtfindicators.a

${OBJECTDIR}/Crossing.o: Crossing.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Crossing.o Crossing.cpp

${OBJECTDIR}/PivotGroup.o: PivotGroup.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/PivotGroup.o PivotGroup.cpp

${OBJECTDIR}/Pivots.o: Pivots.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Pivots.o Pivots.cpp

${OBJECTDIR}/RunningMinMax.o: RunningMinMax.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/RunningMinMax.o RunningMinMax.cpp

${OBJECTDIR}/RunningStats.o: RunningStats.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/RunningStats.o RunningStats.cpp

${OBJECTDIR}/SlidingWindow.o: SlidingWindow.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/SlidingWindow.o SlidingWindow.cpp

${OBJECTDIR}/StatsInSlidingWindow.o: StatsInSlidingWindow.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/StatsInSlidingWindow.o StatsInSlidingWindow.cpp

${OBJECTDIR}/TSDifferential.o: TSDifferential.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TSDifferential.o TSDifferential.cpp

${OBJECTDIR}/TSEMA.o: TSEMA.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TSEMA.o TSEMA.cpp

${OBJECTDIR}/TSHomogenization.o: TSHomogenization.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TSHomogenization.o TSHomogenization.cpp

${OBJECTDIR}/TSMA.o: TSMA.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TSMA.o TSMA.cpp

${OBJECTDIR}/TSNorm.o: TSNorm.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TSNorm.o TSNorm.cpp

${OBJECTDIR}/TSReturns.o: TSReturns.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TSReturns.o TSReturns.cpp

${OBJECTDIR}/TSSWEfficiencyRatio.o: TSSWEfficiencyRatio.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TSSWEfficiencyRatio.o TSSWEfficiencyRatio.cpp

${OBJECTDIR}/TSSWRateOfChange.o: TSSWRateOfChange.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TSSWRateOfChange.o TSSWRateOfChange.cpp

${OBJECTDIR}/TSSWRealizedVolatility.o: TSSWRealizedVolatility.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TSSWRealizedVolatility.o TSSWRealizedVolatility.cpp

${OBJECTDIR}/TSSWRunningTally.o: TSSWRunningTally.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TSSWRunningTally.o TSSWRunningTally.cpp

${OBJECTDIR}/TSSWStats.o: TSSWStats.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TSSWStats.o TSSWStats.cpp

${OBJECTDIR}/TSSWStochastic.o: TSSWStochastic.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TSSWStochastic.o TSSWStochastic.cpp

${OBJECTDIR}/TSSWTickFrequency.o: TSSWTickFrequency.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TSSWTickFrequency.o TSSWTickFrequency.cpp

${OBJECTDIR}/TSVariance.o: TSVariance.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TSVariance.o TSVariance.cpp

${OBJECTDIR}/TSVolatility.o: TSVolatility.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TSVolatility.o TSVolatility.cpp

${OBJECTDIR}/ZigZag.o: ZigZag.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ZigZag.o ZigZag.cpp

${OBJECTDIR}/stdafx.o: stdafx.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/stdafx.o stdafx.cpp

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
