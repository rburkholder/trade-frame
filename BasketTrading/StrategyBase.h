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

// utility has std::move
#include <utility>

//#include <map>

//#include <boost/date_time/gregorian/greg_date.hpp>

//#include <TFOptions/Chain.h>

//#include "LegSelected.h"

namespace Strategy {

// == Common

class Common {
public:

  //using vLegSelected_t = LegSelected::vLegSelected_t;
  //using mapChains_t = std::map<boost::gregorian::date, ou::tf::option::Chain>;

  Common();
  virtual ~Common();

//  virtual void ChooseStrikes( vLegSelected_t&, const mapChains_t&, boost::gregorian::date, double price ) = 0; // throw Chain exceptions

protected:

//  using vLeg_t = std::vector<ou::tf::Leg>;
//  vLeg_t m_vLeg;



private:

};

// == Base

template<typename ComboStrategy, typename Combination>
class Base: public Common {
public:

  Base( ) {}
  Base( const Base&& rhs )
  : m_combo( std::move( rhs.m_combo ) ) {}
  virtual ~Base( ) {} // is the virtual necessary?

  Combination& Combo() { return m_combo; }

protected:

  Combination m_combo;

private:

};

} // namespace Strategy

#endif /* STRATEGYBASE_H */