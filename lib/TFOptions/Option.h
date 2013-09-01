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

#include <TFTrading/Watch.h>

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

class Option: public ou::tf::Watch {
public:

  typedef Instrument::pInstrument_t pInstrument_t;
  typedef ou::tf::ProviderInterfaceBase::pProvider_t pProvider_t;

  Option( pInstrument_t pInstrument, pProvider_t pDataProvider, pProvider_t pGreekProvider );
  Option( const Option& rhs );
  virtual ~Option( void );

  Option& operator=( const Option& rhs );

  bool virtual operator< ( const Option& rhs ) const { return m_dblStrike <  rhs.m_dblStrike; };
  bool virtual operator<=( const Option& rhs ) const { return m_dblStrike <= rhs.m_dblStrike; };

  double GetStrike( void ) const { return m_dblStrike; };

  double ImpliedVolatility( void ) const { return m_greek.ImpliedVolatility(); };
  double Delta( void ) const { return m_greek.Delta(); };
  double Gamma( void ) const { return m_greek.Gamma(); };
  double Theta( void ) const { return m_greek.Theta(); };
  double Vega( void ) const { return m_greek.Vega(); };

  Greeks* Greeks( void ) { return &m_greeks; };

  void StartWatch( void );
  bool StopWatch( void );

  virtual void EmitValues( void );

  void AppendGreek( const ou::tf::Greek& greek );

  void SaveSeries( const std::string& sPrefix );

protected:

  std::string m_sSide;

  double m_dblStrike;
  Greek m_greek;

  ou::tf::Greeks m_greeks;

  pProvider_t m_pGreekProvider;

private:

  void Initialize( void );

  void HandleGreek( const Greek& greek );

};

//
// ==================
//

class Call: public Option {
public:
  Call( pInstrument_t pInstrument, pProvider_t pDataProvider, pProvider_t pGreekProvider );
  virtual ~Call( void ) {};
protected:
private:
};

//
// ==================
//

class Put: public Option {
public:
  Put( pInstrument_t pInstrument, pProvider_t pDataProvider, pProvider_t pGreekProvider );
  virtual ~Put( void ) {};
protected:
private:
};



} // namespace option
} // namespace tf
} // namespace ou

