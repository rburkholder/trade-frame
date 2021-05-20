/************************************************************************
 * Copyright(c) 2020, One Unified. All rights reserved.                 *
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

// Started 2020/12/28

#include <string>
#include <vector>
#include <functional>

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace cboe {
namespace csv {

  struct ExpiryEntry {
    std::string sName;
    std::vector<std::string> vExpiry;
    ExpiryEntry() {}
    ExpiryEntry( const std::string& sName_ ): sName( sName_ ) {}
    ExpiryEntry( const std::string& sName_, const std::vector<std::string>& vExpiry_ )
    : sName( sName_ ), vExpiry( vExpiry_ ) {}
    using value_type = ExpiryEntry;
  };

  struct SymbolEntry {
    std::string sSymbol;
    std::string sDescription;
    SymbolEntry() {}
    SymbolEntry( const std::string& sSymbol_ ): sSymbol( sSymbol_ ) {}
    SymbolEntry( const std::string& sSymbol_, const std::string& sDescription_ )
    : sSymbol( sSymbol_ ), sDescription( sDescription_ )  {}
    using value_type = SymbolEntry;
  };

  struct WeeklyOptions {
    std::vector<std::string> vHeader;
    std::vector<ExpiryEntry> vExpiryEntry;
    std::vector<SymbolEntry> vSymbolExchTrade;
    std::vector<SymbolEntry> vSymbolEquity;
  };

  using fSymbolEntry_t = std::function<void(const SymbolEntry&)>;

  void ReadCboeWeeklyOptions( fSymbolEntry_t&& fET, fSymbolEntry_t&& fEquity );
  void ReadCboeWeeklyOptions( fSymbolEntry_t&& f ); // does both
  void ReadCboeWeeklyOptions( WeeklyOptions& );

} // namespace csv
} // namespace cboe
} // namespace tf
} // namespace ou

// file link location to automatically retrieve:
// https://www.cboe.com/us/options/symboldir/weeklys_options/?download=csv
// at https://www.cboe.com/us/options/symboldir/weeklys_options/

//      JAN FEB	MAR	APR	MAY	JUN JUL	AUG	SEP	OCT	NOV	DEC
//CALLS 	A 	B 	C 	D 	E 	F 	G 	H 	I 	J 	K 	L
//PUTS 	  M 	N 	O 	P 	Q 	R 	S 	T 	U 	V 	W 	X
