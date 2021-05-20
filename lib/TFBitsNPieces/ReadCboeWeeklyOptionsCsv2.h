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

  struct UnderlyingInfo {
    std::string sSymbol;
    std::string sDescription;
    void clear() {
      sSymbol.clear();
      sDescription.clear();
    }
    bool operator<( const UnderlyingInfo& rhs ) const {
     return sSymbol < rhs.sSymbol;
    }
    UnderlyingInfo() {}
  };

  using vUnderlyinginfo_t = std::vector<UnderlyingInfo>;
  using fUnderlyingInfo_t = std::function<void(const UnderlyingInfo&)>;

void ReadCboeWeeklyOptions( fUnderlyingInfo_t&& );
void ReadCboeWeeklyOptions( vUnderlyinginfo_t& );

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

