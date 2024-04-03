/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
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

#include "DatedDatum.h"

#include <OUCommon/FastDelegate.h>
using namespace fastdelegate;

namespace ou { // One Unified
namespace tf { // TradeFrame

class BarFactory {
public:

  typedef unsigned long duration_t;  // seconds
  typedef Bar::volume_t volume_t;
  typedef Bar::price_t price_t;

  BarFactory( duration_t nSeconds = 60);
  virtual ~BarFactory();
  void Add( const ptime &, price_t, volume_t);
  void Add( const Trade &trade ) { Add( trade.DateTime(), trade.Price(), trade.Volume() ); };
  Bar getCurrentBar() const { return m_bar; };
  void SetBarWidth( duration_t seconds ) { m_nBarWidthSeconds = seconds; };
  duration_t GetBarWidth() const { return m_nBarWidthSeconds; };

  typedef FastDelegate1<const Bar&> OnNewBarStartedHandler;  // turn this into a phoenix lambda function?
  void SetOnNewBarStarted( OnNewBarStartedHandler function ) {
    OnNewBarStarted = function;
  }
  typedef FastDelegate1<const Bar&> OnBarUpdatedHandler;  // turn this into a phoenix lambda function?
  void SetOnBarUpdated( OnBarUpdatedHandler function ) {  // called at most once a second
    OnBarUpdated = function;
  }
  typedef FastDelegate1<const Bar&> OnBarCompleteHandler;  // turn this into a phoenix lambda function?
  void SetOnBarComplete( OnBarCompleteHandler function ) {
    OnBarComplete = function;
  }

protected:

  duration_t m_nBarWidthSeconds;
  duration_t m_curInterval; // current bar interval
  ptime m_dtBarStart;
  ptime m_dtLastIntermediateEmission; // changes emitted no less than 1 second apart
  boost::posix_time::time_duration m_1Sec;

  Bar m_bar;

  OnNewBarStartedHandler OnNewBarStarted;
  OnBarUpdatedHandler OnBarUpdated;
  OnBarCompleteHandler OnBarComplete;

private:
};

} // namespace tf
} // namespace ou
