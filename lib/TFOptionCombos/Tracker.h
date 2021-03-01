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
   Rolls a long call up or a long put down to take profit
     => as a vertical or as a diagonal based upon chain provided
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

  using pPosition_t = ou::tf::Position::pPosition_t;
  using pOption_t = ou::tf::option::Option::pOption_t;

  using chain_t = ou::tf::option::Chain;

  using fConstructedOption_t = std::function<void(pOption_t)>;
  using fConstructOption_t   = std::function<void(const std::string&, fConstructedOption_t&&)>;

  using fRoll_t = std::function<pPosition_t/*new?*/(pOption_t,const std::string&)>;  // Fix -> OpenLeg
  using fClose_t = std::function<void(pPosition_t)>; // fix -> CloseLeg

  Tracker();
  Tracker( const Tracker& ) = delete;
  Tracker( Tracker&& );
  ~Tracker();

  void Initialize( // use for call roll-up, put roll-down
    pPosition_t pPosition,
    const chain_t* pChain,
    fConstructOption_t&&,
    fClose_t&&,
    fRoll_t&&
    );

  void TestLong( double dblUnderlyingSlope, double dblUnderlyingPrice );

protected:
private:

  using compare_t = std::function<bool(double,double)>;
  compare_t m_compare;

  using lu_strike_t = std::function<double(double)>;
  lu_strike_t m_luStrike;

  double m_dblStrikeWatch;
  ou::tf::OptionSide::enumOptionSide m_sideWatch;

  double m_dblUnderlyingPrice;
  double m_dblUnderlyingSlope;

  enum class ETransition { Initial, Vacant, Fill, Acquire, Track, Roll };
  ETransition m_transition;

  const chain_t* m_pChain;

  pPosition_t m_pPosition;
  pOption_t m_pOption;

  fConstructOption_t m_fConstructOption;
  fRoll_t m_fRoll;
  fClose_t m_fClose;

  void Construct( double strikeItm );
  void HandleOptionQuote( const ou::tf::Quote& );
  void Initialize( pPosition_t );

};

} // namespace option
} // namespace tf
} // namespace ou
