/************************************************************************
 * Copyright(c) 2011, One Unified. All rights reserved.                 *
 *                                                                      *
 * This file is provided as is WITHOUT ANY WARRANTY                     *
 *  without even the implied warranty of                                *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                *
 *                                                                      *
 * This software may not be used nor distributed without proper license *
 * agreement.                                                           *
 *                                                                      *
 * See the file LICENSE.txt for redistribution information.             *
 ************************************************************************/

#pragma once

#include <string>
#include <map>

#include <TFTimeSeries/TimeSeries.h>
#include <TFTimeSeries/BarFactory.h>

#include <TFIndicators/TSSWStats.h>
#include <TFIndicators/TSSWRunningTally.h>
#include <TFIndicators/TSSWRateOfChange.h>
#include <TFIndicators/ZigZag.h>

#include <TFSimulation/SimulationProvider.h>

#include <TFTrading/PortfolioManager.h>
#include <TFTrading/ProviderManager.h>
#include <TFTrading/Position.h>
#include <TFTrading/Instrument.h>

#include <OUCharting/ChartDataView.h>
#include <OUCharting/ChartEntryBars.h>
#include <OUCharting/ChartEntryVolume.h>
#include <OUCharting/ChartEntryIndicator.h>
#include <OUCharting/ChartEntryShape.h>

#include "OrdersOutstanding.h"

class Strategy {
public:

  typedef ou::tf::CProviderInterfaceBase::pProvider_t pProvider_t;

  Strategy( pProvider_t pDataProvider, pProvider_t pExecutionProvider );
  ~Strategy(void);

  void Start( void ); // for real time
  void Start( const std::string& sSymbolPath );  // for simulation

  ou::ChartDataView& GetChartDataView( void ) {return m_dvChart; };

protected:
private:

  bool m_bFirstTrade;

  enum stateTrade {
    ETradeStart, ETradeOutOfMarket, ETradeWaitLongEntry, ETradeWaitShortEntry, ETradeCancel, ETradeClose, ETradeDone,
    ETradeWaitForBollingerOutside, ETradeWaitForBollingerInside,
    ETradeWaitForUp, ETradeWaitForDn,
    ETradeWaitForBollingerToRise, ETradeWaitForBollingerToFall,
    ETradeWaitForTrendConfirmation,
    ETradeLongAndWaitForCrossingSMADownwards, ETradeShortAndWaitForCrossingSMAUpwards, 
  } m_stateTrade;

  std::stringstream m_ss;

  typedef ou::tf::CPosition::pOrder_t pOrder_t;
  typedef ou::tf::CPosition::pPosition_t pPosition_t;
  typedef ou::tf::CInstrument::pInstrument_t pInstrument_t;
  typedef ou::tf::CSimulationProvider::pProvider_t pProviderSim_t;

  typedef ou::tf::CInstrument::pInstrument_t m_pInstrument_t;
  pInstrument_t m_pShortInstrument;
  pInstrument_t m_pLongInstrument;
  pInstrument_t m_pTestInstrument;

  enum enumTradeDirection { ETradeDirUnkn, ETradeDirUp, ETradeDirDn } m_TradeDirection;
  ou::tf::CQuote m_quoteLast;  // used for classifying the current trade direction

  double m_dblLastMidpoint;

  time_duration m_tdTimeBetweenTrades;
  ptime m_dtLastSubmission;

  ou::tf::CQuotes m_quotes;
  ou::tf::CTrades m_trades;
  ou::tf::CBars m_bars;

  ou::tf::CBarFactory m_barFactory;

  typedef ou::ChartEntryBase::pChartEntryBase_t pChartEntryBase_t;

  ou::ChartDataView m_dvChart;
  ou::ChartEntryBars m_ceBars;
  ou::ChartEntryVolume m_ceVolume;

//  ou::ChartEntryIndicator m_ceSMA1;
//  ou::ChartEntryIndicator m_ceSlopeOfSMA1;
//  ou::ChartEntryIndicator m_ceSlopeOfSlopeOfSMA1;
  ou::ChartEntryIndicator m_ceUpperBollinger1;
  ou::ChartEntryIndicator m_ceLowerBollinger1;
  ou::ChartEntryIndicator m_ceBollinger1Offset;
//  ou::ChartEntryIndicator m_ceBollinger1Ratio;

  ou::ChartEntryIndicator m_ceSMA2;
  ou::ChartEntryIndicator m_ceSlopeOfSMA2;
  ou::ChartEntryIndicator m_ceSlopeOfSlopeOfSMA2;
  ou::ChartEntryIndicator m_ceUpperBollinger2;
  ou::ChartEntryIndicator m_ceLowerBollinger2;
  ou::ChartEntryIndicator m_ceBollinger2Offset;
  ou::ChartEntryIndicator m_ceSlopeOfBollinger2Offset;
  //ou::ChartEntryIndicator m_ceBollinger2Ratio;

