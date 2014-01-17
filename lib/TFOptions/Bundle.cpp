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

#include <sstream>

#include <TFHDF5TimeSeries/HDF5DataManager.h>
#include <TFHDF5TimeSeries/HDF5WriteTimeSeries.h>
#include <TFHDF5TimeSeries/HDF5Attribute.h>

#include "Bundle.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

ExpiryBundle::ExpiryBundle(void)
  : m_stateOptionWatch( EOWSNoWatch ), //m_bWatching( false ), 
  m_dblUpperTrigger( 0.0 ), m_dblLowerTrigger( 0.0 ), m_bfIVUnderlyingCall( 86400 ), m_bfIVUnderlyingPut( 86400 )
{
}

ExpiryBundle::~ExpiryBundle(void) {
}

void ExpiryBundle::SaveSeries( const std::string& sPrefix60sec, const std::string& sPrefix86400sec ) {
//  if ( 0 != m_pwatchUnderlying.get() ) {
//    m_pwatchUnderlying->SaveSeries( sPrefix60sec );
//  }
  for ( mapStrikes_t::iterator iter = m_mapStrikes.begin(); m_mapStrikes.end() != iter; ++iter ) {
    iter->second.SaveSeries( sPrefix60sec );
  }
  SaveAtmIv( sPrefix60sec, sPrefix86400sec );
}

void ExpiryBundle::SaveAtmIv( const std::string& sPrefix60sec, const std::string& sPrefix86400sec ) {

  std::string sPathName;

  ou::tf::HDF5DataManager dm( ou::tf::HDF5DataManager::RDWR );

  if ( 0 != m_tsAtmIv.Size() ) {

    std::stringstream ss;
    //ss.str( "" );
    // http://www.boost.org/doc/libs/1_54_0/doc/html/date_time/date_time_io.html
    //boost::posix_time::time_facet* pFacet( new boost::posix_time::time_facet( "%Y-%m-%d" ) );
    boost::gregorian::date_facet* pFacet( new boost::gregorian::date_facet( "%Y-%m-%d" ) );
    ss.imbue( std::locale( ss.getloc(), pFacet ) );
    ss << m_dtExpiry.date();
  
    sPathName = sPrefix60sec + "/atmiv/" + ss.str();
    HDF5WriteTimeSeries<ou::tf::PriceIVs> wtsAtmIv( dm, true, true, 5, 256 );
    wtsAtmIv.Write( sPathName, &m_tsAtmIv );
    HDF5Attributes attrAtmIv( dm, sPathName );
    attrAtmIv.SetSignature( ou::tf::PriceIV::Signature() );

    {
      sPathName = sPrefix86400sec + "/" + ss.str() + "/call";
      ou::tf::Bars bars( 1 );
      bars.Append( m_bfIVUnderlyingCall.getCurrentBar() );
      HDF5WriteTimeSeries<ou::tf::Bars> wtsBar( dm, true, true, 5, 16 );
      wtsBar.Write( sPathName, &bars );
      HDF5Attributes attrBar( dm, sPathName );
      try {
        attrBar.SetSignature( ou::tf::Bar::Signature() );
      }
        catch (...) {  // may already exist
      }
    }
  
    {
      sPathName = sPrefix86400sec + "/" + ss.str() + "/put";
      ou::tf::Bars bars( 1 );
      bars.Append( m_bfIVUnderlyingPut.getCurrentBar() );
      HDF5WriteTimeSeries<ou::tf::Bars> wtsBar( dm, true, true, 5, 16 );
      wtsBar.Write( sPathName, &bars );
      HDF5Attributes attrBar( dm, sPathName );
      try {
        attrBar.SetSignature( ou::tf::Bar::Signature() );
      }
      catch (...) {  // may already exist
      }
    }
  }
  
}

void ExpiryBundle::EmitValues( void ) {
//  if ( 0 != m_pwatchUnderlying.get() ) {
//    m_pwatchUnderlying->EmitValues();
//  }
  for ( mapStrikes_t::iterator iter = m_mapStrikes.begin(); m_mapStrikes.end() != iter; ++iter ) {
    iter->second.EmitValues();
  }
}

