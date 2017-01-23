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
	${OBJECTDIR}/CharBuffer.o \
	${OBJECTDIR}/ConsoleStream.o \
	${OBJECTDIR}/CountryCode.o \
	${OBJECTDIR}/CurrencyCode.o \
	${OBJECTDIR}/ReadCodeListCommon.o \
	${OBJECTDIR}/ReadNaicsToSicCodeList.o \
	${OBJECTDIR}/ReadSicCodeList.o \
	${OBJECTDIR}/ReadSicToNaicsCodeList.o \
	${OBJECTDIR}/Singleton.o \
	${OBJECTDIR}/SmartVar.o \
	${OBJECTDIR}/TimeSource.o \
	${OBJECTDIR}/WuManber.o


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
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/liboucommon.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/liboucommon.a: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/liboucommon.a
	${AR} -rv ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/liboucommon.a ${OBJECTFILES} 
	$(RANLIB) ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/liboucommon.a

${OBJECTDIR}/CharBuffer.o: CharBuffer.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/CharBuffer.o CharBuffer.cpp

${OBJECTDIR}/ConsoleStream.o: ConsoleStream.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ConsoleStream.o ConsoleStream.cpp

${OBJECTDIR}/CountryCode.o: CountryCode.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/CountryCode.o CountryCode.cpp

${OBJECTDIR}/CurrencyCode.o: CurrencyCode.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/CurrencyCode.o CurrencyCode.cpp

${OBJECTDIR}/ReadCodeListCommon.o: ReadCodeListCommon.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ReadCodeListCommon.o ReadCodeListCommon.cpp

${OBJECTDIR}/ReadNaicsToSicCodeList.o: ReadNaicsToSicCodeList.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ReadNaicsToSicCodeList.o ReadNaicsToSicCodeList.cpp

${OBJECTDIR}/ReadSicCodeList.o: ReadSicCodeList.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ReadSicCodeList.o ReadSicCodeList.cpp

${OBJECTDIR}/ReadSicToNaicsCodeList.o: ReadSicToNaicsCodeList.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ReadSicToNaicsCodeList.o ReadSicToNaicsCodeList.cpp

${OBJECTDIR}/Singleton.o: Singleton.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Singleton.o Singleton.cpp

${OBJECTDIR}/SmartVar.o: SmartVar.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/SmartVar.o SmartVar.cpp

${OBJECTDIR}/TimeSource.o: TimeSource.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TimeSource.o TimeSource.cpp

${OBJECTDIR}/WuManber.o: WuManber.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/WuManber.o WuManber.cpp

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
