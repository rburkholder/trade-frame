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

#include <boost/cstdint.hpp>

#include <TFTrading/TradingEnumerations.h>

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed

// Coding for writing to a sqlite database was stopped as it appeared to take about four to five hours to update
// about a million records.  A bit too long for instant gratification.

class MarketSymbol {
public:

  enum enumSymbolClassifier: boost::uint32_t { // bits in bitmap of stored data record
    Unknown = 0, Bonds, Calc, Equity, FOption, Forex, Forward, Future, ICSpread, 
      IEOption, Index, MktRpt, MktStats, Money, Mutual, PrecMtl, Spot, Spread, StratSpread, Swaps, Treasuries,
      _Count
  };

  struct TableRowDef {

    std::string sSymbol;
    std::string sDescription;
    std::string sExchange;
    std::string sListedMarket;
    enumSymbolClassifier sc;
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

    std::string sSecurityType;  // used during initial parsing
    std::string sFrontMonth;  // used during initial parsing

    template<class A>
    void Fields( A& a) {
      ou::db::Field( a, "symbol", sSymbol );
      ou::db::Field( a, "description", sDescription );
      ou::db::Field( a, "exchange", sExchange );
      ou::db::Field( a, "market", sListedMarket );
      ou::db::Field( a, "symbolclass", sc );
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
      sc( Unknown ), bFrontMonth( false ), bHasOptions( false ), nSIC( 0 ), nNAICS( 0 ),
      eOptionSide( ou::tf::OptionSide::Unknown ) {};

  };

  struct TableCreateDef: TableRowDef {
    template<class A>
    void Fields( A& a ) {
      TableRowDef::Fields( a );
      ou::db::Key( a, "symbol" );
    }
  };

  MarketSymbol(void);
  MarketSymbol( const TableRowDef& row ) : m_row( row ) {};
  ~MarketSymbol(void);

  const TableRowDef& GetRow( void ) const { return m_row; };

protected:
private:
  TableRowDef m_row;
};

} // namespace iqfeed
} // namespace tf
} // namespace ou
