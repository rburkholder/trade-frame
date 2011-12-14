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
#include <TFIndicators/TSSWEfficiencyRatio.h>

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

  Strategy(void);
  ~Strategy(void);

  void Start( const std::string& sSymbolPath );

  ou::ChartDataView& GetChartDataView( void ) {return m_dvChart; };

protected:
private:

  enum stateTrade {
    ETradeStart, ETradeOutOfMarket, ETradeWaitLongEntry, ETradeWaitShortEntry, ETradeCancel, ETradeClose, ETradeDone
  } m_stateTrade;

  std::stringstream m_ss;

  typedef ou::tf::CPosition::pOrder_t pOrder_t;
  typedef ou::tf::CPosition::pPosition_t pPosition_t;
  typedef ou::tf::CInstrument::pInstrument_t pInstrument_t;
  typedef ou::tf::CProviderInterfaceBase::pProvider_t pProvider_t;
  typedef ou::tf::CSimulationProvider::pProvider_t pProviderSim_t;

  typedef ou::tf::CInstrument::pInstrument_t m_pInstrument_t;
  pInstrument_t m_pShortInstrument;
  pInstrument_t m_pLongInstrument;
  pInstrument_t m_pTestInstrument;

  enum enumTradeDirection { ETradeDirUnkn, ETradeDirUp, ETradeDirDn } m_TradeDirection;
  ou::tf::CQuote m_quoteLast;  // used for classifying the current trade

  ou::tf::CQuotes m_quotes;
  ou::tf::CTrades m_trades;
  ou::tf::CBars m_bars;

  ou::tf::CBarFactory m_barFactory;

  typedef ou::ChartEntryBase::pChartEntryBase_t pChartEntryBase_t;

  ou::ChartDataView m_dvChart;
  ou::ChartEntryBars m_ceBars;
  ou::ChartEntryVolume m_ceVolume;

  ou::ChartEntryIndicator m_ceSMA1;
  ou::ChartEntryIndicator m_ceSlopeOfSMA1;
  ou::ChartEntryIndicator m_ceSlopeOfSlopeOfSMA1;
  ou::ChartEntryIndicator m_ceUpperBollinger1;
  ou::ChartEntryIndicator m_ceLowerBollinger1;
  ou::ChartEntryIndicator m_ceBollinger1Offset;
  ou::ChartEntryIndicator m_ceBollinger1Ratio;

  ou::ChartEntryIndicator m_ceSMA2;
  ou::ChartEntryIndicator m_ceSlopeOfSMA2;
  ou::ChartEntryIndicator m_ceSlopeOfSlopeOfSMA2;
  ou::ChartEntryIndicator m_ceUpperBollinger2;
  ou::ChartEntryIndicator m_ceLowerBollinger2;
  ou::ChartEntryIndicator m_ceBollinger2Offset;
  ou::ChartEntryIndicator m_ceSlopeOfBollinger2Offset;
  ou::ChartEntryIndicator m_ceBollinger2Ratio;

  ou::ChartEntryIndicator m_ceSMA3;
  ou::ChartEntryIndicator m_ceUpperBollinger3;
  ou::ChartEntryIndicator m_ceLowerBollinger3;
  ou::ChartEntryIndicator m_ceBollinger3Offset;
  ou::ChartEntryIndicator m_ceBollinger3Ratio;

  ou::ChartEntryIndicator m_cePLLong;
  ou::ChartEntryIndicator m_cePLShort;
  ou::ChartEntryIndicator m_cePLNet;
  ou::ChartEntryIndicator m_ceOutstandingLong;
  ou::ChartEntryIndicator m_ceOutstandingShort;
  ou::ChartEntryIndicator m_ceSpread;
  //ou::ChartEntryIndicator m_ceRR;

  ou::ChartEntryIndicator m_ceER1;
  ou::ChartEntryIndicator m_ceER2;
  ou::ChartEntryIndicator m_ceER3;

  //ou::ChartEntryIndicator m_ceLongTicks;
  //ou::ChartEntryIndicator m_ceShortTicks;

  ou::ChartEntryShape m_ceShorts;
  ou::ChartEntryShape m_ceLongs;

  typedef ou::tf::CSimulationProvider::pSymbol_t pSimSymbol_t;

  pPosition_t m_pPositionLong;
  pPosition_t m_pPositionShort;

  pOrder_t m_pOrder;  // active order

  OrdersOutstandingLongs* m_pOrdersOutstandingLongs;
  OrdersOutstandingShorts* m_pOrdersOutstandingShorts;

  ptime m_dtEnd;
  unsigned int m_nUpTransitions;
  unsigned int m_nDnTransitions;

  ou::tf::TSSWStatsMidQuote m_sma1;
  ou::tf::TSSWStatsMidQuote m_sma2;
  ou::tf::TSSWStatsMidQuote m_sma3;
  ou::tf::TSSWStatsMidQuote m_sma4;
  ou::tf::TSSWStatsMidQuote m_sma5;
  ou::tf::TSSWStatsMidQuote m_sma6;
  ou::tf::TSSWStatsMidQuote m_sma7;
  ou::tf::TSSWStatsMidQuote m_sma8;

  ou::tf::TSSWEfficiencyRatio m_er1;
  ou::tf::TSSWEfficiencyRatio m_er2;
  ou::tf::TSSWEfficiencyRatio m_er3;

  ou::tf::CPrices m_pricesSlopeOfSlopeOfSMA1;
  ou::tf::TSSWStatsPrice m_tsswSlopeOfSlopeOfSMA1;

  ou::tf::CPrices m_pricesSlopeOfSlopeOfSMA2;
  ou::tf::TSSWStatsPrice m_tsswSlopeOfSlopeOfSMA2;

  ou::tf::CPrices m_pricesBollinger2Offset;
  ou::tf::TSSWStatsPrice m_tsswSlopeOfBollinger2Offset;

  ou::tf::CPrices m_spreads;
  ou::tf::TSSWStatsPrice m_tsswSpreads;

  pProviderSim_t m_sim;
  bool m_bSimConnected;

  pProvider_t m_pExecutionProvider;
  pProvider_t m_pDataProvider;

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
};

