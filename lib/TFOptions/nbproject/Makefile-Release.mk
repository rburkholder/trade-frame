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
	${OBJECTDIR}/BearCallSpread.o \
	${OBJECTDIR}/Binomial.o \
	${OBJECTDIR}/BullPutSpread.o \
	${OBJECTDIR}/Bundle.o \
	${OBJECTDIR}/CalcExpiry.o \
	${OBJECTDIR}/CallBackSpread.o \
	${OBJECTDIR}/Chain.o \
	${OBJECTDIR}/Combo.o \
	${OBJECTDIR}/Condor.o \
	${OBJECTDIR}/Engine.o \
	${OBJECTDIR}/Formula.o \
	${OBJECTDIR}/IvAtm.o \
	${OBJECTDIR}/Leg.o \
	${OBJECTDIR}/Margin.o \
	${OBJECTDIR}/NoRiskInterestRateSeries.o \
	${OBJECTDIR}/Option.o \
	${OBJECTDIR}/PopulateWithIBOptions.o \
	${OBJECTDIR}/PutBackSpread.o \
	${OBJECTDIR}/Straddle.o \
	${OBJECTDIR}/Strangle.o \
	${OBJECTDIR}/Strike.o \
	${OBJECTDIR}/SyntheticLong.o \
	${OBJECTDIR}/SyntheticShort.o \
	${OBJECTDIR}/VerticalSpread.o


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
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libtfoptions.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libtfoptions.a: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libtfoptions.a
	${AR} -rv ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libtfoptions.a ${OBJECTFILES} 
	$(RANLIB) ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libtfoptions.a

${OBJECTDIR}/BearCallSpread.o: BearCallSpread.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/BearCallSpread.o BearCallSpread.cpp

${OBJECTDIR}/Binomial.o: Binomial.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Binomial.o Binomial.cpp

${OBJECTDIR}/BullPutSpread.o: BullPutSpread.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/BullPutSpread.o BullPutSpread.cpp

${OBJECTDIR}/Bundle.o: Bundle.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Bundle.o Bundle.cpp

${OBJECTDIR}/CalcExpiry.o: CalcExpiry.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/CalcExpiry.o CalcExpiry.cpp

${OBJECTDIR}/CallBackSpread.o: CallBackSpread.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/CallBackSpread.o CallBackSpread.cpp

${OBJECTDIR}/Chain.o: Chain.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Chain.o Chain.cpp

${OBJECTDIR}/Combo.o: Combo.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Combo.o Combo.cpp

${OBJECTDIR}/Condor.o: Condor.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Condor.o Condor.cpp

${OBJECTDIR}/Engine.o: Engine.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Engine.o Engine.cpp

${OBJECTDIR}/Formula.o: Formula.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Formula.o Formula.cpp

${OBJECTDIR}/IvAtm.o: IvAtm.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/IvAtm.o IvAtm.cpp

${OBJECTDIR}/Leg.o: Leg.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Leg.o Leg.cpp

${OBJECTDIR}/Margin.o: Margin.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Margin.o Margin.cpp

${OBJECTDIR}/NoRiskInterestRateSeries.o: NoRiskInterestRateSeries.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/NoRiskInterestRateSeries.o NoRiskInterestRateSeries.cpp

${OBJECTDIR}/Option.o: Option.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Option.o Option.cpp

${OBJECTDIR}/PopulateWithIBOptions.o: PopulateWithIBOptions.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/PopulateWithIBOptions.o PopulateWithIBOptions.cpp

${OBJECTDIR}/PutBackSpread.o: PutBackSpread.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/PutBackSpread.o PutBackSpread.cpp

${OBJECTDIR}/Straddle.o: Straddle.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Straddle.o Straddle.cpp

${OBJECTDIR}/Strangle.o: Strangle.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Strangle.o Strangle.cpp

${OBJECTDIR}/Strike.o: Strike.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Strike.o Strike.cpp

${OBJECTDIR}/SyntheticLong.o: SyntheticLong.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/SyntheticLong.o SyntheticLong.cpp

${OBJECTDIR}/SyntheticShort.o: SyntheticShort.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/SyntheticShort.o SyntheticShort.cpp

${OBJECTDIR}/VerticalSpread.o: VerticalSpread.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/VerticalSpread.o VerticalSpread.cpp

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
