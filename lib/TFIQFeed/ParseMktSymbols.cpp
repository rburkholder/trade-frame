/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
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

#include "StdAfx.h"

//#include <map>
//#include <vector>
//#include <algorithm>
#include <string>

#include <OUCommon/KeywordMatch.h>
//#include <OUCommon/Debug.h>

#include "ParseMktSymbols.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed

void doout( const std::string& s ) {
  std::cout << "'" << s << "'" << std::endl;
}

// http://www.relisoft.com/book/tech/6lib.html
class LessThan
    : public std::binary_function<char const *, char const *, bool> {
public:
    bool operator () (char const * str1, 
                      char const * str2) const {
        return strcmp (str1, str2) < 0;
    }
};

//  FUTURE MONTH CODES
//  Jan-F    Feb-G    Mar-H    Apr-J    May-K    Jun-M   
//  Jul-N    Aug-Q    Sep-U    Oct-V    Nov-X    Dec-Z 
boost::uint8_t rFutureMonth[] = {
    0,  // A
    0,  // B
    0,  // C
    0,  // D
    0,  // E
      1,  // F
      2,  // G
      3,  // H
    0,  // I
      4,  // J
      5,  // K
    0,  // L
      6,  // M
      7,  // N
    0,  // O
    0,  // P
      8,  // Q
    0,  // R
    0,  // S
    0,  // T
      9,  // U
     10,  // V
    0,  // W
     11,  // X
    0,  // Y
     12  // Z
  };
 
} // namespace iqfeed
} // namespace tf
} // namespace ou
