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
 * File:    StrategyStrangle.h
 * Author:  raymond@burkholder.net
 * Project: BasketTrading
 * Created on June 23, 2019, 9:33 PM
 */

#ifndef STRATEGYSTRANGLE_H
#define STRATEGYSTRANGLE_H

#include <TFOptions/Strangle.h>

#include "StrategyBase.h"

// manage a short strangle to collect declining theta

namespace Strategy {

class Strangle: public Base<Strangle,ou::tf::option::Strangle> {
public:

  Strangle();
  Strangle( const Strangle&& );
  virtual ~Strangle();

  //void ChooseStrikes( const double lower, const double upper, boost::gregorian::date date, const mapChains_t& );
  //static void ChooseStrikes(
  //  const mapChains_t&, boost::gregorian::date, double lower, double upper, ou::tf::option::Strangle::fLegSelected_t&& ); // throw Chain exceptions

protected:
private:

  //using LegDef = Combo::LegDef;
  //using leg_pair_t = ou::tf::option::Combo::leg_pair_t;

  //static const leg_pair_t m_legDefLong;
  //static const leg_pair_t m_legDefShort;

};

} // namespace Strategy

#endif /* STRATEGYSTRANGLE_H */
