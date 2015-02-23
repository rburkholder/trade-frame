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
	${OBJECTDIR}/FrameWork01.o \
	${OBJECTDIR}/IQFeedSymbolListOps.o \
	${OBJECTDIR}/InstrumentSelection.o \
	${OBJECTDIR}/LiborFromIQFeed.o \
	${OBJECTDIR}/ReadCboeWeeklyOptions.o \
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
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libtfbitsnpieces.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libtfbitsnpieces.a: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libtfbitsnpieces.a
	${AR} -rv ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libtfbitsnpieces.a ${OBJECTFILES} 
	$(RANLIB) ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libtfbitsnpieces.a

${OBJECTDIR}/FrameWork01.o: FrameWork01.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -D__WXGTK__ -DwxUSE_GUI -I../ -I/usr/lib/x86_64-linux-gnu/wx/include/gtk2-unicode-3.0 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/FrameWork01.o FrameWork01.cpp

${OBJECTDIR}/IQFeedSymbolListOps.o: IQFeedSymbolListOps.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -D__WXGTK__ -DwxUSE_GUI -I../ -I/usr/lib/x86_64-linux-gnu/wx/include/gtk2-unicode-3.0 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/IQFeedSymbolListOps.o IQFeedSymbolListOps.cpp

${OBJECTDIR}/InstrumentSelection.o: InstrumentSelection.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -D__WXGTK__ -DwxUSE_GUI -I../ -I/usr/lib/x86_64-linux-gnu/wx/include/gtk2-unicode-3.0 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/InstrumentSelection.o InstrumentSelection.cpp

${OBJECTDIR}/LiborFromIQFeed.o: LiborFromIQFeed.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -D__WXGTK__ -DwxUSE_GUI -I../ -I/usr/lib/x86_64-linux-gnu/wx/include/gtk2-unicode-3.0 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/LiborFromIQFeed.o LiborFromIQFeed.cpp

${OBJECTDIR}/ReadCboeWeeklyOptions.o: ReadCboeWeeklyOptions.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -D__WXGTK__ -DwxUSE_GUI -I../ -I/usr/lib/x86_64-linux-gnu/wx/include/gtk2-unicode-3.0 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ReadCboeWeeklyOptions.o ReadCboeWeeklyOptions.cpp

${OBJECTDIR}/stdafx.o: stdafx.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -D__WXGTK__ -DwxUSE_GUI -I../ -I/usr/lib/x86_64-linux-gnu/wx/include/gtk2-unicode-3.0 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/stdafx.o stdafx.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libtfbitsnpieces.a

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
