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

// this code might be deprecating some code in Bundle

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

class Chain {
public:
  Chain( );
  Chain( const Chain&& rhs );
  virtual ~Chain( );

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

  struct OptionsAtStrike {
    std::string sCall;
    std::string sPut;
    OptionsAtStrike() {}
    OptionsAtStrike( const OptionsAtStrike& rhs )
    : sCall( rhs.sCall ),
      sPut(  rhs.sPut  )
    { }
    OptionsAtStrike( const OptionsAtStrike&& rhs )
    : sCall( std::move( rhs.sCall ) ),
      sPut(  std::move( rhs.sPut  ) )
    { }
  };

  using mapChain_t = std::map<double, OptionsAtStrike>;

  mapChain_t m_mapChain;

  mapChain_t::const_iterator FindStrike( const double strike ) const;
  mapChain_t::iterator FindStrike( const double strike );

};

} // namespace option
} // namespace tf
} // namespace ou

#endif /* CHAIN_H */

