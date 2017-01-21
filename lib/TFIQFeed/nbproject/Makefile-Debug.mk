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
	${OBJECTDIR}/BuildInstrument.o \
	${OBJECTDIR}/BuildSymbolName.o \
	${OBJECTDIR}/CurlGetMktSymbols.o \
	${OBJECTDIR}/IQFeed.o \
	${OBJECTDIR}/IQFeedMessages.o \
	${OBJECTDIR}/IQFeedProvider.o \
	${OBJECTDIR}/IQFeedSymbol.o \
	${OBJECTDIR}/InMemoryMktSymbolList.o \
	${OBJECTDIR}/LoadMktSymbols.o \
	${OBJECTDIR}/MarketSymbol.o \
	${OBJECTDIR}/MarketSymbols.o \
	${OBJECTDIR}/Option.o \
	${OBJECTDIR}/ParseMktSymbolDiskFile.o \
	${OBJECTDIR}/ParseMktSymbolLine.o \
	${OBJECTDIR}/UnzipMktSymbols.o \
	${OBJECTDIR}/ValidateMktSymbolLine.o


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
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libtfiqfeed.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libtfiqfeed.a: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libtfiqfeed.a
	${AR} -rv ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libtfiqfeed.a ${OBJECTFILES} 
	$(RANLIB) ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libtfiqfeed.a

${OBJECTDIR}/BuildInstrument.o: BuildInstrument.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/BuildInstrument.o BuildInstrument.cpp

${OBJECTDIR}/BuildSymbolName.o: BuildSymbolName.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/BuildSymbolName.o BuildSymbolName.cpp

${OBJECTDIR}/CurlGetMktSymbols.o: CurlGetMktSymbols.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/CurlGetMktSymbols.o CurlGetMktSymbols.cpp

${OBJECTDIR}/IQFeed.o: IQFeed.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/IQFeed.o IQFeed.cpp

${OBJECTDIR}/IQFeedMessages.o: IQFeedMessages.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/IQFeedMessages.o IQFeedMessages.cpp

${OBJECTDIR}/IQFeedProvider.o: IQFeedProvider.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/IQFeedProvider.o IQFeedProvider.cpp

${OBJECTDIR}/IQFeedSymbol.o: IQFeedSymbol.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/IQFeedSymbol.o IQFeedSymbol.cpp

${OBJECTDIR}/InMemoryMktSymbolList.o: InMemoryMktSymbolList.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/InMemoryMktSymbolList.o InMemoryMktSymbolList.cpp

${OBJECTDIR}/LoadMktSymbols.o: LoadMktSymbols.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/LoadMktSymbols.o LoadMktSymbols.cpp

${OBJECTDIR}/MarketSymbol.o: MarketSymbol.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/MarketSymbol.o MarketSymbol.cpp

${OBJECTDIR}/MarketSymbols.o: MarketSymbols.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/MarketSymbols.o MarketSymbols.cpp

${OBJECTDIR}/Option.o: Option.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Option.o Option.cpp

${OBJECTDIR}/ParseMktSymbolDiskFile.o: ParseMktSymbolDiskFile.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ParseMktSymbolDiskFile.o ParseMktSymbolDiskFile.cpp

${OBJECTDIR}/ParseMktSymbolLine.o: ParseMktSymbolLine.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ParseMktSymbolLine.o ParseMktSymbolLine.cpp

${OBJECTDIR}/UnzipMktSymbols.o: UnzipMktSymbols.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/UnzipMktSymbols.o UnzipMktSymbols.cpp

${OBJECTDIR}/ValidateMktSymbolLine.o: ValidateMktSymbolLine.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ValidateMktSymbolLine.o ValidateMktSymbolLine.cpp

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
