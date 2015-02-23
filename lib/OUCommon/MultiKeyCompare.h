/************************************************************************
 * Copyright(c) 2013, One Unified. All rights reserved.                 *
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

// started 2013/05/04

// source http://cnx.org/content/m35767/latest/
// source http://stackoverflow.com/questions/236801/should-operator-be-implemented-as-a-friend-or-as-a-member-function

#pragma once

#include <iostream>

namespace ou {

template<typename Key1, typename Key2>
struct MultiKeyCompare {
  MultiKeyCompare( const Key1& key1_, const Key2& key2_ )
    : key1( key1_ ), key2( key2 ) {};

  bool operator<( const MultiKeyCompare<Key1, Key2>& rhs ) const {
    if ( key1 == rhs.key1 ) {
      return key2 < rhs.key2;
    }
    else {
      return key1 < rhs.key1;
    }
  }
  bool operator==( const MultiKeyCompare<Key1, Key2>& rhs ) const {
    return ( !(this < rhs) && !(rhs < this) );
  }

  const Key1& GetKey1( void ) const { return key1; };
  const Key2& GetKey2( void ) const { return key2; };

private:
  friend std::ostream& operator<<( std::ostream&, const MultiKeyCompare<Key1, Key2>& );  // 2015/02/21 rebuild and notice warning here
  Key1 key1;
  Key2 key2;
};

template<typename Key>
std::ostream& operator<<( std::ostream& os, const Key& key ) {
  return os << key.key1 << "," << key.key2;
}

} // namespace ou

