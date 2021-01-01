/************************************************************************
 * Copyright(c) 2013, One Unified. All rights reserved.                 *
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

// Started 2013/09/21

// http://www.cboe.com/micro/weeklys/availableweeklys.aspx

#include <vector>
#include <functional>

#include <boost/date_time/gregorian/gregorian_types.hpp>

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace cboe {
namespace xls {

    using vOptionExpiryDates_t = std::vector<boost::gregorian::date>;

    struct OptionExpiryDates_t {
      vOptionExpiryDates_t vExpiriesStandardWeeklies;
      vOptionExpiryDates_t vExpiriesExpandedWeeklies;
      vOptionExpiryDates_t vExpiriesEndOfWeek;
      vOptionExpiryDates_t vExpiriesSpxwXsp;
      vOptionExpiryDates_t vExpiriesXspWednesday;
      vOptionExpiryDates_t vExpiriesSpxMonWed;
      vOptionExpiryDates_t vExpiriesEtfMonday;
      vOptionExpiryDates_t vExpiriesEtfWedndesday;
      vOptionExpiryDates_t vExpiriesVixWeeklies;
    };

    struct UnderlyingInfo {
      std::string sSymbol;
      bool bAdded;
      std::string sDescription;
      std::string sProductType;
      boost::gregorian::date dateListed;
      bool bStandardWeekly;
      bool bExpandedWeekly;
      bool bEOW;
      UnderlyingInfo( void )
        : bAdded(false), bStandardWeekly(false), bExpandedWeekly(false), bEOW(false)
        {};
      bool operator<( const UnderlyingInfo& rhs ) const {
	return sSymbol < rhs.sSymbol;
      }
    };

    using vUnderlyinginfo_t = std::vector<UnderlyingInfo>;
    using fUnderlyingInfo_t = std::function<void(const UnderlyingInfo&)>;

void ReadCboeWeeklyOptions( OptionExpiryDates_t&, fUnderlyingInfo_t );
void ReadCboeWeeklyOptions( OptionExpiryDates_t&, vUnderlyinginfo_t& );

} // namespace xls
} // namespace cboe
} // namespace tf
} // namespace ou



// file link location to automatically retrieve:
// http://www.cboe.com/micro/weeklys/availableweeklys.aspx

//      JAN 	FEB 	MAR 	APR 	MAY 	JUN 	JUL 	AUG 	SEP 	OCT 	NOV 	DEC
//CALLS 	A 	B 	C 	D 	E 	F 	G 	H 	I 	J 	K 	L
//PUTS 	M 	N 	O 	P 	Q 	R 	S 	T 	U 	V 	W 	X