  ou::ChartEntryIndicator m_ceSMA3;
  ou::ChartEntryIndicator m_ceUpperBollinger3;
  ou::ChartEntryIndicator m_ceLowerBollinger3;
  ou::ChartEntryIndicator m_ceBollinger3Offset;
  //ou::ChartEntryIndicator m_ceBollinger3Ratio;

  ou::ChartEntryIndicator m_cePLLong;
  ou::ChartEntryIndicator m_cePLShort;
  ou::ChartEntryIndicator m_cePLNet;
  ou::ChartEntryIndicator m_ceOutstandingEntriesLong;
  ou::ChartEntryIndicator m_ceOutstandingEntriesShort;
  ou::ChartEntryIndicator m_ceOutstandingExitsLong;
  ou::ChartEntryIndicator m_ceOutstandingExitsShort;
  ou::ChartEntryIndicator m_ceSpread;

  //ou::ChartEntryIndicator m_ceUpVolume;
  //ou::ChartEntryIndicator m_ceMdVolume;
  //ou::ChartEntryIndicator m_ceDnVolume;

  ou::ChartEntryIndicator m_ceTickDiffs;
  ou::ChartEntryIndicator m_ceTickDiffsRoc;

  ou::ChartEntryIndicator m_ceZigZag;

  ou::ChartEntryShape m_ceShorts;
  ou::ChartEntryShape m_ceLongs;

  pPosition_t m_pPositionLong;
  pPosition_t m_pPositionShort;

  pOrder_t m_pOrder;  // active order

  OrdersOutstandingLongs* m_pOrdersOutstandingLongs;
  OrdersOutstandingShorts* m_pOrdersOutstandingShorts;

  ptime m_dtEnd;
  unsigned int m_nUpTransitions;
  unsigned int m_nDnTransitions;

  double m_dblUpTicks, m_dblMdTicks, m_dblDnTicks;
  double m_dblUpVolume, m_dblMdVolume, m_dblDnVolume;

  ou::tf::CPrices m_pricesTickDiffs;
  ou::tf::TSSWRunningTally m_rtTickDiffs;

  ou::tf::CPrices m_pricesTickDiffsROC;
  ou::tf::TSSWRateOfChange m_rocTickDiffs;

  ou::tf::TSSWStatsMidQuote m_sma1;
  ou::tf::TSSWStatsMidQuote m_sma2;
  ou::tf::TSSWStatsMidQuote m_sma3;
  ou::tf::TSSWStatsMidQuote m_sma4;
  ou::tf::TSSWStatsMidQuote m_sma5;
  ou::tf::TSSWStatsMidQuote m_sma6;
  ou::tf::TSSWStatsMidQuote m_sma7;
  ou::tf::TSSWStatsMidQuote m_sma8;

//  ou::tf::CPrices m_pricesSlopeOfSlopeOfSMA1;
//  ou::tf::TSSWStatsPrice m_tsswSlopeOfSlopeOfSMA1;

  ou::tf::CPrices m_pricesSlopeOfSlopeOfSMA2;
  ou::tf::TSSWStatsPrice m_tsswSlopeOfSlopeOfSMA2;

  ou::tf::CPrices m_pricesBollinger2Offset;
  ou::tf::TSSWStatsPrice m_tsswSlopeOfBollinger2Offset;

  ou::tf::CPrices m_spreads;
  ou::tf::TSSWStatsPrice m_tsswSpreads;

  ou::tf::ZigZag m_zigzagPrice;

  typedef ou::tf::CSimulationProvider::pSymbol_t pSimSymbol_t;

  pProviderSim_t m_sim;
  bool m_bSimConnected;

  pProvider_t m_pDataProvider;
  pProvider_t m_pExecutionProvider;

  void HandleSimulatorConnected( int );
  void HandleSimulatorDisConnected( int );

  void HandleOnExecConnected( int ); 
  void HandleOnExecDisconnected( int );

  void HandleOnData1Connected( int );
  void HandleOnData1Disconnected( int );

  void HandleFirstQuote( const ou::tf::CQuote& quote );
  void HandleFirstTrade( const ou::tf::CTrade& trade );
  void HandleQuote( const ou::tf::CQuote& quote );
  void HandleTrade( const ou::tf::CTrade& trade );
  void HandleSimulationComplete( void );

  void HandleOrderFilled( const ou::tf::COrder& );

  void HandleExecution( ou::tf::CPosition::execution_delegate_t del );
  void HandleCommission( const ou::tf::CPosition* pPosition );

  void HandleBarCompletion( const ou::tf::CBar& );

  void HandleZigZagPeak( ou::tf::ZigZag*, ptime, double, ou::tf::ZigZag::EDirection );
  void HandleZigZagUpDp( ou::tf::ZigZag* );
  void HandleZigZagDnDp( ou::tf::ZigZag* );
};

