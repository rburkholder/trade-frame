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

#include <OUCommon/TimeSource.h>

#include "BuildInstrument.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed

// 20151228, maybe use trd.sListedMarket instead of trd.sExchange (may help with some IB exchange translations)
pInstrument_t BuildInstrument( const std::string& sGenericName, const trd_t& trd ){
  pInstrument_t pInstrument;
  switch ( trd.sc ) {
    case ESecurityType::Equity:
      pInstrument = BuildInstrument( sGenericName, trd, date_t( 1400, 1, 1 ) );
      break;
    case ESecurityType::IEOption:
      pInstrument = BuildInstrument( sGenericName, trd, date_t( trd.nYear, trd.nMonth, trd.nDay ) );
      break;
    case ESecurityType::Future:  // may need to pull out the prefix
      pInstrument = BuildInstrument( sGenericName, trd, date_t( trd.nYear, trd.nMonth, 1 ) );
      break;
    case ESecurityType::FOption:
      pInstrument = BuildInstrument( sGenericName, trd, date_t( trd.nYear, trd.nMonth, 1 ) );
      break;
    case ESecurityType::Index:
    case ESecurityType::PrecMtl:
    default:
      throw std::runtime_error( "BuildInstrument: no applicable instrument type" );
  }
  return pInstrument;
}

// 2021/09/05 could use market from the fundamental field
pInstrument_t BuildInstrument( const std::string& sGenericName, const trd_t& trd, date_t date ) {
  pInstrument_t pInstrument;
  switch ( trd.sc ) {
    case ESecurityType::Equity:
      pInstrument = std::make_shared<Instrument>( sGenericName, ou::tf::InstrumentType::Stock, trd.sExchange );
      pInstrument->SetAlternateName( ou::tf::Instrument::eidProvider_t::EProviderIQF, trd.sSymbol );
      if ( "TSE" == trd.sExchange ) {
        pInstrument->SetCurrency( ou::tf::Currency::ECurrency::CAD );
      }
      else {
        pInstrument->SetCurrency( ou::tf::Currency::ECurrency::USD );  // by default, but some are alternate
      }
      pInstrument->SetMultiplier( 1 );  // default
      pInstrument->SetMinTick( 0.01 );
      pInstrument->SetSignificantDigits( 2 );  // not sure about this one
      break;
    case ESecurityType::IEOption:
      pInstrument = std::make_shared<Instrument>( sGenericName, ou::tf::InstrumentType::Option, trd.sExchange, date.year(), date.month(), date.day(), trd.eOptionSide, trd.dblStrike );
      pInstrument->SetAlternateName( ou::tf::Instrument::eidProvider_t::EProviderIQF, trd.sSymbol );
      pInstrument->SetCurrency( ou::tf::Currency::ECurrency::USD );  // by default, but some are alternate
      pInstrument->SetMultiplier( 100 );  // default, but there are ones with 10
      pInstrument->SetMinTick( 0.01 );
      pInstrument->SetSignificantDigits( 2 );  // not sure about this one
      break;
    case ESecurityType::Future:  // may need to pull out the prefix
      pInstrument = std::make_shared<Instrument>( sGenericName, ou::tf::InstrumentType::Future, trd.sExchange, date.year(), date.month(), date.day() );
      pInstrument->SetAlternateName( ou::tf::Instrument::eidProvider_t::EProviderIQF, trd.sSymbol );
      pInstrument->SetCurrency( ou::tf::Currency::ECurrency::USD );  // by default, but some are alternate
      pInstrument->SetMultiplier( 100 );  // default
      pInstrument->SetMinTick( 0.05 );  // this may vary depending upon future type
      pInstrument->SetSignificantDigits( 2 );  // not sure about this one
      break;
    case ESecurityType::FOption:  // futures option doesn't require underlying?
      pInstrument = std::make_shared<Instrument>( sGenericName, ou::tf::InstrumentType::FuturesOption, trd.sExchange, date.year(), date.month(), date.day(), trd.eOptionSide, trd.dblStrike );
      pInstrument->SetAlternateName( ou::tf::Instrument::eidProvider_t::EProviderIQF, trd.sSymbol );
      pInstrument->SetCurrency( ou::tf::Currency::ECurrency::USD );  // by default, but some are alternate
      pInstrument->SetMultiplier( 100 );  // varies
      pInstrument->SetMinTick( 0.01 );  // varies
      pInstrument->SetSignificantDigits( 2 );  // not sure about this one
      break;
    case ESecurityType::Index:
    case ESecurityType::PrecMtl:
    default:
      throw std::runtime_error( "BuildInstrument: no applicable instrument type" );
  }
  return pInstrument;
}

