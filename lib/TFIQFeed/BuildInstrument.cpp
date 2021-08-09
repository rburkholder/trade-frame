/************************************************************************
 * Copyright(c) 2014, One Unified. All rights reserved.                 *
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

#include "BuildInstrument.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed

// 20151115, different naming may mess things up in callers, need to check
// 20151228, maybe use trd.sListedMarket instead of trd.sExchange (may help with some IB exchange translations)
pInstrument_t BuildInstrument( const std::string& sGenericName, const trd_t& trd, boost::uint16_t day ){
  pInstrument_t pInstrument;
  switch ( trd.sc ) {
    case MarketSymbol::enumSymbolClassifier::Equity:
      pInstrument.reset( new ou::tf::Instrument( sGenericName, ou::tf::InstrumentType::Stock, trd.sExchange ) );
      pInstrument->SetAlternateName( ou::tf::Instrument::eidProvider_t::EProviderIQF, trd.sSymbol );
      if ( "TSE" == trd.sExchange ) {
        pInstrument->SetCurrency( ou::tf::Currency::enumCurrency::CAD );
      }
      else {
        pInstrument->SetCurrency( ou::tf::Currency::enumCurrency::USD );  // by default, but some are alternate
      }
      pInstrument->SetMultiplier( 1 );  // default
      pInstrument->SetMinTick( 0.01 );
      pInstrument->SetSignificantDigits( 2 );  // not sure about this one
      break;
    case MarketSymbol::enumSymbolClassifier::IEOption:
      pInstrument.reset( new ou::tf::Instrument( sGenericName, ou::tf::InstrumentType::Option, trd.sExchange, trd.nYear, trd.nMonth, trd.nDay, trd.eOptionSide, trd.dblStrike ) );
      pInstrument->SetAlternateName( ou::tf::Instrument::eidProvider_t::EProviderIQF, trd.sSymbol );
      pInstrument->SetCurrency( ou::tf::Currency::enumCurrency::USD );  // by default, but some are alternate
      pInstrument->SetMultiplier( 100 );  // default, but there are ones with 10
      pInstrument->SetMinTick( 0.01 );
      pInstrument->SetSignificantDigits( 2 );  // not sure about this one
      break;
    case MarketSymbol::enumSymbolClassifier::Future:  // may need to pull out the prefix
      assert( ( 0 != day ) || ( 0 != trd.nDay ) );
      pInstrument.reset( new ou::tf::Instrument( sGenericName, ou::tf::InstrumentType::Future, trd.sExchange, trd.nYear, trd.nMonth, 0 != day ? day : trd.nDay ) );
      pInstrument->SetAlternateName( ou::tf::Instrument::eidProvider_t::EProviderIQF, trd.sSymbol );
      pInstrument->SetCurrency( ou::tf::Currency::enumCurrency::USD );  // by default, but some are alternate
      pInstrument->SetMultiplier( 100 );  // default
      pInstrument->SetMinTick( 0.05 );  // this may vary depending upon future type
      pInstrument->SetSignificantDigits( 2 );  // not sure about this one
      break;
    case MarketSymbol::enumSymbolClassifier::FOption:  // futures option doesn't require underlying?
      assert( ( 0 != day ) || ( 0 != trd.nDay ) );
      pInstrument.reset( new ou::tf::Instrument( sGenericName, ou::tf::InstrumentType::FuturesOption, trd.sExchange, trd.nYear, trd.nMonth, 0 != day ? day : trd.nDay, trd.eOptionSide, trd.dblStrike ) );
      pInstrument->SetAlternateName( ou::tf::Instrument::eidProvider_t::EProviderIQF, trd.sSymbol );
      pInstrument->SetCurrency( ou::tf::Currency::enumCurrency::USD );  // by default, but some are alternate
      pInstrument->SetMultiplier( 100 );  // varies
      pInstrument->SetMinTick( 0.01 );  // varies
      pInstrument->SetSignificantDigits( 2 );  // not sure about this one
      break;
    case MarketSymbol::enumSymbolClassifier::Index:
    case MarketSymbol::enumSymbolClassifier::PrecMtl:
    default:
      throw std::runtime_error( "BuildInstrument1: no applicable instrument type" );
  }
  return pInstrument;
}

} // namespace iqfeed
} // namespace TradeFrame
} // namespace ou

