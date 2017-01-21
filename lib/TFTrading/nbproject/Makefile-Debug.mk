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
	${OBJECTDIR}/Account.o \
	${OBJECTDIR}/AccountAdvisor.o \
	${OBJECTDIR}/AccountManager.o \
	${OBJECTDIR}/AccountOwner.o \
	${OBJECTDIR}/AlternateInstrumentNames.o \
	${OBJECTDIR}/CashAccount.o \
	${OBJECTDIR}/CashManager.o \
	${OBJECTDIR}/CashTransaction.o \
	${OBJECTDIR}/Currency.o \
	${OBJECTDIR}/DBOps.o \
	${OBJECTDIR}/DailyTradeTimeFrames.o \
	${OBJECTDIR}/Database.o \
	${OBJECTDIR}/Exchange.o \
	${OBJECTDIR}/Execution.o \
	${OBJECTDIR}/Instrument.o \
	${OBJECTDIR}/InstrumentData.o \
	${OBJECTDIR}/InstrumentManager.o \
	${OBJECTDIR}/KeyTypes.o \
	${OBJECTDIR}/Managers.o \
	${OBJECTDIR}/NoRiskInterestRateSeries.o \
	${OBJECTDIR}/Order.o \
	${OBJECTDIR}/OrderManager.o \
	${OBJECTDIR}/OrdersOutstanding.o \
	${OBJECTDIR}/Portfolio.o \
	${OBJECTDIR}/PortfolioManager.o \
	${OBJECTDIR}/Position.o \
	${OBJECTDIR}/PositionCombo.o \
	${OBJECTDIR}/ProviderManager.o \
	${OBJECTDIR}/RiskManager.o \
	${OBJECTDIR}/Symbol.o \
	${OBJECTDIR}/TradingEnumerations.o \
	${OBJECTDIR}/Watch.o \
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
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libtftrading.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libtftrading.a: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libtftrading.a
	${AR} -rv ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libtftrading.a ${OBJECTFILES} 
	$(RANLIB) ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libtftrading.a

${OBJECTDIR}/Account.o: Account.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Account.o Account.cpp

${OBJECTDIR}/AccountAdvisor.o: AccountAdvisor.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/AccountAdvisor.o AccountAdvisor.cpp

${OBJECTDIR}/AccountManager.o: AccountManager.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/AccountManager.o AccountManager.cpp

${OBJECTDIR}/AccountOwner.o: AccountOwner.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/AccountOwner.o AccountOwner.cpp

${OBJECTDIR}/AlternateInstrumentNames.o: AlternateInstrumentNames.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/AlternateInstrumentNames.o AlternateInstrumentNames.cpp

${OBJECTDIR}/CashAccount.o: CashAccount.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/CashAccount.o CashAccount.cpp

${OBJECTDIR}/CashManager.o: CashManager.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/CashManager.o CashManager.cpp

${OBJECTDIR}/CashTransaction.o: CashTransaction.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/CashTransaction.o CashTransaction.cpp

${OBJECTDIR}/Currency.o: Currency.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Currency.o Currency.cpp

${OBJECTDIR}/DBOps.o: DBOps.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/DBOps.o DBOps.cpp

${OBJECTDIR}/DailyTradeTimeFrames.o: DailyTradeTimeFrames.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/DailyTradeTimeFrames.o DailyTradeTimeFrames.cpp

${OBJECTDIR}/Database.o: Database.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Database.o Database.cpp

${OBJECTDIR}/Exchange.o: Exchange.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Exchange.o Exchange.cpp

${OBJECTDIR}/Execution.o: Execution.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Execution.o Execution.cpp

${OBJECTDIR}/Instrument.o: Instrument.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Instrument.o Instrument.cpp

${OBJECTDIR}/InstrumentData.o: InstrumentData.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/InstrumentData.o InstrumentData.cpp

${OBJECTDIR}/InstrumentManager.o: InstrumentManager.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/InstrumentManager.o InstrumentManager.cpp

${OBJECTDIR}/KeyTypes.o: KeyTypes.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/KeyTypes.o KeyTypes.cpp

${OBJECTDIR}/Managers.o: Managers.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Managers.o Managers.cpp

${OBJECTDIR}/NoRiskInterestRateSeries.o: NoRiskInterestRateSeries.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/NoRiskInterestRateSeries.o NoRiskInterestRateSeries.cpp

${OBJECTDIR}/Order.o: Order.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Order.o Order.cpp

${OBJECTDIR}/OrderManager.o: OrderManager.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/OrderManager.o OrderManager.cpp

${OBJECTDIR}/OrdersOutstanding.o: OrdersOutstanding.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/OrdersOutstanding.o OrdersOutstanding.cpp

${OBJECTDIR}/Portfolio.o: Portfolio.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Portfolio.o Portfolio.cpp

${OBJECTDIR}/PortfolioManager.o: PortfolioManager.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/PortfolioManager.o PortfolioManager.cpp

${OBJECTDIR}/Position.o: Position.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Position.o Position.cpp

${OBJECTDIR}/PositionCombo.o: PositionCombo.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/PositionCombo.o PositionCombo.cpp

${OBJECTDIR}/ProviderManager.o: ProviderManager.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ProviderManager.o ProviderManager.cpp

${OBJECTDIR}/RiskManager.o: RiskManager.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/RiskManager.o RiskManager.cpp

${OBJECTDIR}/Symbol.o: Symbol.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Symbol.o Symbol.cpp

${OBJECTDIR}/TradingEnumerations.o: TradingEnumerations.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TradingEnumerations.o TradingEnumerations.cpp

${OBJECTDIR}/Watch.o: Watch.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Watch.o Watch.cpp

${OBJECTDIR}/stdafx.o: stdafx.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I../ -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/stdafx.o stdafx.cpp

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
