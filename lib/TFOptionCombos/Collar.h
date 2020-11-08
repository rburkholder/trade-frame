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
 * File:    Collar.h
 * Author:  raymond@burkholder.net
 * Project: TFOptionCombos
 * Created on July 19, 2020, 05:43 PM
 */

#ifndef COLLAR_H
#define COLLAR_H

#include "Combo.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

class Collar: public Combo {
public:

  Collar();
  Collar( const Collar& rhs );
  Collar& operator=( const Collar& rhs ) = delete;
  Collar( const Collar&& rhs );
  virtual ~Collar();

  static size_t LegCount();

  static void ChooseLegs( // throw Chain exceptions
    Combo::E20DayDirection direction,
    const mapChains_t& chains,
    boost::gregorian::date,
    double priceUnderlying,
    fLegSelected_t&& );

  static const std::string Name( const std::string& sUnderlying, const mapChains_t& chains, boost::gregorian::date date, double price, Combo::E20DayDirection );

  virtual void Tick( double doubleUnderlyingSlope, double dblPriceUnderlying, ptime dt );

  // long by default for entry, short doesn't make much sense due to combo type
  virtual void PlaceOrder( ou::tf::OrderSide::enumOrderSide );

  virtual double GetNet( double price );

protected:
  virtual void Init( boost::gregorian::date, const mapChains_t* );
private:

  enum class ETransition { Initial, Vacant, Fill, Acquire, Track };
  using pOption_t = ou::tf::option::Option::pOption_t;

  struct Tracker {
    ETransition m_transition;
    const ou::tf::option::Chain* m_pChain;
    pOption_t m_pOption;
    Tracker(): m_transition( ETransition::Initial ), m_pChain( nullptr ) {}
    void SetChain( const ou::tf::option::Chain* pChain ) {
      m_pChain = pChain;
      m_transition = ETransition::Track;
    }
  };

  Tracker m_trackerFront;
  Tracker m_trackerSynthetic;

  void TestLong( const vLeg_t::size_type, double dblUnderlying, Tracker& );
  void Construct( ou::tf::OptionSide::enumOptionSide, double strikeItm, Tracker& );

};

} // namespace option
} // namespace tf
} // namespace ou

#endif /* COLLAR_H */