void ExpiryBundle::SetCall( pInstrument_t pInstrument, pProvider_t pDataProvider, pProvider_t pGreekProvider ) {
  mapStrikes_t::iterator iter = FindStrikeAuto( pInstrument->GetStrike() );
  iter->second.AssignCall( pInstrument, pDataProvider, pGreekProvider );
}

void ExpiryBundle::SetPut( pInstrument_t pInstrument, pProvider_t pDataProvider, pProvider_t pGreekProvider ) {
  mapStrikes_t::iterator iter = FindStrikeAuto( pInstrument->GetStrike() );
  iter->second.AssignPut( pInstrument, pDataProvider, pGreekProvider );
}

Call* ExpiryBundle::GetCall( double dblStrike ) {
  mapStrikes_t::iterator iter = FindStrike( dblStrike );
  return iter->second.Call();
}

Put* ExpiryBundle::GetPut( double dblStrike ) {
  mapStrikes_t::iterator iter = FindStrike( dblStrike );
  return iter->second.Put();
}

void ExpiryBundle::StartWatch( void ) {
//  if ( !m_bWatching ) {
//    m_bWatching = true;
//    if ( 0 != m_pwatchUnderlying.get() ) m_pwatchUnderlying->StartWatch();
    for ( mapStrikes_t::iterator iter = m_mapStrikes.begin(); m_mapStrikes.end() != iter; ++iter ) {
      iter->second.WatchStart();
    }
//  }
}

void ExpiryBundle::StopWatch( void ) {
//  if ( m_bWatching ) {
//    m_bWatching = false;
//    if ( 0 != m_pwatchUnderlying.get() ) m_pwatchUnderlying->StopWatch();
    for ( mapStrikes_t::iterator iter = m_mapStrikes.begin(); m_mapStrikes.end() != iter; ++iter ) {
      if ( iter->second.IsWatching() )
        iter->second.WatchStop();
    }
//  }
}

void ExpiryBundle::SetWatchableOn( double dblStrike ) {
  mapStrikes_t::iterator iter = m_mapStrikes.find( dblStrike );
  if ( m_mapStrikes.end() != iter ) {
    iter->second.SetWatchableOn();
  }
  
}

void ExpiryBundle::SetWatchableOff( double dblStrike ) {
  mapStrikes_t::iterator iter = m_mapStrikes.find( dblStrike );
  if ( m_mapStrikes.end() != iter ) {
    iter->second.SetWatchableOff();
  }
}

void ExpiryBundle::SetWatchOn( double dblStrike, bool bForce ) {
  mapStrikes_t::iterator iter = m_mapStrikes.find( dblStrike );
  assert( m_mapStrikes.end() != iter );
  if ( bForce ) {
    iter->second.SetWatchableOn();
  }
  iter->second.WatchStart();
  OnStrikeWatchOn( iter->second );
}

void ExpiryBundle::SetWatchOff( double dblStrike, bool bForce ) {
  mapStrikes_t::iterator iter = m_mapStrikes.find( dblStrike );
  assert( m_mapStrikes.end() != iter );
  OnStrikeWatchOff( iter->second );
  iter->second.WatchStop();
  if ( bForce ) {
    iter->second.SetWatchableOff();
  }
}

ExpiryBundle::mapStrikes_iter_t ExpiryBundle::FindStrike( double strike ) {
  mapStrikes_iter_t iter = m_mapStrikes.find( strike );
  if ( m_mapStrikes.end() == iter ) {
    throw std::runtime_error( "Bundle::FindStrike: can't find strike" );
  }
  return iter;
}

ExpiryBundle::mapStrikes_iter_t ExpiryBundle::FindStrikeAuto( double strike ) {
  mapStrikes_iter_t iter = m_mapStrikes.find( strike );
  if ( m_mapStrikes.end() == iter ) {
    m_mapStrikes[ strike ] = Strike( strike );
    iter = m_mapStrikes.find( strike );
  }
  return iter;
}

