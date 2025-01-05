/************************************************************************
 * Copyright(c) 2012, One Unified. All rights reserved.                 *
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

#include <TFTrading/Instrument.h>

#include "MarketSymbol.h"
#include "ParseOptionSymbol.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed

MarketSymbol::MarketSymbol() {
}

MarketSymbol::~MarketSymbol() {
}

// deprecating?
const std::string MarketSymbol::BuildGenericName( const std::string& sBaseName, const TableRowDef& trd ) {
  std::string sName( sBaseName );
  switch( trd.sc ) {
    case ESecurityType::Equity:
      // uses base name
      break;
    case ESecurityType::IEOption:
      sName = ou::tf::Instrument::BuildGenericOptionName( sBaseName, trd.nYear, trd.nMonth, trd.nDay, trd.eOptionSide, trd.dblStrike );
      break;
    default:
      assert( false );
      break;
  }
  return sName;
}

// improved version
const std::string MarketSymbol::BuildGenericName( const std::string& sBaseName, const TableRowDef& trd, Date date ) {
  std::string sName( sBaseName );
  switch( trd.sc ) {
    case ESecurityType::Equity:
      // uses base name
      break;
    case ESecurityType::Future:
      sName = ou::tf::Instrument::BuildGenericFutureName( sBaseName, date.year(), date.month(), date.day() );
      break;
    case ESecurityType::IEOption:
    case ESecurityType::FOption:
      sName = ou::tf::Instrument::BuildGenericOptionName( sBaseName, date.year(), date.month(), date.day(), trd.eOptionSide, trd.dblStrike );
      break;
    default:
      assert( false );
      break;
  }
  return sName;
}

// improved improved version
const std::string MarketSymbol::BuildGenericName( const TableRowDef& trd, const Fundamentals& fundamentals ) {
  std::string sName( fundamentals.sExchangeRoot );
  switch( trd.sc ) {
    case ESecurityType::Equity:
      // uses base name
      break;
    case ESecurityType::Future:
      sName = ou::tf::Instrument::BuildGenericFutureName( sName, fundamentals.dateExpiration );
      break;
    case ESecurityType::IEOption:
    case ESecurityType::FOption:
      sName = ou::tf::Instrument::BuildGenericOptionName( sName, fundamentals.dateExpiration, trd.eOptionSide, fundamentals.dblStrikePrice );
      break;
    default:
      assert( false );
      break;
  }
  return sName;
}

// improved improved improved version
const std::string MarketSymbol::BuildGenericName( const Fundamentals& fundamentals ) {
  std::string sName;
  if ( 0 == fundamentals.sExchangeRoot.size() ) {
    sName = fundamentals.sSymbolName;
  }
  else {
    sName = fundamentals.sExchangeRoot;
  }
  switch( fundamentals.eSecurityType ) {
    case ESecurityType::Equity:
      // uses base name
      break;
    case ESecurityType::Future:
      sName = ou::tf::Instrument::BuildGenericFutureName( sName, fundamentals.dateExpiration );
      break;
    case ESecurityType::IEOption:
      sName = ou::tf::Instrument::BuildGenericOptionName( OptionBaseName( fundamentals ), fundamentals.dateExpiration, fundamentals.eOptionSide, fundamentals.dblStrikePrice );
      break;
    case ESecurityType::FOption:
      if ( sName != fundamentals.sExchangeRoot ) {
        std::cout << "MarketSymbol::BuildGenericName futures-option problem: " << sName << " vs " << fundamentals.sExchangeRoot << std::endl;
      }
      sName = ou::tf::Instrument::BuildGenericOptionName( sName, fundamentals.dateExpiration, fundamentals.eOptionSide, fundamentals.dblStrikePrice );
      break;
    case ESecurityType::MktStats:
      // uses base name
      break;
    case ESecurityType::Forex:
      // example: "USDCAD.FXCM"
      assert( 11 == fundamentals.sSymbolName.size() );
      assert( 16 == fundamentals.sCompanyName.size() );
      assert( "FXCM" == fundamentals.sExchange );
      sName = fundamentals.sSymbolName.substr( 0, 3 ) + '.' + fundamentals.sSymbolName.substr( 3, 3 );
      break;
    default:
      assert( false );
      break;
  }
  return sName;
}

const std::string MarketSymbol::OptionBaseName( const Fundamentals& fundamentals ) {
  const std::string& sName( fundamentals.sSymbolName );
  ou::tf::iqfeed::structParsedOptionSymbol1 parsed;
  ou::tf::iqfeed::OptionSymbolParser1<std::string::const_iterator> parserOptionSymbol1;
  bool bOk = parse( sName.cbegin(), sName.cend(), parserOptionSymbol1, parsed );
  assert( bOk );
  return parsed.sText;
}

} // namespace iqfeed
} // namespace tf
} // namespace ou