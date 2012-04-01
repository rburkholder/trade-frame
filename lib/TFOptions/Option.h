/************************************************************************
 * Copyright(c) 2011, One Unified. All rights reserved.                 *
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

// 2012/02/05 http://seekingalpha.com/article/274736-option-expiration-for-metals-lots-of-puts-on-slv-more-calls-on-gld
// 2012/03/31 be aware that some options do not expire on friday.  Some like, next week, 
//            expire on thursday due to good friday being a holiday

#include <TFTimeSeries/TimeSeries.h>

#include <TFTrading/Instrument.h>
#include <TFTrading/ProviderInterface.h>

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

class Option {
public:

  typedef CInstrument::pInstrument_t pInstrument_t;
  typedef ou::tf::CProviderInterfaceBase::pProvider_t pProvider_t;

  Option( pInstrument_t pInstrument, pProvider_t pDataProvider, pProvider_t pGreekProvider );
  Option( const Option& rhs );
  virtual ~Option( void );

  Option& operator=( const Option& rhs );

  bool operator< ( const Option& rhs ) const { return m_dblStrike <  rhs.m_dblStrike; };
  bool operator<=( const Option& rhs ) const { return m_dblStrike <= rhs.m_dblStrike; };

  double GetStrike( void ) const { return m_dblStrike; };
  pInstrument_t GetInstrument( void ) { return m_pInstrument; };

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

  void StartWatch( void );
  void StopWatch( void );

  void SaveSeries( const std::string& sPrefix );

protected:

  std::string m_sSide;

  // use an interator instead?  or keep as is as it facilitates multithread append and access operations
  // or will the stuff in TBB help with this type of access?
  double m_dblBid;
  double m_dblAsk;
  double m_dblTrade;

  double m_dblStrike;
  CGreek m_greek;

  CQuotes m_quotes;
  CTrades m_trades;
  CGreeks m_greeks;

  pInstrument_t m_pInstrument;

  pProvider_t m_pDataProvider;
  pProvider_t m_pGreekProvider;

  std::stringstream m_ss;

private:

  unsigned int m_cntWatching;  // maybe implement counter at some point to allow multiple calls

  void Initialize( void );

  void HandleQuote( const CQuote& quote );
  void HandleTrade( const CTrade& trade );
  void HandleGreek( const CGreek& greek );

};

//
// ==================
//

class Call: public Option
{
public:
  Call( pInstrument_t pInstrument, pProvider_t pDataProvider, pProvider_t pGreekProvider );
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
  Put( pInstrument_t pInstrument, pProvider_t pDataProvider, pProvider_t pGreekProvider );
  virtual ~Put( void ) {};
protected:
private:
};



} // namespace option
} // namespace tf
} // namespace ou