// lower_bound: key value eq or gt than query
// upper_bound: key value ft than query
// 2013/09/09 doesn't appear to be called from anywhere
void ExpiryBundle::FindAdjacentStrikes( double dblValue, double& dblLower, double& dblUpper ) {

  mapStrikes_t::iterator iter = m_mapStrikes.lower_bound( dblValue ); 
  if ( m_mapStrikes.end() == iter ) {
    throw std::runtime_error( "Bundle::FindAdjacentStrikes: no upper strike available" );
  }
  dblUpper = iter->first;
  if ( dblValue == dblUpper ) {
    dblLower = dblUpper;
  }
  else {
    if ( m_mapStrikes.begin() == iter ) {
      throw std::runtime_error( "Bundle::FindAdjacentStrikes: already at lower lower end of strkes" );
    }
    --iter;
    dblLower = iter->first;
  }
}

void ExpiryBundle::RecalcATMWatch( double dblValue ) {
  // uses a 25% edge hysterisis level to force recalc of three containing options 
  //   ie when underlying is within 25% of upper strike or within 25% of lower strike
  // uses a 50% hysterisis level to select new set of three containing options
  //   ie underlying has to be within +/- 50% of mid strike to choose midstrike and corresponding upper/lower strikes
  mapStrikes_iter_t iterUpper;
  mapStrikes_iter_t iterLower;
  iterUpper = m_mapStrikes.lower_bound( dblValue ); 
  if ( m_mapStrikes.end() == iterUpper ) {
    std::cout << "Bundle::UpdateATMWatch: no upper strike available" << std::endl; // stay in no watch state
    m_stateOptionWatch = EOWSNoWatch;
  }
  else {
    iterLower = iterUpper;
    if ( m_mapStrikes.begin() == iterLower ) {
      std::cout << "Bundle::UpdateATMWatch: no lower strike available" << std::endl;  // stay in no watch state
      m_stateOptionWatch = EOWSNoWatch;
    }
    else {
      --iterLower;
      double dblMidPoint = ( iterUpper->first + iterLower->first ) * 0.5;
      if ( dblValue >= dblMidPoint ) { // third strike is above
        m_iterUpper = iterUpper;
        ++m_iterUpper;
        if ( m_mapStrikes.end() == m_iterUpper ) {
          std::cout << "Bundle::UpdateATMWatch: no upper upper strike available" << std::endl;  // stay in no watch state
          m_stateOptionWatch = EOWSNoWatch;
        }
        else {
          m_iterMid = iterUpper;
          m_iterLower = iterLower;
          m_stateOptionWatch = EOWSWatching;
        }
      }
      else { // third strike is below
        m_iterLower = iterLower;
        if ( m_mapStrikes.begin() == m_iterLower ) {
          std::cout << "Bundle::UpdateATMWatch: no lower lower strike available" << std::endl;  // stay in no watch state
          m_stateOptionWatch = EOWSNoWatch;
        }
        else {
          --m_iterLower;
          m_iterMid = iterLower;
          m_iterUpper = iterUpper;
          m_stateOptionWatch = EOWSWatching;
        }
      }
      if ( EOWSWatching == m_stateOptionWatch ) {
        m_dblUpperTrigger = m_iterUpper->first - ( m_iterUpper->first - m_iterMid->first ) * 0.25;
        m_dblLowerTrigger = m_iterLower->first + ( m_iterMid->first - m_iterLower->first ) * 0.25;
        std::cout << m_dblLowerTrigger << " < " << dblValue << " < " << m_dblUpperTrigger << std::endl;
      }
    }
  }
}

void ExpiryBundle::UpdateATMWatch( double dblValue ) {
  switch ( m_stateOptionWatch ) {
  case EOWSNoWatch:
    RecalcATMWatch( dblValue );
    if ( EOWSWatching == m_stateOptionWatch ) {
      SetWatchOn( m_iterUpper->first, true );
      SetWatchOn( m_iterMid->first, true );
      SetWatchOn( m_iterLower->first, true );
    }
    break;
  case EOWSWatching:
    if ( ( dblValue > m_dblUpperTrigger ) || ( dblValue < m_dblLowerTrigger ) ) {
      mapStrikes_iter_t iterUpper( m_iterUpper );
      mapStrikes_iter_t iterMid( m_iterMid );
      mapStrikes_iter_t iterLower( m_iterLower );
      RecalcATMWatch( dblValue );
      if ( EOWSWatching == m_stateOptionWatch ) { // by setting on before off allows continuity of capture
        SetWatchOn( m_iterUpper->first, true );
        SetWatchOn( m_iterMid->first, true );
        SetWatchOn( m_iterLower->first, true );
      }
      SetWatchOff( iterUpper->first );
      SetWatchOff( iterMid->first );
      SetWatchOff( iterLower->first );
    }
    break;
  }
}

