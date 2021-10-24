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

// 20151228, maybe use trd.sListedMarket instead of trd.sExchange (may help with some IB exchange translations)
pInstrument_t BuildInstrument( const std::string& sGenericName, const trd_t& trd ){
  pInstrument_t pInstrument;
  switch ( trd.sc ) {
    case MarketSymbol::enumSymbolClassifier::Equity:
      pInstrument = BuildInstrument( sGenericName, trd, date_t( 1400, 1, 1 ) );
      break;
    case MarketSymbol::enumSymbolClassifier::IEOption:
      pInstrument = BuildInstrument( sGenericName, trd, date_t( trd.nYear, trd.nMonth, trd.nDay ) );
      break;
    case MarketSymbol::enumSymbolClassifier::Future:  // may need to pull out the prefix
      pInstrument = BuildInstrument( sGenericName, trd, date_t( trd.nYear, trd.nMonth, 1 ) );
      break;
    case MarketSymbol::enumSymbolClassifier::FOption:
      pInstrument = BuildInstrument( sGenericName, trd, date_t( trd.nYear, trd.nMonth, 1 ) );
      break;
    case MarketSymbol::enumSymbolClassifier::Index:
    case MarketSymbol::enumSymbolClassifier::PrecMtl:
    default:
      throw std::runtime_error( "BuildInstrument1: no applicable instrument type" );
  }
  return pInstrument;
}

// 2021/09/05 could use market from the fundamental field
pInstrument_t BuildInstrument( const std::string& sGenericName, const trd_t& trd, date_t date ) {
  pInstrument_t pInstrument;
  switch ( trd.sc ) {
    case MarketSymbol::enumSymbolClassifier::Equity:
      pInstrument = std::make_shared<Instrument>( sGenericName, ou::tf::InstrumentType::Stock, trd.sExchange );
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
      pInstrument = std::make_shared<Instrument>( sGenericName, ou::tf::InstrumentType::Option, trd.sExchange, date.year(), date.month(), date.day(), trd.eOptionSide, trd.dblStrike );
      pInstrument->SetAlternateName( ou::tf::Instrument::eidProvider_t::EProviderIQF, trd.sSymbol );
      pInstrument->SetCurrency( ou::tf::Currency::enumCurrency::USD );  // by default, but some are alternate
      pInstrument->SetMultiplier( 100 );  // default, but there are ones with 10
      pInstrument->SetMinTick( 0.01 );
      pInstrument->SetSignificantDigits( 2 );  // not sure about this one
      break;
    case MarketSymbol::enumSymbolClassifier::Future:  // may need to pull out the prefix
      pInstrument = std::make_shared<Instrument>( sGenericName, ou::tf::InstrumentType::Future, trd.sExchange, date.year(), date.month(), date.day() );
      pInstrument->SetAlternateName( ou::tf::Instrument::eidProvider_t::EProviderIQF, trd.sSymbol );
      pInstrument->SetCurrency( ou::tf::Currency::enumCurrency::USD );  // by default, but some are alternate
      pInstrument->SetMultiplier( 100 );  // default
      pInstrument->SetMinTick( 0.05 );  // this may vary depending upon future type
      pInstrument->SetSignificantDigits( 2 );  // not sure about this one
      break;
    case MarketSymbol::enumSymbolClassifier::FOption:  // futures option doesn't require underlying?
      pInstrument = std::make_shared<Instrument>( sGenericName, ou::tf::InstrumentType::FuturesOption, trd.sExchange, date.year(), date.month(), date.day(), trd.eOptionSide, trd.dblStrike );
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

pInstrument_t BuildInstrument( const trd_t& trd, const Fundamentals& fundamentals ) {

  const std::string sGenericName
    = ou::tf::iqfeed::MarketSymbol::BuildGenericName( fundamentals.sExchangeRoot, trd, fundamentals );

  pInstrument_t pInstrument;

  switch ( trd.sc ) {
    case MarketSymbol::enumSymbolClassifier::Equity:
      pInstrument = std::make_shared<Instrument>( sGenericName, ou::tf::InstrumentType::Stock, trd.sExchange );
      if ( "TSE" == trd.sExchange ) {
        pInstrument->SetCurrency( ou::tf::Currency::enumCurrency::CAD );
      }
      else {
        pInstrument->SetCurrency( ou::tf::Currency::enumCurrency::USD );  // by default, but some are alternate
      }
      break;
    case MarketSymbol::enumSymbolClassifier::IEOption: {
      auto date( fundamentals.dateExpiration );
      pInstrument = std::make_shared<Instrument>( sGenericName, ou::tf::InstrumentType::Option, trd.sExchange, date.year(), date.month().as_number(), date.day().as_number(), trd.eOptionSide, fundamentals.dblStrikePrice );
      pInstrument->SetCurrency( ou::tf::Currency::enumCurrency::USD );  // by default, but some are alternate
      }
      break;
    case MarketSymbol::enumSymbolClassifier::Future: { // may need to pull out the prefix
      auto date( fundamentals.dateExpiration );
      pInstrument = std::make_shared<Instrument>( sGenericName, ou::tf::InstrumentType::Future, trd.sExchange, date.year(), date.month().as_number(), date.day().as_number() );
      pInstrument->SetCurrency( ou::tf::Currency::enumCurrency::USD );  // by default, but some are alternate
      }
      break;
    case MarketSymbol::enumSymbolClassifier::FOption: { // futures option doesn't require underlying?
      auto date( fundamentals.dateExpiration );
      pInstrument = std::make_shared<Instrument>( sGenericName, ou::tf::InstrumentType::FuturesOption, trd.sExchange, date.year(), date.month().as_number(), date.day().as_number(), trd.eOptionSide, fundamentals.dblStrikePrice );
      pInstrument->SetCurrency( ou::tf::Currency::enumCurrency::USD );  // by default, but some are alternate
      }
      break;
    case MarketSymbol::enumSymbolClassifier::Index:
    case MarketSymbol::enumSymbolClassifier::PrecMtl:
    default:
      throw std::runtime_error( "BuildInstrument1: no applicable instrument type" );
  }

  pInstrument->SetAlternateName( ou::tf::Instrument::eidProvider_t::EProviderIQF, trd.sSymbol );
  pInstrument->SetMultiplier( fundamentals.nContractSize );
  pInstrument->SetMinTick( fundamentals.dblTickSize );
  pInstrument->SetSignificantDigits( fundamentals.nPrecision );

  return pInstrument;
}

} // namespace iqfeed
} // namespace TradeFrame
} // namespace ou

