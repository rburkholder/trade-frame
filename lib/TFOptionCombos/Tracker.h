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

#pragma once

#include <TFOptions/Option.h>
#include <TFOptions/Chain.h>

#include <TFTrading/Position.h>

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

class Tracker {
public:

  using pPosition_t = ou::tf::Position::pPosition_t;
  using chain_t = ou::tf::option::Chain;

  using pOption_t = ou::tf::option::Option::pOption_t;
  using fConstructedOption_t = std::function<void(pOption_t)>;
  using fConstructOption_t = std::function<void(const std::string&, fConstructedOption_t&&)>;

  using fRoll_t = std::function<void(pOption_t)>;

  Tracker();
  ~Tracker();

  void Initialize( // ensure it is re-usable
    pPosition_t pPosition,
    const chain_t* pChain,
    fConstructOption_t&&,
    fRoll_t&&
    );

  void TestLong( double dblUnderlyingSlope, double dblUnderlying );

protected:
private:

  using pWatch_t      = ou::tf::option::Option::pWatch_t;
  using pInstrument_t = ou::tf::Instrument::pInstrument_t;

  using compare_t = std::function<bool(double,double)>;
  compare_t m_compare;

  using lu_strike_t = std::function<double(double)>;
  lu_strike_t m_luStrike;

  double m_dblStrikePosition;
  ou::tf::OptionSide::enumOptionSide m_sidePosition;

  double m_dblUnderlying;
  double m_dblUnderlyingSlope;

  enum class ETransition { Initial, Vacant, Fill, Acquire, Track, Roll };
  ETransition m_transition;

  const chain_t* m_pChain;

  pPosition_t m_pPosition;
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
