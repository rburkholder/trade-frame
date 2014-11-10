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

#include "StdAfx.h"

#include "BuildInstrument.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed

pInstrument_t BuildInstrument( const trd_t& trd ){
  pInstrument_t pInstrument;
  switch ( trd.sc ) {
  case MarketSymbol::enumSymbolClassifier::Equity:
    pInstrument.reset( ( new ou::tf::Instrument( trd.sSymbol, ou::tf::InstrumentType::Stock, trd.sExchange ) ) );
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
  case MarketSymbol::enumSymbolClassifier::Future:  // may need to pull out the prefix
    pInstrument.reset( ( new ou::tf::Instrument( trd.sUnderlying, ou::tf::InstrumentType::Future, trd.sExchange, trd.nYear, trd.nMonth ) ) );
    pInstrument->SetAlternateName( ou::tf::Instrument::eidProvider_t::EProviderIQF, trd.sSymbol );
    pInstrument->SetCurrency( ou::tf::Currency::enumCurrency::USD );  // by default, but some are alternate
    pInstrument->SetMultiplier( 100 );  // default
    pInstrument->SetMinTick( 0.05 );
    pInstrument->SetSignificantDigits( 2 );  // not sure about this one
    break;
  case MarketSymbol::enumSymbolClassifier::Index:
  case MarketSymbol::enumSymbolClassifier::PrecMtl:
  default:
    throw std::runtime_error( "BuildInstrument1: no applicable instrument type" );
  }
  return pInstrument;
}

pInstrument_t BuildInstrument( const trd_t& trd, pInstrument_t pUnderlying ){
  pInstrument_t pInstrument;
  switch ( trd.sc ) {
  case MarketSymbol::enumSymbolClassifier::IEOption:
    pInstrument.reset( ( new ou::tf::Instrument( trd.sUnderlying, ou::tf::InstrumentType::Option, trd.sExchange, trd.nYear, trd.nMonth, trd.nDay, pUnderlying, trd.eOptionSide, trd.dblStrike ) ) );
    pInstrument->SetAlternateName( ou::tf::Instrument::eidProvider_t::EProviderIQF, trd.sSymbol );
    pInstrument->SetCurrency( ou::tf::Currency::enumCurrency::USD );  // by default, but some are alternate
    pInstrument->SetMultiplier( 100 );  // default, but there are ones with 10
    pInstrument->SetMinTick( 0.01 );
    pInstrument->SetSignificantDigits( 2 );  // not sure about this one
    break;
  case MarketSymbol::enumSymbolClassifier::FOption:
    pInstrument.reset( ( new ou::tf::Instrument( trd.sUnderlying, ou::tf::InstrumentType::FuturesOption, trd.sExchange ) ) );
    pInstrument->SetAlternateName( ou::tf::Instrument::eidProvider_t::EProviderIQF, trd.sSymbol );
    pInstrument->SetCurrency( ou::tf::Currency::enumCurrency::USD );  // by default, but some are alternate
    pInstrument->SetMultiplier( 100 );  // varies
    pInstrument->SetMinTick( 0.01 );  // varies
    pInstrument->SetSignificantDigits( 2 );  // not sure about this one
  case MarketSymbol::enumSymbolClassifier::Forex:
  default:
    throw std::runtime_error( "BuildInstrument2: no applicable instrument type" );
  }
  return pInstrument;
}


} // namespace iqfeed
} // namespace TradeFrame
} // namespace ou