void ExpiryBundle::SetExpiry( ptime dt ) {
  m_dtExpiry = dt;
}

void ExpiryBundle::CalcGreeksAtStrike( ptime now, mapStrikes_iter_t iter, ou::tf::option::binomial::structInput& input ) {

  // use the haskell book to get an estimator

  ou::tf::option::binomial::structOutput output;
  input.X = iter->first;

  // Manaster and Koehler Start Value, Option Pricing Formulas, pg 454
  double dblVolatilityGuess = std::sqrt( std::abs( std::log( input.S / input.X ) + input.r * input.T ) * 2.0 / input.T );
  input.v = dblVolatilityGuess;

//  std::cout << "Guess " << input.v << std::endl;

  if ( 0 != iter->second.Call() ) {
//    std::cout << "Call @" << input.X << ": ";
    try {
      input.optionSide = ou::tf::OptionSide::Call;
      ou::tf::option::binomial::CalcImpliedVolatility( input, iter->second.Call()->LastQuote().Midpoint(), output );
      ou::tf::Greek greek( now, output.iv, output.delta, output.gamma, output.theta, output.vega, output.rho );
      iter->second.Call()->AppendGreek( greek );
    }
    catch (...) {
//      std::cout << iter->second.Call()->GetInstrument()->GetInstrumentName() << ": IV Calc problem" << std::endl;
    }
  }
  if ( 0 != iter->second.Put() ) {
//    std::cout << "Put  @" << input.X << ": ";
    try {
      input.optionSide = ou::tf::OptionSide::Put;
      ou::tf::option::binomial::CalcImpliedVolatility( input, iter->second.Put()->LastQuote().Midpoint(), output );
      ou::tf::Greek greek( now, output.iv, output.delta, output.gamma, output.theta, output.vega, output.rho );
      iter->second.Put()->AppendGreek( greek );
    }
    catch (...) {
//      std::cout << iter->second.Put()->GetInstrument()->GetInstrumentName() << ": IV Calc problem" << std::endl;
    }
  }
}

