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
 * File:    Strangle.h
 * Author:  raymond@burkholder.net
 * Project: TFOptions
 * Created on May 25, 2019, 10:56 PM
 */

#ifndef STRANGLE_H
#define STRANGLE_H

#include <OUCharting/ChartDataView.h>

#include <TFTrading/Position.h>

#include "Option.h"
#include "Leg.h"

#include "SpreadCandidate.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

class Strangle {
public:

  using pOption_t = Option::pOption_t;
  using pPosition_t = ou::tf::Position::pPosition_t;
  using pChartDataView_t = ou::ChartDataView::pChartDataView_t;

  enum class State { Initializing, Validating, Positions, Executing, Watching, Canceled, Closing };
  State m_state;

  Strangle();
  Strangle( const Strangle& rhs ) = delete;
  Strangle& operator=( const Strangle& rhs ) = delete;
  Strangle( const Strangle&& rhs );

  void SetOptionCall( pOption_t pCall, ou::Colour::enumColour colour );
  pOption_t GetOptionCall();
  void SetOptionPut( pOption_t pPut, ou::Colour::enumColour colour );
  pOption_t GetOptionPut();

  bool ValidateSpread( size_t nDuration );

  void SetPositionCall( pPosition_t pCall );
  pPosition_t GetPositionCall();
  void SetPositionPut( pPosition_t pPut );
  pPosition_t GetPositionPut();

  void Tick( bool bInTrend, double dblPriceUnderlying, ptime dt );

  void OrderLongStrangle();
  void CancelOrders();
  void ClosePositions();

  bool AreOrdersActive() const;
  void SaveSeries( const std::string& sPrefix );
  void AddChartData( pChartDataView_t pChartData );

  void SetColours( ou::Colour::enumColour colourCall, ou::Colour::enumColour colourPut );
  void SetColourCall( ou::Colour::enumColour colour );
  void SetColourPut( ou::Colour::enumColour colour );

  double GetNet();

  void CloseExpiryItm( const boost::gregorian::date date, double price );

private:
  double m_dblStrikeAtm;

  SpreadCandidate m_scCall;
  SpreadCandidate m_scPut;

  ou::tf::Leg m_legCall;
  ou::tf::Leg m_legPut;

  bool m_bUpperClosed;
  bool m_bLowerClosed;

  void Update( bool bTrending, double dblPrice );
};

} // namespace option
} // namespace tf
} // namespace ou

#endif /* STRANGLE_H */
