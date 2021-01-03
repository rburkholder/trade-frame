/************************************************************************
 * Copyright(c) 2020, One Unified. All rights reserved.                 *
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
 * File:    Tracker.h
 * Author:  raymond@burkholder.net
 * Project: TFOptionCombos
 * Created on Novemeber 8, 2020, 11:41 AM
 */

 /*
   Rolls a long call up or a long put down to take
     as a vertical or as a diagonal
 */

#pragma once

#include <TFTrading/Position.h>

#include <TFOptions/Option.h>
#include <TFOptions/Chain.h>

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

class Tracker {
public:

  using pWatch_t  = ou::tf::option::Option::pWatch_t;
  using pOption_t = ou::tf::option::Option::pOption_t;

  using chain_t = ou::tf::option::Chain;

  using fConstructedOption_t = std::function<void(pOption_t)>;
  using fConstructOption_t   = std::function<void(const std::string&, fConstructedOption_t&&)>;

  using fRoll_t = std::function<void(pOption_t)>;

  Tracker();
  ~Tracker();

  void Initialize( // ensure it is re-usable
    pWatch_t pWatch,
    const chain_t* pChain,
    fConstructOption_t&&,
    fRoll_t&&
    );

  void TestLong( double dblUnderlyingSlope, double dblUnderlying );

protected:
private:

  using compare_t = std::function<bool(double,double)>;
  compare_t m_compare;

  using lu_strike_t = std::function<double(double)>;
  lu_strike_t m_luStrike;

  double m_dblStrikeWatch;
  ou::tf::OptionSide::enumOptionSide m_sideWatch;

  double m_dblUnderlying;
  double m_dblUnderlyingSlope;

  enum class ETransition { Initial, Vacant, Fill, Acquire, Track, Roll };
  ETransition m_transition;

  const chain_t* m_pChain;

  pWatch_t m_pWatch;
  pOption_t m_pOption;

  fConstructOption_t m_fConstructOption;
  fRoll_t m_fRoll;

  void Construct( double strikeItm );
  void HandleOptionQuote( const ou::tf::Quote& );

};

} // namespace option
} // namespace tf
} // namespace ou
