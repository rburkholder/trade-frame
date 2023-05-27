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

// TODO: pass in a function to test if roll target already exists
//   acts as a gate to transition

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

  using fLegRoll_t = std::function<pPosition_t(pPosition_t old, pOption_t )>;
  using fLegClose_t = std::function<void(pPosition_t)>;

  Tracker();
  Tracker( const Tracker& ) = delete;
  Tracker( Tracker&& );
  ~Tracker();

  void Initialize(
    pPosition_t pPosition
  , bool bLock
  , const chain_t* pChain
  , fConstructOption_t&&
  , fLegRoll_t&&
  , fLegClose_t&&
  );

  bool TestLong( boost::posix_time::ptime, double dblUnderlyingSlope, double dblUnderlyingPrice );
  bool TestShort( boost::posix_time::ptime, double dblUnderlyingSlope, double dblUnderlyingPrice );
  void TestItmRoll( boost::gregorian::date, boost::posix_time::time_duration );

  void Emit();

  void ForceRoll();
  void ForceClose();

  void Quiesce(); // called from Collar

protected:
private:

  bool m_bLock;  // do not roll longs up or down

  using compare_t = std::function<bool(double,double)>;
  compare_t m_compare;

  using lu_strike_t = std::function<double(double)>;
  lu_strike_t m_luStrike;

  using lu_name_t = std::function<std::string(double)>;
  lu_name_t m_luNameAtStrike;

  double m_dblStrikePosition;
  ou::tf::OptionSide::EOptionSide m_sidePosition;

  double m_dblUnderlyingPrice;
  double m_dblUnderlyingSlope;

  enum class ETransition {
    Unknown   // unused
  , Initial   // on creation, waiting for basic position
  , Vacant_Init
  , Vacant    // candidate construction preparation
  , Acquire   // candidate construction
  , Spread    // candidate spread - wait for spread stats/stability
  , Track_Long // actively tracking price
  , Track_Short // actibvely tracking price
  , Roll_start  // needs to process the calendar roll at expiry
  , Close_start // force a close
  , Quiesce   // stop tracking
  , Done      // prepare for destruction
    };
  ETransition m_transition;
  ETransition m_track_type; // Track_Long or Track_Short

  const chain_t* m_pChain;

  pPosition_t m_pPosition; // existing option / position
  pOption_t m_pOptionCandidate; // track an option for next position with roll
    // TestLong: vertical or diagonal change
    // TestShort: calendar roll, if enough premium difference, else close leg

  fConstructOption_t m_fConstructOption;
  fLegRoll_t m_fLegRoll; // use two leg OrderCombo
  fLegClose_t m_fLegClose; // use one leg OrderCombo

  void Initialize( pPosition_t );

  void OptionCandidate_Construct( boost::posix_time::ptime, double strikeItm );
  void OptionCandidate_HandleQuote( const ou::tf::Quote& );

  void OptionCandidate_StartWatch();
  void OptionCandidate_StopWatch();

  void LegRoll();
  void LegClose();

};

} // namespace option
} // namespace tf
} // namespace ou
