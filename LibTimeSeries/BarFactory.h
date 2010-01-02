/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
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

#include "DatedDatum.h"

#include "LibCommon/FastDelegate.h"
using namespace fastdelegate;

class CBarFactory {
public:
  CBarFactory(long nSeconds = 60);
  virtual ~CBarFactory(void);
  void Add( const ptime &, double, unsigned int);
  void Add( const CTrade &trade ) { Add( trade.m_dt, trade.m_dblTrade, trade.m_nTradeSize ); };
  const CBar &getCurrentBar() { return m_bar; };
  void SetBarWidth( long seconds ) { m_nBarWidthSeconds = seconds; };
  long GetBarWidth( void ) { return m_nBarWidthSeconds; };

  typedef FastDelegate1<const CBar &> OnNewBarStartedHandler;
  void SetOnNewBarStarted( OnNewBarStartedHandler function ) {
    OnNewBarStarted = function;
  }
  typedef FastDelegate1<const CBar &> OnBarUpdatedHandler;
  void SetOnBarUpdated( OnBarUpdatedHandler function ) {  // called at most once a second
    OnBarUpdated = function;
  }
  typedef FastDelegate1<const CBar &> OnBarCompleteHandler;
  void SetOnBarComplete( OnBarCompleteHandler function ) {
    OnBarComplete = function;
  }

protected:
  CBar m_bar;
  OnNewBarStartedHandler OnNewBarStarted;
  OnBarUpdatedHandler OnBarUpdated;
  OnBarCompleteHandler OnBarComplete;
  long m_nBarWidthSeconds;
  long m_curInterval; // current bar interval
  ptime m_dtBarStart;
  ptime m_dtLastIntermediateEmission; // changes emitted no less than 1 second apart
  boost::posix_time::time_duration m_1Sec;
private:
};
