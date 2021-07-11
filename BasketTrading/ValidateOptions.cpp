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
 * File:    ValidateOptions.cpp
 * Author:  raymond@burkholder.net
 * Project: BasketTrading
 * Created on July 4, 2019, 12:10 PM
 */

#include "ValidateOptions.h"

ValidateOptions::ValidateOptions(
  pWatch_t pWatchUnderlying,  // underlying
  const mapChains_t& mapChains,
  fConstructOption_t& fConstructOption )
: m_state( EState::WaitForSize ),
  m_pWatchUnderlying( pWatchUnderlying ),
  m_mapChains( mapChains ),
  m_fConstructOption( fConstructOption )
{
  assert( nullptr != fConstructOption );
}

ValidateOptions::ValidateOptions( const ValidateOptions& rhs )
: m_state( rhs.m_state ),
  m_pWatchUnderlying( rhs.m_pWatchUnderlying ),
  m_mapChains( rhs.m_mapChains ),
  m_fConstructOption( rhs.m_fConstructOption ),
  m_SpreadValidation( rhs.m_SpreadValidation )
{
}

ValidateOptions::ValidateOptions( const ValidateOptions&& rhs )
: m_state( rhs.m_state ),
  m_pWatchUnderlying( std::move( rhs.m_pWatchUnderlying ) ),
  m_mapChains( rhs.m_mapChains ),
  m_fConstructOption( rhs.m_fConstructOption ),
  m_SpreadValidation( std::move( rhs.m_SpreadValidation ) )
{
}

ValidateOptions::~ValidateOptions( ) {
  m_SpreadValidation.ClearCandidates();
}

void ValidateOptions::SetSize( vLegSelected_t::size_type size ) {
  m_vLegSelected.resize( size );
  m_SpreadValidation.SetLegCount( size );
  m_state = EState::FirstTime;
}

bool ValidateOptions::ValidateBidAsk(
  boost::gregorian::date dateToday, double priceUnderlying, size_t nDuration, fChooseLegs_t&& fChooseLegs
) {

  bool bValidated( false );
  bool bRecirculate;
  do {
    bRecirculate = false;
    switch ( m_state ) {
      case EState::WaitForSize:
        // state changes in SetSize()
        break;
      case EState::FirstTime:
      case EState::FindStrikes:  // always run each time through to detect change in strikes as underlying moves
        {
          size_t nChanged {};
          size_t ixLeg {};
          try {
            fChooseLegs(
              m_mapChains, dateToday, priceUnderlying,
              [this,&ixLeg,&nChanged](double spread, double strike, boost::gregorian::date dateStrike, const std::string& sIQFeedOptionName){
                nChanged += m_vLegSelected.at( ixLeg ).Update( spread, strike, dateStrike, sIQFeedOptionName );
                ixLeg++;
              } );
            if ( 0 == nChanged ) {
              m_state = EState::Validate; // can set as no exception was thrown
            }
            else {
              m_state = EState::BuildOptions; // can set as no exception was thrown
            }
            bRecirculate = true;
          }
          catch ( const ou::tf::option::exception_strike_range_exceeded& e ) {
            // don't worry about this, price is not with in range yet
            throw e;
          }
          catch ( const std::runtime_error& e ) {
            const std::string& sUnderlying( m_pWatchUnderlying->GetInstrument()->GetInstrumentName() );
            std::cout
              << sUnderlying
              << " found no strike for"
              << " underlying=" << priceUnderlying
              << ", ixLeg=" << ixLeg
              << ", today=" << dateToday
              << " [" << e.what() << "]"
              << std::endl;
            throw e;
          }
        }
        break;
      case EState::BuildOptions:
        {
          pInstrument_t pInstrumentUnderlying = m_pWatchUnderlying->GetInstrument();
          const std::string& sUnderlying( pInstrumentUnderlying->GetInstrumentName() );
          std::cout
            << "BuildOptions for "
            << sUnderlying
            << "("
            << m_vLegSelected.size()
            << ")"
            << ": combo -> price=" << priceUnderlying;

          size_t ix {};
          m_state = EState::WaitForBuildCompletion;
          for ( vLegSelected_t::value_type& leg: m_vLegSelected ) {
            std::cout << ",strike=" << leg.Strike() << "@" << leg.Expiry();
            if ( leg.Changed() ) {
              std::cout << '+';
              m_SpreadValidation.ClearWatch( ix );  // IsActive becomes false
              m_fConstructOption(
                leg.IQFeedOptionName(), pInstrumentUnderlying,
                [this,&leg,ix]( pOption_t pOption ) {
                  leg.Option() = pOption;
                  m_SpreadValidation.SetWatch( ix, pOption, leg.Spread() );
                }
                );
            }
            ix++;
          }
          std::cout << std::endl;
          m_state = EState::WaitForBuildCompletion;
        }
        break;
      case EState::WaitForBuildCompletion:
        if ( m_SpreadValidation.IsActive() ) {
          m_state = EState::Validate;
          bRecirculate = true;
        }
        break;
      case EState::Validate:
        bValidated = m_SpreadValidation.Validate( nDuration );
        m_state = EState::FindStrikes;
        break;
    }
  } while( bRecirculate );

  return bValidated;
}

void ValidateOptions::WhenValidated( fValidatedOption_t&& fValidatedOption ) {
  assert( m_SpreadValidation.IsActive() );
  size_t ix {};
  for ( vLegSelected_t::value_type& vt: m_vLegSelected ) {
    fValidatedOption( ix, vt.Option() );
    ix++;
  }
}

void ValidateOptions::ClearValidation() {
  m_SpreadValidation.ClearCandidates();
  for ( vLegSelected_t::value_type& leg: m_vLegSelected ) {
    leg.Clear();
  }
}

