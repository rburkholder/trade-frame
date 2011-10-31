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

#include <TFIndicators/TSSWStochastic.h>
#include <TFIndicators/TSSWStats.h>
#include <TFIndicators/Pivots.h>
#include <TFTimeSeries/TimeSeries.h>
#include <TFTrading/Position.h>

struct InstrumentState {

  InstrumentState( void );
  ~InstrumentState( void ) {};

  double dblAmountToTrade;
  unsigned int nSharesToTrade;

  ou::tf::CQuotes quotes;
  ou::tf::CTrades trades;

  ou::tf::CTrades history;

  //double dblOpen, dblHigh, dblLow, dblClose;

  double dblMidQuoteAtOpen;
  double dblOpeningTrade;

  ou::tf::TSSWStochastic stochFast;
  ou::tf::TSSWStochastic stochMed;
  ou::tf::TSSWStochastic stochSlow;

  ou::tf::TSSWStatsMidQuote statsFast;
  ou::tf::TSSWStatsMidQuote statsMed;
  ou::tf::TSSWStatsMidQuote statsSlow;

  //ou::tf::CPivotSet pivots;

  time_duration tdMarketOpen;
  time_duration tdMarketOpenIdle;
  //time_duration tdMarketTrading;
  time_duration tdCancelOrders;
  time_duration tdClosePositions;
  time_duration tdAfterMarket;
  time_duration tdMarketClosed;

  ptime dtPreTradingStop;

  ou::tf::CPosition::pPosition_t pPosition;

  bool bMarketHoursCrossMidnight;
  bool bDaySession;

  typedef std::vector<double> vZeroMark_t;
  typedef vZeroMark_t::const_iterator vZeroMark_iter_t;
  vZeroMark_t vZeroMarks;
  vZeroMark_iter_t iterZeroMark;
  vZeroMark_iter_t iterNextMark; // relative to the zero mark, if long, higher mark, if short, lower mark
};


