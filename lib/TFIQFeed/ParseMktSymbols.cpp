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

#include <map>
#include <vector>
#include <algorithm>

#include <OUCommon/KeywordMatch.h>
#include <OUCommon/Debug.h>

#include "ParseMktSymbols.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed

ParseMktSymbols::ParseMktSymbols(void) {
}

ParseMktSymbols::~ParseMktSymbols(void) {
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

unsigned char DecodeMonth( const std::string &s ) {
  unsigned char month2 = 0;
  if ( 'J' < s[0] ) {
    if ( 'N' < s[0] ) {
      month2 = ( 'O' == s[0] ) ? 10 : 9;
    }
    else {
      if ( 'M' == s[0] ) {
        month2 = ( 'R' == s[2] ) ? 3 : 5;
      }
      else {
        month2 = 11;
      }
    }
  }
  else {
    if ( 'J' == s[0] ) {
      if ( 'U' == s[1] ) {
        month2 = ( 'N' == s[2] ) ? 6 : 7;
      }
      else {
        month2 = 1;
      }
    }
    else {
      if ( 'A' == s[0] ) {
        month2 = ( 'P' == s[1] ) ? 4 : 8;
      }
      else {
        month2 = ( 'D' == s[0] ) ? 12 : 2;
      }
    }
  }
  assert( month2 > 0 );
  return month2;
}

void ParseMktSymbols::Parse( void ) {
}

} // namespace iqfeed
} // namespace tf
} // namespace ou
