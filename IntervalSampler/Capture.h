/************************************************************************
 * Copyright(c) 2019, One Unified. All rights reserved.                 *
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

/*
 * File:    Capture.h
 * Author:  raymond@burkholder.net
 * Project: IntervalSampler
 * Created on August 21, 2019, 11:10 AM
 */

#ifndef CAPTURE_H
#define CAPTURE_H

#include <OUCommon/SpinLock.h>

#include <TFTimeSeries/BarFactory.h>

#include <TFTrading/Watch.h>

struct Capture {
public:

  using pWatch_t = ou::tf::Watch::pWatch_t;

  using fBarComplete_t
    = std::function<void(
        const ou::tf::Instrument::idInstrument_t&,
        size_t,
        const ou::tf::Bar&,
        const ou::tf::Quote&,
        const ou::tf::Trade&
        )>;

  Capture();
  ~Capture();

  void Assign(
    ou::tf::BarFactory::duration_t duration,
    pWatch_t pWatch,
    fBarComplete_t&& fBarComplete
  );

  void Pull(
    bool& bBarReady, ou::tf::Bar& bar,
    bool& bQuoteReady, ou::tf::Quote& quote,
    bool& bTradeReady, ou::tf::Trade& trade
  );

protected:
private:
  size_t m_nSequence;

  pWatch_t m_pWatch;
  ou::tf::BarFactory m_bf;
  fBarComplete_t m_fBarComplete;

  ou::SpinLock m_spinlock;

  bool m_bQuoteReady;
  ou::tf::Quote m_quote;

  bool m_bTradeReady;
  ou::tf::Trade m_trade;

  bool m_bBarReady;
  boost::posix_time::ptime m_dtBarStart;
  double m_dblHigh;
  double m_dblLow;
  double m_dblOpen;
  double m_dblClose;
  size_t m_nVolume;

  void HandleQuote( const ou::tf::Quote& quote );
  void HandleTrade( const ou::tf::Trade& trade );
  void HandleBarComplete( const ou::tf::Bar& bar );

  void BarAddTrade( const ou::tf::Trade& trade );

};

#endif /* CAPTURE_H */