void ExpiryBundle::CalcGreeks( double dblUnderlying, double dblVolHistorical, ptime now, ou::tf::LiborFromIQFeed& libor ) {

  assert( boost::posix_time::not_a_date_time != now );
  assert( boost::posix_time::not_a_date_time != m_dtExpiry );

  if ( EOWSNoWatch == m_stateOptionWatch ) return;  // not watching so no active data

  static time_duration tdurOneYear( 365 * 24, 0, 0 );  // should generalize to calc for current year (leap year, etc)
//    time_duration tdurOneYear( 360 * 24, 0, 0 );  // https://www.interactivebrokers.com/en/index.php?f=interest&p=schedule
//    time_duration tdurOneYear( 250 * 24, 0, 0 );  
  static long lSecForOneYear = tdurOneYear.total_seconds();
  long lSecToExpiry = ( m_dtExpiry - now ).total_seconds();
  double ratioToExpiry = (double) lSecToExpiry / (double) lSecForOneYear;
  double rate = libor.ValueAt( m_dtExpiry - now ) / 100.0;

//  ou::tf::option::binomial::structOutput output;
  ou::tf::option::binomial::structInput input;
  input.optionStyle = ou::tf::OptionStyle::American;
  input.S = dblUnderlying;
  input.T = ratioToExpiry;
  input.r = rate;
  input.b = rate; // is this correct?
  input.n = 91;  // binomial steps

  double dblVolatilityGuess = dblVolHistorical / 100.0;
  input.v = dblVolatilityGuess;

  CalcGreeksAtStrike( now, m_iterUpper, input );
  CalcGreeksAtStrike( now, m_iterMid, input );
  CalcGreeksAtStrike( now, m_iterLower, input );

  double dblIvCall( 0.0 );
  double dblIvPut( 0.0 );

  if ( dblUnderlying == m_iterMid->first ) {
    dblIvCall = m_iterMid->second.Call()->ImpliedVolatility();
    dblIvPut = m_iterMid->second.Put()->ImpliedVolatility();
  }
  else {
    if ( dblUnderlying > m_iterMid->first ) { // linear interpolation
      double ratio = ( dblUnderlying - m_iterMid->first ) / ( m_iterUpper->first - m_iterMid->first );

      double iv1, iv2;
      iv1 = m_iterMid->second.Call()->ImpliedVolatility();
      iv2 = m_iterUpper->second.Call()->ImpliedVolatility();
      dblIvCall = iv1 + ( iv2 - iv1 ) * ratio; 

      iv1 = m_iterMid->second.Put()->ImpliedVolatility();
      iv2 = m_iterUpper->second.Put()->ImpliedVolatility();
      dblIvPut = iv1 + ( iv2 - iv1 ) * ratio; 
    }
    else { // linear interpolation
      double ratio = ( dblUnderlying - m_iterLower->first ) / ( m_iterMid->first - m_iterLower->first );

      double iv1, iv2;
      iv1 = m_iterLower->second.Call()->ImpliedVolatility();
      iv2 = m_iterMid->second.Call()->ImpliedVolatility();
      dblIvCall = iv1 + ( iv2 - iv1 ) * ratio; 

      iv1 = m_iterLower->second.Put()->ImpliedVolatility();
      iv2 = m_iterMid->second.Put()->ImpliedVolatility();
      dblIvPut = iv1 + ( iv2 - iv1 ) * ratio; 
    }
  }
  PriceIV atmIV( now, dblUnderlying, m_dtExpiry, dblIvCall, dblIvPut);
  m_tsAtmIv.Append( atmIV );
  m_bfIVUnderlyingCall.Add( now, dblIvCall, 0 );
  m_bfIVUnderlyingPut.Add( now, dblIvPut, 0 );
  OnAtmIvCalc( atmIV );

//  std::cout << "AtmIV " << now << "" << m_dtExpiry << " " << dblUnderlying << "," << dblIvCall << "," << dblIvPut << std::endl;

}

std::ostream& operator<<( std::ostream& os, const ExpiryBundle& eb ) {
  os << eb.m_dtExpiry << ": #strikes=" << eb.m_mapStrikes.size();
  return os;
}

//=====================================

void ExpiryBundleWithUnderlying::SetUnderlying( pInstrument_t pInstrument, pProvider_t pProvider ) {

  // todo:  check if already something present, and if something present, should stop the watch, if running
  m_pwatchUnderlying.reset( new ou::tf::Watch( pInstrument, pProvider ) );
//  if ( m_bWatching ) {
    m_pwatchUnderlying->StartWatch();
//  }
}

void ExpiryBundleWithUnderlying::StartWatch( void ) {
//  if ( !m_bWatching ) {
//    m_bWatching = true;
    if ( 0 != m_pwatchUnderlying.get() ) m_pwatchUnderlying->StartWatch();
    ExpiryBundle::StartWatch();
//    for ( mapStrikes_t::iterator iter = m_mapStrikes.begin(); m_mapStrikes.end() != iter; ++iter ) {
//      iter->second.SetWatchOn();
//    }
//  }
}

void ExpiryBundleWithUnderlying::StopWatch( void ) {
//  if ( m_bWatching ) {
//    m_bWatching = false;
    if ( 0 != m_pwatchUnderlying.get() ) m_pwatchUnderlying->StopWatch();
    ExpiryBundle::StopWatch();
//    for ( mapStrikes_t::iterator iter = m_mapStrikes.begin(); m_mapStrikes.end() != iter; ++iter ) {
//      iter->second.SetWatchOff();
//    }
//  }
}

void ExpiryBundleWithUnderlying::EmitValues( void ) {
  if ( 0 != m_pwatchUnderlying.get() ) {
    m_pwatchUnderlying->EmitValues();
  }
  ExpiryBundle::EmitValues();
//  for ( mapStrikes_t::iterator iter = m_mapStrikes.begin(); m_mapStrikes.end() != iter; ++iter ) {
//    iter->second.EmitValues();
//  }
}

