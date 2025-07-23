/************************************************************************
 * Copyright(c) 2013, One Unified. All rights reserved.                 *
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

#include <set>
#include <algorithm>

#include <boost/log/trivial.hpp>
#include <boost/assign/std/vector.hpp>

#include <TFTrading/InstrumentManager.h>

#include "NoRiskInterestRateSeries.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

NoRiskInterestRateSeries::NoRiskInterestRateSeries()
  : m_bInitialized( false ), m_bWatching( false ), m_sDescription( "rates")
{
}

NoRiskInterestRateSeries::~NoRiskInterestRateSeries() {
}

void NoRiskInterestRateSeries::Initialize() {

  ou::tf::InstrumentManager& mgr( ou::tf::InstrumentManager::GlobalInstance() );
  ou::tf::Instrument::pInstrument_t pInstrument;
  for ( vInterestRate_iter_t iter = m_vInterestRate.begin(); m_vInterestRate.end() != iter; ++iter ) {
    // need to check if it already exists
    if ( mgr.Exists( iter->Symbol, pInstrument ) ) {
    }
    else {
      pInstrument = mgr.ConstructInstrument( iter->Symbol, "INDEX", ou::tf::InstrumentType::Index );
    }
    iter->pWatch.reset( new Watch( pInstrument, m_pProvider ) );
  }
}

void NoRiskInterestRateSeries::SetWatchOn( pProvider_t pProvider ) {
  assert( ou::tf::keytypes::EProviderIQF == pProvider->ID() );
  if ( !m_bInitialized) {
    m_pProvider = pProvider;
    Initialize();
    m_bInitialized = true;
  }
  if ( !m_bWatching ) {
    m_bWatching = true;
    for ( vInterestRate_iter_t iter = m_vInterestRate.begin(); m_vInterestRate.end() != iter; ++ iter ) {
      iter->pWatch->StartWatch();
    }
  }
}

void NoRiskInterestRateSeries::SetWatchOff() {
  if ( m_bWatching ) {
    m_bWatching = false;
    for ( vInterestRate_iter_t iter = m_vInterestRate.begin(); m_vInterestRate.end() != iter; ++ iter ) {
      iter->pWatch->StopWatch();
    }
  }
}

void NoRiskInterestRateSeries::SaveSeries( const std::string& sPrefix ) {

  //const std::string sPath( sPrefix + '/' + m_sDescription );
  const std::string& sPath( sPrefix );

  using setName_t = std::set<std::string>;
  setName_t setName; // ignore duplicates
  for ( vInterestRate_t::iterator iter = m_vInterestRate.begin(); m_vInterestRate.end() != iter; ++iter ) {
    const std::string& sName( iter->pWatch->GetInstrumentName() );
    if ( setName.end() == setName.find( sName ) ) {
      setName.emplace( sName );
      BOOST_LOG_TRIVIAL(info) << "NoRiskInterestRate saving " << sName << " to " << sPath;
      iter->pWatch->SaveSeries( sPath );
    }
  }
}

double NoRiskInterestRateSeries::ValueAt( time_duration td ) const {

  assert( m_bWatching );

  assert( boost::posix_time::not_a_date_time != td );
  assert( td >= m_vInterestRate[ 0 ].td );
  //assert( m_bWatching );  // don't worry about this as the values don't vary much anyway

  structInterestRate tmp( td, "" );
  vInterestRate_citer_t iter1 = std::lower_bound( m_vInterestRate.begin(), m_vInterestRate.end(), tmp, compareInterestRate() );
  vInterestRate_citer_t iter2;

  if ( m_vInterestRate.end() == iter1 ) {
    // extrapolate beyond end
    --iter1;
    iter2 = iter1;
    -- iter1;
  }
  else {
    if ( iter1->td == td ) {
      // use the assigned value
      return iter1->pWatch->LastTrade().Price();
    }
    else {
      // interpolate
      iter2 = iter1;
      --iter1;
    }
  }
  double rate = iter1->pWatch->LastTrade().Price() +
    ( ( iter2->pWatch->LastTrade().Price() - iter1->pWatch->LastTrade().Price() ) *
    ( ( (double)( td - iter1->td ).total_seconds() ) / ( (double) ( iter2->td - iter1->td ).total_seconds() ) ) );
  return rate;
}

//void NoRiskInterestRateSeries::EmitYieldCurve() {
//  for ( vInterestRate_iter_t iter = m_vInterestRate.begin(); m_vInterestRate.end() != iter; ++ iter ) {
//    std::cout << iter->Symbol << " " << iter->pWatch->LastTrade().Price() << std::endl;
//  }
//}

std::ostream& operator<<( std::ostream& os, const NoRiskInterestRateSeries& nrirs ) {
  for ( NoRiskInterestRateSeries::vInterestRate_t::const_iterator iter = nrirs.m_vInterestRate.begin(); nrirs.m_vInterestRate.end() != iter; ++iter ) {
    os << iter->Symbol << " " << iter->pWatch->LastTrade().Price() << std::endl;
  }
  return os;
}

void NoRiskInterestRateSeries::AssignSymbols( const vSymbol_t& vSymbol ) {

  for ( vSymbol_t::const_iterator iter = vSymbol.begin(); vSymbol.end() != iter; ++ iter ) {
    m_vInterestRate.push_back( *iter );
  }

}

// *** LiborFromIQFeed

using namespace boost::assign;

LiborFromIQFeed::LiborFromIQFeed(): NoRiskInterestRateSeries() {
  m_sDescription = "libor";
  using structSymbol = NoRiskInterestRateSeries::structSymbol;
  NoRiskInterestRateSeries::vSymbol_t vLibor;
  // http://www.global-rates.com/interest-rates/libor/american-dollar/american-dollar.aspx
  //   shows some are not available
  // also based upon that chart, the values here do correspond to USD libor
  vLibor +=
    structSymbol( time_duration( hours(   0 * 24 ) ),  "ONLIB.X" ), // overnight
    //structSymbol( time_duration( hours(   7 * 24 ) ),  "1WLIB.X" ), //  1 week - no longer present
    //structSymbol( time_duration( hours(  14 * 24 ) ),  "2WLIB.X" ), //  2 week
    structSymbol( time_duration( hours(  30 * 24 ) ),  "1MLIB.X" ), //  1 month
    //structSymbol( time_duration( hours(  60 * 24 ) ),  "2MLIB.X" ), //  2 month - no longer present
    structSymbol( time_duration( hours(  90 * 24 ) ),  "3MLIB.X" ), //  3 month
    //structSymbol( time_duration( hours( 120 * 24 ) ),  "4MLIB.X" ), //  4 month
    //structSymbol( time_duration( hours( 150 * 24 ) ),  "5MLIB.X" ), //  5 month
    structSymbol( time_duration( hours( 180 * 24 ) ),  "6MLIB.X" ), //  6 month
    //structSymbol( time_duration( hours( 210 * 24 ) ),  "7MLIB.X" ), //  7 month
    //structSymbol( time_duration( hours( 240 * 24 ) ),  "8MLIB.X" ), //  8 month
    //structSymbol( time_duration( hours( 270 * 24 ) ),  "9MLIB.X" ), //  9 month
    //structSymbol( time_duration( hours( 300 * 24 ) ), "10MLIB.X" ), // 10 month
    //structSymbol( time_duration( hours( 330 * 24 ) ), "11MLIB.X" ), // 11 month
    structSymbol( time_duration( hours( 365 * 24 ) ),  "1YLIB.X" ); //  1 year
  NoRiskInterestRateSeries::AssignSymbols( vLibor );
}

LiborFromIQFeed::~LiborFromIQFeed() {
}

// *** FedRateFromIQFeed

FedRateFromIQFeed::FedRateFromIQFeed(): NoRiskInterestRateSeries() {
  m_sDescription = "fedrate";
  using structSymbol = NoRiskInterestRateSeries::structSymbol;
  NoRiskInterestRateSeries::vSymbol_t vFedRate;
  vFedRate +=
    structSymbol( time_duration( hours(   0 * 24 ) ),  "TB30.X" ), // overnight, base at 0 days needed for algorithm
    structSymbol( time_duration( hours(  30 * 24 ) ),  "TB30.X" ), //  30 day
    structSymbol( time_duration( hours(  90 * 24 ) ),  "TB90.X" ), //  90 day
    structSymbol( time_duration( hours( 180 * 24 ) ),  "TB180.X" ), //  180 day
    structSymbol( time_duration( hours( 365 * 24 ) ),  "1YCMY.X" ); //  1 year
  // these are 10x actual value:
//TNX.XO	CBOE TREASURY YIELD 10 YEAR	CBOE	CBOE	INDEX	/ 1000
//TYX.XO	CBOE 30 YEAR TREASURY YIELD INDEX	CBOE	CBOE	INDEX	/ 1000
  NoRiskInterestRateSeries::AssignSymbols( vFedRate );

}

FedRateFromIQFeed::~FedRateFromIQFeed() {
}

} // namespace tf
} // namespace ou
