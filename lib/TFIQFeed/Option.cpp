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

#include "StdAfx.h"

#include <boost/lexical_cast.hpp>

#include <TFTrading/KeyTypes.h>

#include "Option.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed

const char rchCallMonth[] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L' };
const char rchPutMonth[] =  { 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X' };

void ComposeOptionName( 
  std::string& sCall, std::string& sPut, 
  const std::string& sUnderlying, ou::tf::OptionSide::enumOptionSide option, ptime dtExpiry, double dblStrike ) {
  std::string sDay;
  boost::gregorian::date::day_type day = dtExpiry.date().day();
  sDay = ( ( 9 < day ) ? "" : "0" ) + boost::lexical_cast<std::string>( day );
  std::string sYear = boost::lexical_cast<std::string>( dtExpiry.date().year() );
  std::string sStrike = boost::lexical_cast<std::string>( dblStrike );
  sCall = sUnderlying + sYear.substr( 2, 2 ) + sDay + rchCallMonth[ dtExpiry.date().month() - 1 ] + sStrike;
  sPut  = sUnderlying + sYear.substr( 2, 2 ) + sDay + rchPutMonth[  dtExpiry.date().month() - 1 ] + sStrike;
}

void SetAlternateName( const pInstrument_t& pInstrument ) {
  assert( ou::tf::InstrumentType::Option == pInstrument->GetInstrumentType() );
  boost::gregorian::date dateExpiry = pInstrument->GetExpiry();
  boost::gregorian::date::day_type day = dateExpiry.day() + 1;  // IQFeed dates are the saturday after expiry
  char chMonth;
  switch ( pInstrument->GetOptionSide() ) {
  case ou::tf::OptionSide::Call: 
    chMonth = rchCallMonth[ dateExpiry.month() - 1 ];
    break;
  case ou::tf::OptionSide::Put:
    chMonth = rchPutMonth[ dateExpiry.month() - 1 ];
    break;
  default: throw std::runtime_error( "ou::tf::iqfeed::option::AddAlternateName: no option side specified" );
  }

  pInstrument->SetAlternateName( ou::tf::keytypes::EProviderIQF, 
      pInstrument->GetUnderlyingName() 
    + boost::lexical_cast<std::string>( dateExpiry.year() ).substr( 2, 2 ) 
    + ( ( 9 < day ) ? "" : "0" ) + boost::lexical_cast<std::string>( day )
    + chMonth
    + boost::lexical_cast<std::string>( pInstrument->GetStrike() ) );
}

} // namespace iqfeed
} // namespace TradeFrame
} // namespace ou

