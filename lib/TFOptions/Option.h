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

// 20120129 Inherit from <TFTrading/InstrumentData.h> ?

#include <TFTimeSeries/TimeSeries.h>

#include <TFTrading/Instrument.h>

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

class Option {
public:

  typedef CInstrument::pInstrument_t pInstrument_t;

  Option( pInstrument_t pInstrument );
  Option( const Option& rhs );
  virtual ~Option( void ) {};

  Option& operator=( const Option& rhs );

  bool operator< ( const Option& rhs ) const { return m_dblStrike <  rhs.m_dblStrike; };
  bool operator<=( const Option& rhs ) const { return m_dblStrike <= rhs.m_dblStrike; };

  double GetStrike( void ) { return m_dblStrike; };
  pInstrument_t GetInstrument( void ) { return m_pInstrument; };

  void HandleQuote( const CQuote& quote );
  void HandleTrade( const CTrade& trade );
  void HandleGreek( const CGreek& greek );

  double Bid( void ) const { return m_dblBid; };
  double Ask( void ) const { return m_dblAsk; };

  double ImpliedVolatility( void ) const { return m_greek.ImpliedVolatility(); };
  double Delta( void ) const { return m_greek.Delta(); };
  double Gamma( void ) const { return m_greek.Gamma(); };
  double Theta( void ) const { return m_greek.Theta(); };
  double Vega( void ) const { return m_greek.Vega(); };

  CQuotes* Quotes( void ) { return &m_quotes; };
  CTrades* Trades( void ) { return &m_trades; };
  CGreeks* Greeks( void ) { return &m_greeks; };

protected:

  std::string m_sSide;

  double m_dblBid;
  double m_dblAsk;
  double m_dblTrade;

  double m_dblStrike;
  CGreek m_greek;

  CQuotes m_quotes;
  CTrades m_trades;
  CGreeks m_greeks;

  bool m_bWatching;

  pInstrument_t m_pInstrument;

  std::stringstream m_ss;

private:
};

//
// ==================
//

class Call: public Option
{
public:
  Call( pInstrument_t pInstrument );
  virtual ~Call( void ) {};
protected:
private:
};

//
// ==================
//

class Put: public Option
{
public:
  Put( pInstrument_t pInstrument );
  virtual ~Put( void ) {};
protected:
private:
};



} // namespace option
} // namespace tf
} // namespace ou

