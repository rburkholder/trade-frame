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
	${OBJECTDIR}/ControllerPortfolioPositionOrderExecution.o \
	${OBJECTDIR}/DialogBase.o \
	${OBJECTDIR}/DialogInstrumentSelect.o \
	${OBJECTDIR}/DialogNewPortfolio.o \
	${OBJECTDIR}/DialogPickSymbol.o \
	${OBJECTDIR}/DialogSimpleOneLineOrder.o \
	${OBJECTDIR}/EventDrawChart.o \
	${OBJECTDIR}/FrameMain.o \
	${OBJECTDIR}/ModelBase.o \
	${OBJECTDIR}/ModelCell.o \
	${OBJECTDIR}/ModelChartHdf5.o \
	${OBJECTDIR}/ModelExecution.o \
	${OBJECTDIR}/ModelOrder.o \
	${OBJECTDIR}/ModelPortfolio.o \
	${OBJECTDIR}/ModelPortfolioPositionOrderExecution.o \
	${OBJECTDIR}/ModelPosition.o \
	${OBJECTDIR}/PanelChartHdf5.o \
	${OBJECTDIR}/PanelFinancialChart.o \
	${OBJECTDIR}/PanelIBAccountValues.o \
	${OBJECTDIR}/PanelIBAccountValues_impl.o \
	${OBJECTDIR}/PanelIBPositionDetails.o \
	${OBJECTDIR}/PanelIBPositionDetails_impl.o \
	${OBJECTDIR}/PanelLogging.o \
	${OBJECTDIR}/PanelManualOrder.o \
	${OBJECTDIR}/PanelOptionDetails.o \
	${OBJECTDIR}/PanelOptionDetails_impl.o \
	${OBJECTDIR}/PanelPortfolioPosition.o \
	${OBJECTDIR}/PanelPortfolioPositionOrderExecution.o \
	${OBJECTDIR}/PanelPortfolioPosition_impl.o \
	${OBJECTDIR}/PanelProviderControl.o \
	${OBJECTDIR}/ValidatorInstrumentName.o \
	${OBJECTDIR}/VuBase.o \
	${OBJECTDIR}/VuExecutions.o \
	${OBJECTDIR}/VuOrders.o \
	${OBJECTDIR}/VuPortfolios.o \
	${OBJECTDIR}/VuPositions.o \
	${OBJECTDIR}/VuTreePortfolioPositionOrder.o \
	${OBJECTDIR}/WinChartView.o \
	${OBJECTDIR}/stdafx.o \
	${OBJECTDIR}/wxETKBaseValidator.o


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
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libtfvutrading.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libtfvutrading.a: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libtfvutrading.a
	${AR} -rv ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libtfvutrading.a ${OBJECTFILES} 
	$(RANLIB) ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libtfvutrading.a

${OBJECTDIR}/ControllerPortfolioPositionOrderExecution.o: ControllerPortfolioPositionOrderExecution.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ControllerPortfolioPositionOrderExecution.o ControllerPortfolioPositionOrderExecution.cpp

${OBJECTDIR}/DialogBase.o: DialogBase.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/DialogBase.o DialogBase.cpp

${OBJECTDIR}/DialogInstrumentSelect.o: DialogInstrumentSelect.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/DialogInstrumentSelect.o DialogInstrumentSelect.cpp

${OBJECTDIR}/DialogNewPortfolio.o: DialogNewPortfolio.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/DialogNewPortfolio.o DialogNewPortfolio.cpp

${OBJECTDIR}/DialogPickSymbol.o: DialogPickSymbol.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/DialogPickSymbol.o DialogPickSymbol.cpp

${OBJECTDIR}/DialogSimpleOneLineOrder.o: DialogSimpleOneLineOrder.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/DialogSimpleOneLineOrder.o DialogSimpleOneLineOrder.cpp

${OBJECTDIR}/EventDrawChart.o: EventDrawChart.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/EventDrawChart.o EventDrawChart.cpp

${OBJECTDIR}/FrameMain.o: FrameMain.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/FrameMain.o FrameMain.cpp

${OBJECTDIR}/ModelBase.o: ModelBase.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ModelBase.o ModelBase.cpp

${OBJECTDIR}/ModelCell.o: ModelCell.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ModelCell.o ModelCell.cpp

${OBJECTDIR}/ModelChartHdf5.o: ModelChartHdf5.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ModelChartHdf5.o ModelChartHdf5.cpp

