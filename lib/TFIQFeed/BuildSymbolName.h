/************************************************************************
 * Copyright(c) 2015, One Unified. All rights reserved.                 *
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

// started 2015/11/15

#pragma once

#include <cstdint>
#include <string>
#include <cassert>

#include <TFTrading/TradingEnumerations.h>

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed

struct NameParts {
  InstrumentType::EInstrumentType it;
  const std::string& sRootName;
  uint16_t year;
  uint16_t month;
  uint16_t day;
  double strike;
  ou::tf::OptionSide::enumOptionSide side;
  NameParts(InstrumentType::EInstrumentType it_, const std::string& sRootName_)
    : it( it_ ), sRootName( sRootName_), year(0), month(0), day(0), strike(0.0),
        side( ou::tf::OptionSide::Unknown) {}
  NameParts(InstrumentType::EInstrumentType it_, const std::string& sRootName_,
    uint16_t year_, uint16_t month_, uint16_t day_,
    double strike_, ou::tf::OptionSide::enumOptionSide side_ )
      : it( it_ ), sRootName( sRootName_ ), year( year_ ), month( month_ ), day( day_ ),
          strike( strike_ ), side( side_ ) 
             { assert( 0 < month ); assert( 12 >= month ); }
  NameParts(InstrumentType::EInstrumentType it_, const std::string& sRootName_,
    uint16_t year_, uint16_t month_, uint16_t day_ )
      : it( it_ ), sRootName( sRootName_ ), year( year_ ), month( month_ ), day( day_ ), 
          strike(0.0), side( ou::tf::OptionSide::Unknown) 
             { assert( 0 < month ); assert( 12 >= month ); }
};
  
// months are 1 .. 12
const std::string BuildName( const NameParts& parts );
const std::string BuildOptionName( const std::string& sUnderlying, uint16_t year, uint16_t month, uint16_t day, double strike, ou::tf::OptionSide::enumOptionSide side );
const std::string BuildFuturesName( const std::string& sUnderlying, uint16_t year, uint16_t month );
const std::string BuildFuturesOptionName( const std::string& sUnderlying, uint16_t year, uint16_t month, double strike, ou::tf::OptionSide::enumOptionSide side );
const std::string BuildFuturesOptionName( const std::string& sUnderlying, uint16_t year, uint16_t month, uint16_t day, double strike, ou::tf::OptionSide::enumOptionSide side );

} // namespace iqfeed
} // namespace tf
} // namespace ou