void ExpiryBundleWithUnderlying::SaveSeries( const std::string& sPrefix60sec, const std::string& sPrefix86400sec ) {
  if ( 0 != m_pwatchUnderlying.get() ) {
    m_pwatchUnderlying->SaveSeries( sPrefix60sec );
  }
  ExpiryBundle::SaveSeries( sPrefix60sec, sPrefix86400sec );
//  for ( mapStrikes_t::iterator iter = m_mapStrikes.begin(); m_mapStrikes.end() != iter; ++iter ) {
//    iter->second.SaveSeries( sPrefix60sec );
//  }
//  SaveAtmIv( sPrefix60sec, sPrefix86400sec );
}

//=====================================

MultiExpiryBundle::~MultiExpiryBundle( void ) {
  if ( 0 != m_pWatchUnderlying.get() ) {
    StopWatch();
    m_pWatchUnderlying->OnQuote.Remove( MakeDelegate( this, &MultiExpiryBundle::HandleUnderlyingQuote ) );
    m_pWatchUnderlying.reset();
  }
}

bool MultiExpiryBundle::ExpiryBundleExists( boost::gregorian::date date ) {
  mapExpiryBundles_t::iterator iter = m_mapExpiryBundles.find( date );
  return ( m_mapExpiryBundles.end() != iter );
}

ExpiryBundle& MultiExpiryBundle::GetExpiryBundle( boost::gregorian::date date ) {
  mapExpiryBundles_t::iterator iter = m_mapExpiryBundles.find( date );
  if ( m_mapExpiryBundles.end() == iter ) {
    throw std::runtime_error( "MultiExpiryBundle::GetExpiryBundle no expiry" );
  }
  return iter->second;
}

//ExpiryBundle& MultiExpiryBundle::CreateExpiryBundle( boost::gregorian::date date ) {
ExpiryBundle& MultiExpiryBundle::CreateExpiryBundle( boost::posix_time::ptime dt ) {
  std::pair<mapExpiryBundles_t::iterator, bool> pair
    = m_mapExpiryBundles.insert( mapExpiryBundles_t::value_type( dt.date(), ExpiryBundle() ) );
  if ( pair.second ) {
    pair.first->second.SetExpiry( dt );
    return pair.first->second;
  }
  else {
    throw std::runtime_error( "MultiExpiryBundle::CreateExpiryBundle expiry exists" );
  }
}

void MultiExpiryBundle::SetWatchUnderlying( pInstrument_t& pInstrument, pProvider_t& pProvider ) {
  if ( 0 != m_pWatchUnderlying.get() ) {
    //m_pWatchUnderlying->SetOnQuote( 0 );
    throw std::runtime_error( "MultiExpiryBundle::SetWatchUnderlying underlying already exists" );
  }
  m_pWatchUnderlying.reset( new ou::tf::Watch( pInstrument, pProvider ) );
  m_pWatchUnderlying->OnQuote.Add( MakeDelegate( this, &MultiExpiryBundle::HandleUnderlyingQuote ) );
}

void MultiExpiryBundle::HandleUnderlyingQuote( const ou::tf::Quote& quote ) {
  // on quote mid point change of underlying, re-calculate which options to watch
  for ( mapExpiryBundles_t::iterator iter = m_mapExpiryBundles.begin(); m_mapExpiryBundles.end() != iter; ++iter ) {
    iter->second.UpdateATMWatch( quote.Midpoint() );
  }
};

void MultiExpiryBundle::StartWatch( void ) {
  if ( !m_pWatchUnderlying->Watching() ) {
    m_pWatchUnderlying->StartWatch();
  }
  // don't start option watch as that is handled via HandleQuote
}

void MultiExpiryBundle::StopWatch( void ) {
  if ( m_pWatchUnderlying->Watching() ) {
    m_pWatchUnderlying->StopWatch();
  }
  // are there issues with quotes arriving after underlying turned off?
  for ( mapExpiryBundles_t::iterator iter = m_mapExpiryBundles.begin(); m_mapExpiryBundles.end() != iter; ++iter ) {
    iter->second.StopWatch();
  }
}

