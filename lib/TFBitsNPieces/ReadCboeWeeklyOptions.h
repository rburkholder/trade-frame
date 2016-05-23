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

#include <boost/date_time/gregorian/gregorian_types.hpp>

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace cboe {

    typedef std::vector<boost::gregorian::date> vOptionExpiryDates_t;

    struct OptionExpiryDates_t {
      vOptionExpiryDates_t vExpiriesStandardWeeklies;
      vOptionExpiryDates_t vExpiriesExpandedWeeklies;
      vOptionExpiryDates_t vExpiriesEndOfWeek;
      vOptionExpiryDates_t vExpiriesSpxwXsp;
      vOptionExpiryDates_t vExpiriesSpxWednesday;
      vOptionExpiryDates_t vExpiriesVixWeeklies;
    };
    
    struct UnderlyingInfo {
      std::string sSymbol;
      bool bAdded;
      std::string sDescription;
      std::string sProductType;
      //boost::gregorian::date dateInitialList;
      boost::gregorian::date dateListed;
      bool bStandardWeekly;
      bool bExpandedWeekly;
      bool bEOW;
      UnderlyingInfo( void ):
        bAdded(false), bStandardWeekly(false), bExpandedWeekly(false), bEOW(false)
        {  };
      bool operator<( const UnderlyingInfo& rhs ) {
	return sSymbol < rhs.sSymbol;
      }
    };
    typedef std::vector<UnderlyingInfo> vUnderlyinginfo_t;

    //std::vector<UnderlyingInfo> vui;

void ReadCboeWeeklyOptions( OptionExpiryDates_t&, vUnderlyinginfo_t& );

} // namespace cboe  
} // namespace tf
} // namespace ou



// file link location to automatically retrieve:  
// http://www.cboe.com/micro/weeklys/availableweeklys.aspx

//      JAN 	FEB 	MAR 	APR 	MAY 	JUN 	JUL 	AUG 	SEP 	OCT 	NOV 	DEC
//CALLS 	A 	B 	C 	D 	E 	F 	G 	H 	I 	J 	K 	L
//PUTS 	M 	N 	O 	P 	Q 	R 	S 	T 	U 	V 	W 	X
