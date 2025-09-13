/************************************************************************
 * Copyright(c) 2025, One Unified. All rights reserved.                 *
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
 * File:    TrackBracketOrder.hpp
 * Author:  raymond@burkholder.net
 * Project: TFBitsNPieces
 * Created: September 13, 2025 12:52:21
 */

#pragma once

#include "TrackOrderBase.hpp"

namespace ou { // namespace oneunified
namespace tf { // namespace tradeframe

// unused, no tick for tracking stop, ... requires re-factoring
// => could connect a HandleQuote to m_pWatch in m_pPosition
class TrackBracketOrder: public TrackOrderBase {
public:

  struct OrderArgs: public TrackOrderBase::OrderArgs {
    double profit;
    double stop;
    OrderArgs(): profit {}, stop {} {}

    // limit, stop
    //explicit OrderArgs( boost::posix_time::ptime dt_, quantity_t quantity_, double signal_, double limit_, double stop_ )
    //: dt( dt_ ), quantity( quantity_ ), signal( signal_ ), limit( limit_ ), profit {}, stop( stop_ ), duration {}
    //{
    //  assert( 0 < quantity );
    //}


    // limit time limit, stop
    //explicit OrderArgs( boost::posix_time::ptime dt_, quantity_t quantity_, double signal_, double limit_, double stop_, int duration_ )
    //: dt( dt_ ), quantity( quantity_ ), signal( signal_ ), limit( limit_ ), profit {}, stop( stop_ ), duration( duration_ )
    //{
    //  assert( 0 < quantity );
    //}

  };

  using pPosition_t = ou::tf::Position::pPosition_t;

  TrackBracketOrder();
  TrackBracketOrder( pPosition_t, ou::ChartDataView&, int slot );
  virtual ~TrackBracketOrder();

  void EnterLongBracket( const OrderArgs& ); // not useable at present

protected:
private:

  double m_dblProfitMax;  // not used, only referenced in bracket
  double m_dblUnRealized; // not used, only referenced in bracket
  double m_dblProfitMin;  // not used, only referenced in bracket

  std::string m_sProfitDescription; // doesn't appear to be set, referenced in ExitPosition

  void HandleExitOrderCancelled( const ou::tf::Order& ); // unused
  void HandleExitOrderFilled( const ou::tf::Order& ); // unused

  void ExitPosition( const ou::tf::Quote& ); // unused

  void ShowOrder( pOrder_t& ); // unused

};

} // namespace tradeframe
} // namespace oneunified