${OBJECTDIR}/ModelExecution.o: ModelExecution.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ModelExecution.o ModelExecution.cpp

${OBJECTDIR}/ModelOrder.o: ModelOrder.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ModelOrder.o ModelOrder.cpp

${OBJECTDIR}/ModelPortfolio.o: ModelPortfolio.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ModelPortfolio.o ModelPortfolio.cpp

${OBJECTDIR}/ModelPortfolioPositionOrderExecution.o: ModelPortfolioPositionOrderExecution.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ModelPortfolioPositionOrderExecution.o ModelPortfolioPositionOrderExecution.cpp

${OBJECTDIR}/ModelPosition.o: ModelPosition.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ModelPosition.o ModelPosition.cpp

${OBJECTDIR}/PanelChartHdf5.o: PanelChartHdf5.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/PanelChartHdf5.o PanelChartHdf5.cpp

${OBJECTDIR}/PanelFinancialChart.o: PanelFinancialChart.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/PanelFinancialChart.o PanelFinancialChart.cpp

${OBJECTDIR}/PanelIBAccountValues.o: PanelIBAccountValues.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/PanelIBAccountValues.o PanelIBAccountValues.cpp

${OBJECTDIR}/PanelIBAccountValues_impl.o: PanelIBAccountValues_impl.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/PanelIBAccountValues_impl.o PanelIBAccountValues_impl.cpp

${OBJECTDIR}/PanelIBPositionDetails.o: PanelIBPositionDetails.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/PanelIBPositionDetails.o PanelIBPositionDetails.cpp

${OBJECTDIR}/PanelIBPositionDetails_impl.o: PanelIBPositionDetails_impl.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/PanelIBPositionDetails_impl.o PanelIBPositionDetails_impl.cpp

${OBJECTDIR}/PanelLogging.o: PanelLogging.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/PanelLogging.o PanelLogging.cpp

${OBJECTDIR}/PanelManualOrder.o: PanelManualOrder.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/PanelManualOrder.o PanelManualOrder.cpp

${OBJECTDIR}/PanelOptionDetails.o: PanelOptionDetails.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/PanelOptionDetails.o PanelOptionDetails.cpp

${OBJECTDIR}/PanelOptionDetails_impl.o: PanelOptionDetails_impl.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/PanelOptionDetails_impl.o PanelOptionDetails_impl.cpp

${OBJECTDIR}/PanelPortfolioPosition.o: PanelPortfolioPosition.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/PanelPortfolioPosition.o PanelPortfolioPosition.cpp

${OBJECTDIR}/PanelPortfolioPositionOrderExecution.o: PanelPortfolioPositionOrderExecution.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/PanelPortfolioPositionOrderExecution.o PanelPortfolioPositionOrderExecution.cpp

${OBJECTDIR}/PanelPortfolioPosition_impl.o: PanelPortfolioPosition_impl.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/PanelPortfolioPosition_impl.o PanelPortfolioPosition_impl.cpp

${OBJECTDIR}/PanelProviderControl.o: PanelProviderControl.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/PanelProviderControl.o PanelProviderControl.cpp

${OBJECTDIR}/ValidatorInstrumentName.o: ValidatorInstrumentName.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ValidatorInstrumentName.o ValidatorInstrumentName.cpp

${OBJECTDIR}/VuBase.o: VuBase.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/VuBase.o VuBase.cpp

${OBJECTDIR}/VuExecutions.o: VuExecutions.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/VuExecutions.o VuExecutions.cpp

${OBJECTDIR}/VuOrders.o: VuOrders.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/VuOrders.o VuOrders.cpp

${OBJECTDIR}/VuPortfolios.o: VuPortfolios.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/VuPortfolios.o VuPortfolios.cpp

${OBJECTDIR}/VuPositions.o: VuPositions.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/VuPositions.o VuPositions.cpp

${OBJECTDIR}/VuTreePortfolioPositionOrder.o: VuTreePortfolioPositionOrder.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/VuTreePortfolioPositionOrder.o VuTreePortfolioPositionOrder.cpp

${OBJECTDIR}/WinChartView.o: WinChartView.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/WinChartView.o WinChartView.cpp

${OBJECTDIR}/stdafx.o: stdafx.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/stdafx.o stdafx.cpp

${OBJECTDIR}/wxETKBaseValidator.o: wxETKBaseValidator.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/wxETKBaseValidator.o wxETKBaseValidator.cpp

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
