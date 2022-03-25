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


#include "client/EWrapper.h"

#include <TFTimeSeries/DatedDatum.h>

#include <TFTrading/Symbol.h>

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace ib { // Interactive Brokers

class Symbol : public ou::tf::Symbol<Symbol> {
  friend class TWS;
public:

  using inherited_t = ou::tf::Symbol<Symbol>;
  using pInstrument_t = inherited_t::pInstrument_t;
  using pSymbol_t = inherited_t::pSymbol_t;

  //IBSymbol( TickerId id, pInstrument_t pInstrument );
  Symbol( inherited_t::idSymbol_t, pInstrument_t pInstrument, TickerId id );
  Symbol( pInstrument_t pInstrument, TickerId id );
  virtual ~Symbol(void);

  TickerId GetTickerId( void ) { return m_TickerId; };

  void Greeks( double optPrice, double undPrice, double pvDividend,
    double impliedVol, double delta, double gamma, double vega, double theta );

  double OptionPrice( void ) { return m_dblOptionPrice; };

protected:

  TickerId m_TickerId;

  bool m_bAskFound;
  bool m_bAskSizeFound;
  bool m_bBidFound;
  bool m_bBidSizeFound;

  bool m_bLastTimeStampFound;
  bool m_bLastFound;
  bool m_bLastSizeFound;

  uint32_t m_nAskSize;
  uint32_t m_nBidSize;
  uint32_t m_nLastSize;
  uint32_t m_nVolume;

  double m_dblAsk;
  double m_dblBid;
  double m_dblLast;

  double m_dblHigh;
  double m_dblLow;
  double m_dblClose;

  double m_dblOptionPrice;
  double m_dblUnderlyingPrice;
  double m_dblPvDividend;

  void SetQuoteTradeWatchInProgress( void ) { m_bQuoteTradeWatchInProgress = true; };
  void ResetQuoteTradeWatchInProgress( void ) { m_bQuoteTradeWatchInProgress = false; };
  bool GetQuoteTradeWatchInProgress( void ) { return m_bQuoteTradeWatchInProgress; };
  bool m_bQuoteTradeWatchInProgress;

  void SetDepthWatchInProgress( void ) { m_bDepthWatchInProgress = true; };
  void ResetDepthWatchInProgress( void ) { m_bDepthWatchInProgress = false; };
  bool GetDepthWatchInProgress( void ) { return m_bDepthWatchInProgress; };
  bool m_bDepthWatchInProgress;

  void AcceptTickPrice( TickType tickType, double price );
  void AcceptTickSize( TickType tickType, Decimal size );
  void AcceptTickString( TickType tickType, const std::string& value );

  void BuildQuote( void );
  void BuildTrade( void );

private:

  long m_conId;  // matches IB contract id

};

} // namespace ib
} // namespace tf
} // namespace ou
