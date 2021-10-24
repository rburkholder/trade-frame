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

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed

MarketSymbol::MarketSymbol(void) {
}

MarketSymbol::~MarketSymbol(void) {
}

// deprecating?
const std::string MarketSymbol::BuildGenericName( const std::string& sBaseName, const TableRowDef& trd ) {
  std::string sName( sBaseName );
  switch( trd.sc ) {
    case enumSymbolClassifier::Equity:
      // uses base name
      break;
    case enumSymbolClassifier::IEOption:
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
    case enumSymbolClassifier::Equity:
      // uses base name
      break;
    case enumSymbolClassifier::Future:
      sName = ou::tf::Instrument::BuildGenericFutureName( sBaseName, date.year(), date.month(), date.day() );
      break;
    case enumSymbolClassifier::IEOption:
    case enumSymbolClassifier::FOption:
      sName = ou::tf::Instrument::BuildGenericOptionName( sBaseName, date.year(), date.month(), date.day(), trd.eOptionSide, trd.dblStrike );
      break;
    default:
      assert( false );
      break;
  }
  return sName;
}

// improved improved version
const std::string MarketSymbol::BuildGenericName( const std::string& sBaseName, const TableRowDef& trd, const Fundamentals& fundamentals ) {
  std::string sName( sBaseName );
  switch( trd.sc ) {
    case enumSymbolClassifier::Equity:
      // uses base name
      break;
    case enumSymbolClassifier::Future:
      sName = ou::tf::Instrument::BuildGenericFutureName( sBaseName, fundamentals.dateExpiration );
      break;
    case enumSymbolClassifier::IEOption:
    case enumSymbolClassifier::FOption:
      sName = ou::tf::Instrument::BuildGenericOptionName( sBaseName, fundamentals.dateExpiration, trd.eOptionSide, fundamentals.dblStrikePrice );
      break;
    default:
      assert( false );
      break;
  }
  return sName;
}

} // namespace iqfeed
} // namespace tf
} // namespace ou