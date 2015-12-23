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
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/combotrading

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/combotrading: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/combotrading ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/BundleTracking.o: BundleTracking.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/BundleTracking.o BundleTracking.cpp

${OBJECTDIR}/ComboTrading.o: ComboTrading.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ComboTrading.o ComboTrading.cpp

${OBJECTDIR}/PanelCharts.o: PanelCharts.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/PanelCharts.o PanelCharts.cpp

${OBJECTDIR}/Process.o: Process.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Process.o Process.cpp

${OBJECTDIR}/TreeItem.o: TreeItem.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TreeItem.o TreeItem.cpp

${OBJECTDIR}/TreeItemGroup.o: TreeItemGroup.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TreeItemGroup.o TreeItemGroup.cpp

${OBJECTDIR}/TreeItemInstrument.o: TreeItemInstrument.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TreeItemInstrument.o TreeItemInstrument.cpp

${OBJECTDIR}/TreeItemPortfolio.o: TreeItemPortfolio.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TreeItemPortfolio.o TreeItemPortfolio.cpp

${OBJECTDIR}/TreeItemPosition.o: TreeItemPosition.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TreeItemPosition.o TreeItemPosition.cpp

${OBJECTDIR}/TreeOps.o: TreeOps.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TreeOps.o TreeOps.cpp

${OBJECTDIR}/UnderlyingSelection.o: UnderlyingSelection.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/UnderlyingSelection.o UnderlyingSelection.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/combotrading

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
