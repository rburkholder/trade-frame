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
  RunSequence(void);
  ~RunSequence(void);
  void Run( void );
protected:
private:

  typedef ou::tf::CPosition::pOrder_t pOrder_t;
  typedef ou::tf::CPosition::pPosition_t pPosition_t;
  typedef ou::tf::CInstrument::pInstrument_t pInstrument_t;
  pInstrument_t m_pInstrument;
  ou::tf::CPortfolio m_portfolio;
  pPosition_t m_pPosition;
  pOrder_t m_pOrder;  // active order

  ou::tf::Quotes m_quotes;
  ou::tf::Trades m_trades;

  void HandleProviderConnected( int );
  void HandleProviderDisconnected( int );
  void HandleSimulationComplete( void );
  void HandleQuote( const ou::tf::Quote& );
  void HandleTrade( const ou::tf::Trade& );
  void HandlePortfolioExecution( const ou::tf::CPortfolio*  );
  void HandlePortfolioCommission( const ou::tf::CPortfolio*  );
  void HandlePositionExecution( const ou::tf::CPosition::execution_delegate_t );
  void HandlePositionCommission( const ou::tf::CPosition* );
};

