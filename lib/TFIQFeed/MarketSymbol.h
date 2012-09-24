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

class MarketSymbol {
public:

  enum enumSymbolClassifier: boost::uint8_t { // bits in bitmap of stored data record
    Unknown = 0, Bonds, Calc, Equity, FOption, Forex, Forward, Future, ICSpread, 
      IEOption, Index, MktStats, Money, Mutual, PrecMtl, Spot, Spread, StratSpread, Swaps, Treasuries
  };

  struct TableRowDef {

    std::string sSymbol;
    std::string sDescription;
    std::string sExchange;
    std::string sListedMarket;
    enumSymbolClassifier sc;
//    ou::tf::InstrumentType::enumInstrumentTypes eContractTypes;
    std::string sSIC;
    std::string sNAICS;
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
//      ou::db::Field( a, "type", eContractTypes );
      ou::db::Field( a, "sic", sSIC );
      ou::db::Field( a, "naics", sNAICS );
      ou::db::Field( a, "optionside", eOptionSide );
      ou::db::Field( a, "strike", dblStrike );
      ou::db::Field( a, "year", nYear );
      ou::db::Field( a, "month", nMonth );
      ou::db::Field( a, "day", nDay );
      ou::db::Field( a, "frontmonth", bFrontMonth );
      ou::db::Field( a, "hasoptions", bHasOptions );
    }

    TableRowDef(void): dblStrike( 0 ), nYear( 0 ), nMonth( 0 ), nDay( 0 ), 
      sc( Unknown ), bFrontMonth( false ), bHasOptions( false ),
//      eContractTypes( ou::tf::InstrumentType::Unknown ), 
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
