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

#include <OUCommon/FastDelegate.h>
using namespace fastdelegate;

namespace ou { // One Unified
namespace tf { // TradeFrame

class CBarFactory {
public:

  typedef unsigned long duration_t;
  typedef CBar::volume_t volume_t;
  typedef CBar::price_t price_t;

  CBarFactory( duration_t nSeconds = 60);
  virtual ~CBarFactory(void);
  void Add( const ptime &, price_t, volume_t);
  void Add( const CTrade &trade ) { Add( trade.DateTime(), trade.Trade(), trade.Volume() ); };
  const CBar& getCurrentBar() const { return m_bar; };
  void SetBarWidth( duration_t seconds ) { m_nBarWidthSeconds = seconds; };
  duration_t GetBarWidth( void ) const { return m_nBarWidthSeconds; };

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
  duration_t m_nBarWidthSeconds;
  duration_t m_curInterval; // current bar interval
  ptime m_dtBarStart;
  ptime m_dtLastIntermediateEmission; // changes emitted no less than 1 second apart
  boost::posix_time::time_duration m_1Sec;
private:
};

} // namespace tf
} // namespace ou