void MultiExpiryBundle::CalcIV( ptime dtNow /*utc*/, ou::tf::LiborFromIQFeed& libor ) {
  for ( mapExpiryBundles_t::iterator iter = m_mapExpiryBundles.begin(); m_mapExpiryBundles.end() != iter; ++iter ) {
    iter->second.CalcGreeks( 
      m_pWatchUnderlying->LastQuote().Midpoint(), 
      m_pWatchUnderlying->Fundamentals().dblHistoricalVolatility,
      dtNow, libor );
  }
}

void MultiExpiryBundle::SaveData( const std::string& sPrefixSession, const std::string& sPrefix86400sec ) {
  m_pWatchUnderlying->SaveSeries( sPrefixSession );
  for ( mapExpiryBundles_t::iterator iter = m_mapExpiryBundles.begin(); m_mapExpiryBundles.end() != iter; ++iter ) {
    iter->second.SaveSeries( sPrefixSession, sPrefix86400sec );
  }
}

void MultiExpiryBundle::AssignOption( pInstrument_t pInstrument, pProvider_t pDataProvider, pProvider_t pGreekProvider ) {
//      std::stringstream ss;
//      ss << (*iter).dateExpiry << "," << pInstrument->GetExpiry();
  ExpiryBundle& eb( GetExpiryBundle( pInstrument->GetExpiry() ) );
  switch ( pInstrument->GetOptionSide() ) {
  case ou::tf::OptionSide::Call:
    eb.SetCall( pInstrument, pDataProvider, pGreekProvider );
    break;
  case ou::tf::OptionSide::Put:
    eb.SetPut( pInstrument, pDataProvider, pGreekProvider );
    break;
  default: { 
    assert( 1 == 0 );
    }
  }
  // should check that at least one of the entries was used.
}

void MultiExpiryBundle::AddOnStrikeWatchOn( ExpiryBundle::OnStrikeWatch_t function ) {
  for ( mapExpiryBundles_t::iterator iter = m_mapExpiryBundles.begin(); m_mapExpiryBundles.end() != iter; ++iter ) {
    iter->second.OnStrikeWatchOn.Add( function );
  }
}

void MultiExpiryBundle::RemoveOnStrikeWatchOn(ExpiryBundle:: OnStrikeWatch_t function ) {
  for ( mapExpiryBundles_t::iterator iter = m_mapExpiryBundles.begin(); m_mapExpiryBundles.end() != iter; ++iter ) {
    iter->second.OnStrikeWatchOn.Remove( function );
  }
}

void MultiExpiryBundle::AddOnStrikeWatchOff( ExpiryBundle::OnStrikeWatch_t function ) {
  for ( mapExpiryBundles_t::iterator iter = m_mapExpiryBundles.begin(); m_mapExpiryBundles.end() != iter; ++iter ) {
    iter->second.OnStrikeWatchOff.Add( function );
  }
}

void MultiExpiryBundle::RemoveOnStrikeWatchOff(ExpiryBundle:: OnStrikeWatch_t function ) {
  for ( mapExpiryBundles_t::iterator iter = m_mapExpiryBundles.begin(); m_mapExpiryBundles.end() != iter; ++iter ) {
    iter->second.OnStrikeWatchOff.Remove( function );
  }
}

void MultiExpiryBundle::AddOnAtmIv( ExpiryBundle::OnAtmIvCalc_t function ) {
  for ( mapExpiryBundles_t::iterator iter = m_mapExpiryBundles.begin(); m_mapExpiryBundles.end() != iter; ++iter ) {
    iter->second.OnAtmIvCalc.Add( function );
  }
}

void MultiExpiryBundle::RemoveOnAtmIv( ExpiryBundle::OnAtmIvCalc_t function ) {
  for ( mapExpiryBundles_t::iterator iter = m_mapExpiryBundles.begin(); m_mapExpiryBundles.end() != iter; ++iter ) {
    iter->second.OnAtmIvCalc.Remove( function );
  }
}

std::ostream& operator<<( std::ostream& os, const MultiExpiryBundle& meb ) {
  for ( MultiExpiryBundle::mapExpiryBundles_t::const_iterator iter = meb.m_mapExpiryBundles.begin(); meb.m_mapExpiryBundles.end() != iter; ++iter ) {
    os << iter->second << std::endl;
  }
  return os;
}


} // namespace option
} // namespace tf
} // namespace ou

