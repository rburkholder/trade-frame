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

#include "NoRiskInterestRateSeries.h"
#include "Binomial.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

class Option: public ou::tf::Watch {
public:

  using pOption_t = std::shared_ptr<Option>;
  using pInstrument_t = Instrument::pInstrument_t;
  using pProvider_t = ou::tf::ProviderInterfaceBase::pProvider_t;

  using fCallbackWithGreek_t = std::function<void(const Greek&)>;

  Option( pInstrument_t pInstrument, pProvider_t pDataProvider, pProvider_t pGreekProvider );
  Option( pInstrument_t pInstrument, pProvider_t pDataProvider );  // Greek calculations locally
  Option( const Option& );
  virtual ~Option();

  Option& operator=( const Option& rhs );

  bool virtual operator< ( const Option& rhs ) const { return m_dblStrike <  rhs.m_dblStrike; };
  bool virtual operator<=( const Option& rhs ) const { return m_dblStrike <= rhs.m_dblStrike; };

  double GetStrike() const { return m_dblStrike; }
  boost::gregorian::date GetExpiry() const { return m_pInstrument->GetExpiry(); }
  ou::tf::OptionSide::EOptionSide GetOptionSide() const { return m_pInstrument->GetOptionSide(); }

  static void CalcRate( // basic libor calcs
    ou::tf::option::binomial::structInput& input,
    const ou::tf::NoRiskInterestRateSeries& libor,
    boost::posix_time::ptime dtUtcNow, boost::posix_time::ptime dtUtcExpiry );
  // calls static CalcRate with specific expiry info
  void CalcRate( ou::tf::option::binomial::structInput& input, const ptime dtUtcNow, const ou::tf::NoRiskInterestRateSeries& libor );
  // caller needs to have updated input with CalcRate
  void CalcGreeks( ou::tf::option::binomial::structInput& input, ptime dtUtcNow, bool bNeedsGuess = true ); // Calc and Append

  double ImpliedVolatility() const { return m_greek.ImpliedVolatility(); };
  double Delta() const { return m_greek.Delta(); }
  double Gamma() const { return m_greek.Gamma(); }
  double Theta() const { return m_greek.Theta(); }
  double Vega() const { return m_greek.Vega(); }
  double Rho() const { return m_greek.Rho(); }

  ou::tf::Greeks* Greeks() { return &m_greeks; };

  virtual bool StartWatch();
  virtual bool StopWatch();

  virtual void EmitValues( double dblPriceUnderlying, bool bEmitName = true );
  void NetGreeks( const double quantity, double& delta, double& gamma ) const;

  // TODO: needs spinlock
  inline const Greek& LastGreek() const { return m_greek; };

  ou::Delegate<const Greek&> OnGreek;

  void SaveSeries( const std::string& sPrefix );

protected:

  double m_dblStrike;
  Greek m_greek;

  ou::tf::Greeks m_greeks;

  pProvider_t m_pGreekProvider;

private:

  void Initialize();

  void HandleGreek( const Greek& greek );
  void AppendGreek( const Greek& greek );

};

//
// ==================
//

class Call: public Option {
public:
  Call( pInstrument_t pInstrument, pProvider_t pDataProvider, pProvider_t pGreekProvider );
  Call( pInstrument_t pInstrument, pProvider_t pDataProvider );
  virtual ~Call() {};
protected:
private:
};

//
// ==================
//

class Put: public Option {
public:
  Put( pInstrument_t pInstrument, pProvider_t pDataProvider, pProvider_t pGreekProvider );
  Put( pInstrument_t pInstrument, pProvider_t pDataProvider );
  virtual ~Put() {};
protected:
private:
};

} // namespace option
} // namespace tf
} // namespace ou

