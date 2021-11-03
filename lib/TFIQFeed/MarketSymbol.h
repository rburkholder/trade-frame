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

#pragma once

#include <bitset>
#include <string>

#include <boost/date_time/gregorian/greg_date.hpp>

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

#include <OUSQL/Functions.h>

#include <TFTrading/TradingEnumerations.h>

#include "SecurityType.h"
#include "Fundamentals.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed

// Coding for writing to a sqlite database was stopped as it appeared to take about four to five hours to update
// about a million records.  A bit too long for instant gratification.
// 2013/10/09 Will need to try it in release mode.

class MarketSymbol {
public:

  struct TableRowDef {

    std::string sSymbol;
    std::string sDescription;
    std::string sExchange;
    std::string sListedMarket;
    ESecurityType sc;
    boost::uint16_t nMultiplier;
    boost::uint32_t nSIC;
    boost::uint32_t nNAICS;
    std::string sUnderlying;
    ou::tf::OptionSide::enumOptionSide eOptionSide;
    double dblStrike;
    boost::uint16_t nYear;
    boost::uint8_t nMonth;
    boost::uint8_t nDay;
    bool bFrontMonth;
    bool bHasOptions;

    template<class A>
    void Fields( A& a) {
      ou::db::Field( a, "symbol", sSymbol );
      ou::db::Field( a, "description", sDescription );
      ou::db::Field( a, "exchange", sExchange );
      ou::db::Field( a, "market", sListedMarket );
      ou::db::Field( a, "symbolclass", sc );
      ou::db::Field( a, "multiplier", nMultiplier );  // 2014/10/11 new field, parsers need to be changed to update this field
      ou::db::Field( a, "sic", nSIC );
      ou::db::Field( a, "naics", nNAICS );
      ou::db::Field( a, "underlying", sUnderlying );
      ou::db::Field( a, "optionside", eOptionSide );
      ou::db::Field( a, "strike", dblStrike );
      ou::db::Field( a, "year", nYear );
      ou::db::Field( a, "month", nMonth );
      ou::db::Field( a, "day", nDay );
      ou::db::Field( a, "frontmonth", bFrontMonth );
      ou::db::Field( a, "hasoptions", bHasOptions );
    }

    TableRowDef(void): dblStrike( 0 ), nYear( 0 ), nMonth( 0 ), nDay( 0 ),
      sc( ESecurityType::Unknown ), bFrontMonth( false ), bHasOptions( false ), nSIC( 0 ), nNAICS( 0 ), nMultiplier( 1 ),
      eOptionSide( ou::tf::OptionSide::Unknown ) {};

  private:

    /* serialization support */

    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive& ar,const unsigned int) {
      ar
        & BOOST_SERIALIZATION_NVP(sSymbol)
        & BOOST_SERIALIZATION_NVP(sDescription)
        & BOOST_SERIALIZATION_NVP(sExchange)
        & BOOST_SERIALIZATION_NVP(sListedMarket)
        & BOOST_SERIALIZATION_NVP(sc)
        & BOOST_SERIALIZATION_NVP(nSIC)
        & BOOST_SERIALIZATION_NVP(nNAICS)
        & BOOST_SERIALIZATION_NVP(sUnderlying)
        & BOOST_SERIALIZATION_NVP(eOptionSide)
        & BOOST_SERIALIZATION_NVP(dblStrike)
        & BOOST_SERIALIZATION_NVP(nYear)
        & BOOST_SERIALIZATION_NVP(nMonth)
        & BOOST_SERIALIZATION_NVP(nDay)
        & BOOST_SERIALIZATION_NVP(bFrontMonth)
        & BOOST_SERIALIZATION_NVP(bHasOptions)
        ;
    }

  };

  struct TableCreateDef: TableRowDef {
    template<class A>
    void Fields( A& a ) {
      TableRowDef::Fields( a );
      ou::db::Key( a, "symbol" );
    }
  };

  MarketSymbol();
  MarketSymbol( const TableRowDef& row ) : m_row( row ) {};
  ~MarketSymbol();

  const TableRowDef& GetRow( void ) const { return m_row; };

  using Date = boost::gregorian::date;

  // equity, ieoption (from trd)
  const std::string static BuildGenericName( const std::string& sBaseName, const TableRowDef& );
  // equity, ieoption, future, futures option (from fundamentals)
  const std::string static BuildGenericName( const std::string& sBaseName, const TableRowDef&, Date );
  // use fundamentals directly
  const std::string static BuildGenericName( const TableRowDef&, const Fundamentals& );

protected:
private:
  TableRowDef m_row;
};

} // namespace iqfeed
} // namespace tf
} // namespace ou
