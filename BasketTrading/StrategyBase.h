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
 * File:    StrategyBase.h
 * Author:  raymond@burkholder.net
 * Project: BasketTrading
 * Created on June 23, 2019, 9:28 PM
 */

#ifndef STRATEGYBASE_H
#define STRATEGYBASE_H

#include <map>

#include <boost/date_time/gregorian/greg_date.hpp>

#include <TFOptions/Chain.h>

// base class from which specific strategies inherit

class StrategyBase {
public:

  using chain_t = ou::tf::option::Chain;
  using mapChains_t = std::map<boost::gregorian::date, chain_t>;
  using citerChain_t = mapChains_t::const_iterator;

  struct exception_chain_no_found: public std::runtime_error {
    exception_chain_no_found( const char* ch ): std::runtime_error( ch ) {}
  };

  StrategyBase( );
  StrategyBase( const StrategyBase&& orig );
  virtual ~StrategyBase( );

protected:
  static citerChain_t SelectChain( const mapChains_t&, boost::gregorian::date, boost::gregorian::days daysToExpiry );
private:

};

#endif /* STRATEGYBASE_H */

