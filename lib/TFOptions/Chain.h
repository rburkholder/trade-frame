/************************************************************************
 * Copyright(c) 2019, One Unified. All rights reserved.                 *
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

/*
 * File:    Chains.h
 * Author:  raymond@burkholder.net
 * Project: TFOptions
 * Created on June 23, 2019, 9:49 PM
 */

#ifndef CHAIN_H
#define CHAIN_H

#include <map>
#include <string>
#include <stdexcept>

// use this for light weight strike calculations and name lookups
// use Bundle for for full Strike/Call/Put watch

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

namespace chain {

  struct Option { // inherit to add addtiional fields
    std::string sName;
    Option() {}
    Option( const std::string& sName_ )
    : sName( sName_ ) {}
    Option( const std::string&& sName_ )
    : sName( std::move( sName_ ) ) {}
    Option( const Option& rhs )
    : sName( rhs.sName ) {}
    Option( const Option&& rhs )
    : sName( std::move( rhs.sName ) ) {}
  };

  template<typename Option=Option>
  struct Strike {
    Option call;
    Option put;
    Strike() {}
    Strike( Option&& call_, Option&& put_ )
    : call( std::move( call_ ) ),
      put( std::move( put_ ) )
      {}
    Strike( const Strike& rhs )
    : call( rhs.call ),
      put( rhs.put )
      {}
    Strike( const Strike&& rhs )
    : call( std::move( rhs.call ) ),
      put( std::move( rhs.put ) )
      {}
  };
}

class Chain {
public:
  Chain() {}
  Chain( const Chain&& rhs ) {
    m_mapChain = std::move( rhs.m_mapChain );
  }
  virtual ~Chain() {};

  struct exception_strike_not_found: public std::runtime_error {
    exception_strike_not_found( const char* ch ): std::runtime_error( ch ) {}
  };
  struct exception_at_start_of_chain: public exception_strike_not_found {
    exception_at_start_of_chain( const char* ch ): exception_strike_not_found( ch ) {}
  };

  void SetIQFeedNameCall( double dblStrike, const std::string& sIQFeedSymbolName );
  void SetIQFeedNamePut( double dblStrike, const std::string& sIQFeedSymbolName );

  const std::string GetIQFeedNameCall( double dblStrike ) const;
  const std::string GetIQFeedNamePut( double dblStrike ) const;

  double Put_Itm( double ) const ;
  double Put_ItmAtm( double ) const;
  double Put_Atm( double ) const;
  double Put_OtmAtm( double ) const;
  double Put_Otm( double ) const;

  double Call_Itm( double ) const;
  double Call_ItmAtm( double ) const;
  double Call_Atm( double ) const;
  double Call_OtmAtm( double ) const;
  double Call_Otm( double ) const;

  // returns 0, 1, 2 strikes found
  // needs exact match on strikeSource
  int AdjacentStrikes( double strikeSource, double& strikeLower, double& strikeUpper ) const;

  void EmitValues( void ) const;

  void Test( double price );

protected:
private:

  using option_t = chain::Option;
  using strike_t = chain::Strike<chain::Option>;
  using mapChain_t = std::map<double, strike_t>;

  mapChain_t m_mapChain;

  mapChain_t::const_iterator FindStrike( const double strike ) const;
  mapChain_t::iterator FindStrike( const double strike );

};

} // namespace option
} // namespace tf
} // namespace ou

#endif /* CHAIN_H */