pInstrument_t BuildInstrument( const trd_t& trd, const Fundamentals& fundamentals ) {

  const std::string sGenericName
    = ou::tf::iqfeed::MarketSymbol::BuildGenericName( trd, fundamentals );

  pInstrument_t pInstrument;

  switch ( trd.sc ) {
    case ESecurityType::Equity:
      pInstrument = std::make_shared<Instrument>( sGenericName, ou::tf::InstrumentType::Stock, trd.sExchange );
      if ( "TSE" == trd.sExchange ) {
        pInstrument->SetCurrency( ou::tf::Currency::ECurrency::CAD );
      }
      else {
        pInstrument->SetCurrency( ou::tf::Currency::ECurrency::USD );  // by default, but some are alternate
      }
      break;
    case ESecurityType::IEOption: {
      auto date( fundamentals.dateExpiration );
      pInstrument = std::make_shared<Instrument>( sGenericName, ou::tf::InstrumentType::Option, trd.sExchange, date.year(), date.month().as_number(), date.day().as_number(), trd.eOptionSide, fundamentals.dblStrikePrice );
      pInstrument->SetCurrency( ou::tf::Currency::ECurrency::USD );  // by default, but some are alternate
      }
      break;
    case ESecurityType::Future: { // may need to pull out the prefix
      auto date( fundamentals.dateExpiration );
      pInstrument = std::make_shared<Instrument>( sGenericName, ou::tf::InstrumentType::Future, trd.sExchange, date.year(), date.month().as_number(), date.day().as_number() );
      pInstrument->SetCurrency( ou::tf::Currency::ECurrency::USD );  // by default, but some are alternate
      }
      break;
    case ESecurityType::FOption: { // futures option doesn't require underlying?
      auto date( fundamentals.dateExpiration );
      pInstrument = std::make_shared<Instrument>( sGenericName, ou::tf::InstrumentType::FuturesOption, trd.sExchange, date.year(), date.month().as_number(), date.day().as_number(), trd.eOptionSide, fundamentals.dblStrikePrice );
      pInstrument->SetCurrency( ou::tf::Currency::ECurrency::USD );  // by default, but some are alternate
      }
      break;
    case ESecurityType::Index:
    case ESecurityType::PrecMtl:
    default:
      throw std::runtime_error( "BuildInstrument: no applicable instrument type" );
  }

  pInstrument->SetAlternateName( ou::tf::Instrument::eidProvider_t::EProviderIQF, trd.sSymbol );
  pInstrument->SetMultiplier( fundamentals.nContractSize );
  pInstrument->SetMinTick( fundamentals.dblTickSize );
  pInstrument->SetSignificantDigits( fundamentals.nPrecision );

  return pInstrument;
}

pInstrument_t BuildInstrument( const Fundamentals& fundamentals ) {

  const std::string sGenericName
    = ou::tf::iqfeed::MarketSymbol::BuildGenericName( fundamentals );

  pInstrument_t pInstrument;

  const std::string& sExchange( fundamentals.sExchange );

  switch ( fundamentals.eSecurityType ) {
    case ESecurityType::Equity:
      pInstrument = std::make_shared<Instrument>( sGenericName, ou::tf::InstrumentType::Stock, sExchange );
      if ( "TSE" == sExchange ) {
        pInstrument->SetCurrency( ou::tf::Currency::ECurrency::CAD );
      }
      else {
        pInstrument->SetCurrency( ou::tf::Currency::ECurrency::USD );  // by default, but some are alternate
      }
      break;
    case ESecurityType::IEOption: {
      auto date( fundamentals.dateExpiration );
      pInstrument = std::make_shared<Instrument>( sGenericName, ou::tf::InstrumentType::Option, sExchange, date.year(), date.month().as_number(), date.day().as_number(), fundamentals.eOptionSide, fundamentals.dblStrikePrice );
      pInstrument->SetCurrency( ou::tf::Currency::ECurrency::USD );  // by default, but some are alternate
      }
      break;
    case ESecurityType::Future: {
      auto date( fundamentals.dateExpiration );
      pInstrument = std::make_shared<Instrument>( sGenericName, ou::tf::InstrumentType::Future, sExchange, date.year(), date.month().as_number(), date.day().as_number() );
      pInstrument->SetCurrency( ou::tf::Currency::ECurrency::USD );  // by default, but some are alternate
      }
      break;
    case ESecurityType::FOption: {
      //boost::posix_time::ptime dt( fundamentals.dateExpiration, fundamentals.timeSessionClose );
      // timeSessionClose isn't actually expiry, but gets us closer
      boost::posix_time::ptime dt = ou::TimeSource::ConvertRegionalToUtc( fundamentals.dateExpiration, fundamentals.timeSessionClose, "America/New_York" );
      //std::string s( boost::lexical_cast<std::string>(dt) );
      //std::string t( boost::lexical_cast<std::string>(fundamentals.timeSessionClose) ); "17:00:00"
      pInstrument = std::make_shared<Instrument>( sGenericName, ou::tf::InstrumentType::FuturesOption, sExchange, dt, fundamentals.eOptionSide, fundamentals.dblStrikePrice );
      pInstrument->SetCurrency( ou::tf::Currency::ECurrency::USD );  // by default, but some are alternate
      }
      break;
    case ESecurityType::Forex:
      {
        const ou::tf::Currency::pair_t pairCurrency( ou::tf::Currency::Split( sGenericName ) ); // XXX.YYY
        pInstrument
          = std::make_shared<ou::tf::Instrument>(
              sGenericName,
              ou::tf::InstrumentType::Currency, sExchange,
              pairCurrency.first, pairCurrency.second
              );
      }
      break;
    case ESecurityType::Index:
    case ESecurityType::MktStats:
      pInstrument = std::make_shared<Instrument>( sGenericName, ou::tf::InstrumentType::Index, sExchange );
      pInstrument->SetCurrency( ou::tf::Currency::ECurrency::USD );  // by default, but some are alternate
      break;
    case ESecurityType::PrecMtl:
    default:
      throw std::runtime_error( "BuildInstrument: no applicable instrument type" );
      break;
  }

  pInstrument->SetAlternateName( ou::tf::Instrument::eidProvider_t::EProviderIQF, fundamentals.sSymbolName );
  if ( 0 != fundamentals.nContractSize ) pInstrument->SetMultiplier( fundamentals.nContractSize );
  if ( 0 != fundamentals.dblTickSize ) pInstrument->SetMinTick( fundamentals.dblTickSize );
  if ( 0 != fundamentals.nPrecision ) pInstrument->SetSignificantDigits( fundamentals.nPrecision );

  return pInstrument;
}

} // namespace iqfeed
} // namespace TradeFrame
} // namespace ou

