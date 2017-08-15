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

#include <string>

#include <TFTrading/TradingEnumerations.h>

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed

struct NameParts {
  InstrumentType::enumInstrumentTypes it;
  const std::string& sRootName;
  boost::uint16_t year;
  boost::uint16_t month;
  boost::uint16_t day;
  double strike;
  ou::tf::OptionSide::enumOptionSide side;
  NameParts(InstrumentType::enumInstrumentTypes it_, const std::string& sRootName_)
    : it( it_ ), sRootName( sRootName_), year(0), month(0), day(0), strike(0.0),
        side( ou::tf::OptionSide::Unknown) {}
  NameParts(InstrumentType::enumInstrumentTypes it_, const std::string& sRootName_,
    boost::uint16_t year_, boost::uint16_t month_, boost::uint16_t day_,
    double strike_, ou::tf::OptionSide::enumOptionSide side_ )
      : it( it_ ), sRootName( sRootName_ ), year( year_ ), month( month_ ), day( day_ ),
          strike( strike_ ), side( side_ ) {}
  NameParts(InstrumentType::enumInstrumentTypes it_, const std::string& sRootName_,
    boost::uint16_t year_, boost::uint16_t month_, boost::uint16_t day_ )
      : it( it_ ), sRootName( sRootName_ ), year( year_ ), month( month_ ), day( day_ ), 
          strike(0.0), side( ou::tf::OptionSide::Unknown) {}
};
  
// months are 1 .. 12
const std::string BuildName( const NameParts& parts );
const std::string BuildOptionName( const std::string& sUnderlying, boost::uint16_t year, boost::uint16_t month, boost::uint16_t day, double strike, ou::tf::OptionSide::enumOptionSide side );
const std::string BuildFuturesName( const std::string& sUnderlying, boost::uint16_t year, boost::uint16_t month );
const std::string BuildFuturesOptionName( const std::string& sUnderlying, boost::uint16_t year, boost::uint16_t month, double strike, ou::tf::OptionSide::enumOptionSide side );
const std::string BuildFuturesOptionName( const std::string& sUnderlying, boost::uint16_t year, boost::uint16_t month, boost::uint16_t day, double strike, ou::tf::OptionSide::enumOptionSide side );

} // namespace iqfeed
} // namespace tf
} // namespace ou
