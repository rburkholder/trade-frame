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
	${OBJECTDIR}/Binomial.o \
	${OBJECTDIR}/Bundle.o \
	${OBJECTDIR}/CalcExpiry.o \
	${OBJECTDIR}/Formula.o \
	${OBJECTDIR}/Margin.o \
	${OBJECTDIR}/Option.o \
	${OBJECTDIR}/PopulateWithIBOptions.o \
	${OBJECTDIR}/Strike.o


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

${OBJECTDIR}/Binomial.o: Binomial.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Binomial.o Binomial.cpp

${OBJECTDIR}/Bundle.o: Bundle.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Bundle.o Bundle.cpp

${OBJECTDIR}/CalcExpiry.o: CalcExpiry.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/CalcExpiry.o CalcExpiry.cpp

${OBJECTDIR}/Formula.o: Formula.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Formula.o Formula.cpp

${OBJECTDIR}/Margin.o: Margin.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Margin.o Margin.cpp

${OBJECTDIR}/Option.o: Option.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Option.o Option.cpp

${OBJECTDIR}/PopulateWithIBOptions.o: PopulateWithIBOptions.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/PopulateWithIBOptions.o PopulateWithIBOptions.cpp

${OBJECTDIR}/Strike.o: Strike.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Strike.o Strike.cpp

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
