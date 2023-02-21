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
  On a long option, if it goes down and passes through $0.10, mark that strike.
    when combo starts moving positive, buy in at $0.10 at a similar difference to see
    if can make some more on the recovery
 */

#pragma once

#include <TFTrading/Position.h>

#include <TFOptions/Chain.h>
#include <TFOptions/Option.h>

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

class Tracker {
public:

  using pPosition_t = ou::tf::Position::pPosition_t;
  using pOption_t = ou::tf::option::Option::pOption_t;

  using chain_t = ou::tf::option::Chain<ou::tf::option::chain::OptionName>;

  using fConstructedOption_t = std::function<void(pOption_t)>;
  using fConstructOption_t   = std::function<void(const std::string&, fConstructedOption_t&&)>;

  using fOpenLeg_t = std::function<pPosition_t/*new?*/(pOption_t,const std::string&)>;
  using fCloseLeg_t = std::function<void(pPosition_t)>;

  Tracker();
  Tracker( const Tracker& ) = delete;
  Tracker( Tracker&& );
  ~Tracker();

  void Initialize( // use for call roll-up, put roll-down
    pPosition_t pPosition,
    const chain_t* pChain,
    fConstructOption_t&&,
    fCloseLeg_t&&,
    fOpenLeg_t&&
    );

  void TestLong( boost::posix_time::ptime, double dblUnderlyingSlope, double dblUnderlyingPrice );
  void TestShort( boost::posix_time::ptime, double dblUnderlyingSlope, double dblUnderlyingPrice );
  void TestItmRoll( boost::gregorian::date, boost::posix_time::time_duration );

  void Quiesce(); // called from Collar

protected:
private:

  using compare_t = std::function<bool(double,double)>;
  compare_t m_compare;

  using lu_strike_t = std::function<double(double)>;
  lu_strike_t m_luStrike;

  double m_dblStrikePosition;
  ou::tf::OptionSide::EOptionSide m_sidePosition;

  double m_dblUnderlyingPrice;
  double m_dblUnderlyingSlope;

  enum class ETransition {
    Initial,   // on creation
    Vacant,
    Fill,
    Acquire,
    Track,
    Roll,
    Quiesce,
    Done       // prepare for destruction
    };
  ETransition m_transition;

  const chain_t* m_pChain;

  pPosition_t m_pPosition; // existing option / position
  pOption_t m_pOptionCandidate; // track an option for next position

  fConstructOption_t m_fConstructOption;
  fOpenLeg_t m_fOpenLeg;
  fCloseLeg_t m_fCloseLeg;

  void Construct( boost::posix_time::ptime, double strikeItm );
  void HandleLongOptionQuote( const ou::tf::Quote& );
  void Initialize( pPosition_t );

  void OptionCandidate_StartWatch();
  void OptionCandidate_StopWatch();

};

} // namespace option
} // namespace tf
} // namespace ou
