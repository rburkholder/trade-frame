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

#include <TFHDF5TimeSeries/HDF5DataManager.h>
#include <TFHDF5TimeSeries/HDF5WriteTimeSeries.h>
#include <TFHDF5TimeSeries/HDF5IterateGroups.h>
#include <TFHDF5TimeSeries/HDF5Attribute.h>

#include "Option.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

Option::Option( pInstrument_t pInstrument, pProvider_t pDataProvider, pProvider_t pGreekProvider )
: Watch( pInstrument, pDataProvider ),
  m_pGreekProvider( pGreekProvider ),
  m_dblStrike( pInstrument->GetStrike() ),
  m_sSide( "-" )
{
  Initialize();
}

Option::Option( const Option& rhs ) :
  Watch( rhs ),
  m_dblStrike( rhs.m_dblStrike ),
  m_greek( rhs.m_greek ),
  m_sSide( rhs.m_sSide ),
  m_pGreekProvider( rhs.m_pGreekProvider )
{
  Initialize();
}

Option::~Option( void ) {
//  StopWatch();  // issues here
}

Option& Option::operator=( const Option& rhs ) {
  Watch::operator=( rhs );
  m_dblStrike = rhs.m_dblStrike;
  m_greek = rhs.m_greek;
  m_sSide = rhs.m_sSide;
  m_pGreekProvider = rhs.m_pGreekProvider;
  Initialize();
  return *this;
}

void Option::Initialize( void ) {
  if ( 0 != m_pGreekProvider.get() ) 
    assert( m_pGreekProvider->ProvidesGreeks() );
}

void Option::StartWatch( void ) {
  if ( 0 == m_cntWatching ) {
    Watch::StartWatch();
    if ( 0 != m_pGreekProvider.get() ) 
      m_pGreekProvider->AddGreekHandler( m_pInstrument, MakeDelegate( this, &Option::HandleGreek ) );
  }
}

bool Option::StopWatch( void ) {
  bool b = Watch::StopWatch();
  if ( b ) {
    if ( 0 != m_pGreekProvider.get() ) 
      m_pGreekProvider->RemoveGreekHandler( m_pInstrument, MakeDelegate( this, &Option::HandleGreek ) );
  }
  return b;
}

void Option::EmitValues( void ) {
  Watch::EmitValues();
  std::cout << m_pInstrument->GetInstrumentName() << ": " 
    << "IV:" << m_greek.ImpliedVolatility() << "," 
    << "D:" << m_greek.Delta() << "," 
    << "G:" << m_greek.Gamma() << "," 
    << "T:" << m_greek.Theta() << "," 
    << "V:" << m_greek.Vega() 
    << std::endl;
}

void Option::HandleGreek( const Greek& greek ) {
  m_greek = greek;
  m_greeks.Append( greek );
}

void Option::AppendGreek( const ou::tf::Greek& greek ) {
  HandleGreek( greek );
}

void Option::SaveSeries( const std::string& sPrefix ) {

  std::string sPathName;

  HDF5Attributes::structOption option( 
    m_dblStrike, m_pInstrument->GetExpiryYear(), m_pInstrument->GetExpiryMonth(), m_pInstrument->GetExpiryDay(), m_pInstrument->GetOptionSide() );

  Watch::SaveSeries( sPrefix );

  ou::tf::HDF5DataManager dm( ou::tf::HDF5DataManager::RDWR );

  if ( 0 != m_quotes.Size() ) {
    sPathName = sPrefix + "/quotes/" + m_pInstrument->GetInstrumentName();
    HDF5Attributes attrGreeks( dm, sPathName, option );
  }

  if ( 0 != m_trades.Size() ) {
    sPathName = sPrefix + "/trades/" + m_pInstrument->GetInstrumentName();
    HDF5Attributes attrGreeks( dm, sPathName, option );
  }

  if ( 0 != m_greeks.Size() ) {
    sPathName = sPrefix + "/greeks/" + m_pInstrument->GetInstrumentName();
    HDF5WriteTimeSeries<ou::tf::Greeks> wtsGreeks( dm, true, true, 5, 256 );
    wtsGreeks.Write( sPathName, &m_greeks );
    HDF5Attributes attrGreeks( dm, sPathName, option );
    attrGreeks.SetMultiplier( m_pInstrument->GetMultiplier() );
    attrGreeks.SetSignificantDigits( m_pInstrument->GetSignificantDigits() );
    if ( 0 != m_pGreekProvider.get() ) {
      attrGreeks.SetProviderType( m_pGreekProvider->ID() );
    }
    else {
      attrGreeks.SetProviderType( ou::tf::keytypes::EProviderCalc );
    }
  }

}


//
// ==================^
//

Call::Call( pInstrument_t pInstrument, pProvider_t pDataProvider, pProvider_t pGreekProvider )
: Option( pInstrument, pDataProvider,pGreekProvider )
{
  // assert instrument is a call
  assert( ou::tf::OptionSide::Call == pInstrument->GetOptionSide() );
  m_sSide = "C";
}

//
// ==================
//

Put::Put( pInstrument_t pInstrument, pProvider_t pDataProvider, pProvider_t pGreekProvider )
: Option( pInstrument, pDataProvider,pGreekProvider )
{
  // assert instrument is a put
  assert( ou::tf::OptionSide::Put == pInstrument->GetOptionSide() );
  m_sSide = "P";
}


//
// ==================
//

} // namespace option
} // namespace tf
} // namespace ou

