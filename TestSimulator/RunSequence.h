/************************************************************************
 * Copyright(c) 2012, One Unified. All rights reserved.                 *
 * email: info@oneunified.net                                           *
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

#include <TFTrading/InstrumentManager.h>

#include <TFTrading/OrderManager.h>
#include <TFTrading/PortfolioManager.h>

#include <TFTrading/Portfolio.h>
#include <TFTrading/Position.h>
#include <TFTrading/Instrument.h>

#include <TFTimeSeries/TimeSeries.h>

class RunSequence {
public:
  RunSequence( const boost::gregorian::date& dateStart );
  ~RunSequence(void);
  void Run( void );
protected:
private:

  enum enumTimeFrames { EPreOpen, EBellHeard, EPauseForQuotes, EAfterBell, ETrading, ECancelling, EGoingNeutral, EClosing, EAfterHours };
  enum enumTradeState { ENeutral, ELong, EShort };

  enumTimeFrames m_stateTimeFrame;
  enumTradeState m_stateTrading;

  time_duration m_timeOpeningBell, m_timeCancelTrades, m_timeClosePositions, m_timeClosingBell;
  ptime m_dtOpeningBell, m_dtStartTrading, m_dtCancelTrades, m_dtClosePositions, m_dtClosingBell;



  typedef ou::tf::CPosition::pOrder_t pOrder_t;
  typedef ou::tf::CPosition::pPosition_t pPosition_t;
  typedef ou::tf::CInstrument::pInstrument_t pInstrument_t;
  pInstrument_t m_pInstrument;
  ou::tf::CPortfolio m_portfolio;
  pPosition_t m_pPosition;
  pOrder_t m_pOrder;  // active order

  ou::tf::Quote m_quote;
  ou::tf::Trade m_trade;

  ou::tf::Quotes m_quotes;
  ou::tf::Trades m_trades;

  void HandleProviderConnected( int );
  void HandleProviderDisconnected( int );
  void HandleSimulationComplete( void );
  void HandleQuote1( const ou::tf::Quote& );  // simple simulation
  void HandleQuote2( const ou::tf::Quote& );  // for testing OptimizeStrategy code
  void Trade( void ); // used with HandleQuote2
  void HandleTrade( const ou::tf::Trade& );
  void HandlePortfolioExecution( const ou::tf::CPortfolio*  );
  void HandlePortfolioCommission( const ou::tf::CPortfolio*  );
  void HandlePositionExecution( const ou::tf::CPosition::execution_delegate_t );
  void HandlePositionCommission( const ou::tf::CPosition* );
};

