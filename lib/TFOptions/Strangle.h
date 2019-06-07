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

#include <TFTrading/Portfolio.h>
#include <TFTrading/Position.h>

#include "Option.h"
#include "Leg.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

class Strangle {
public:

  using pInstrument_t = ou::tf::Instrument::pInstrument_t;
  using pOption_t = Option::pOption_t;
  using pPosition_t = ou::tf::Position::pPosition_t;
  using pPortfolio_t = ou::tf::Portfolio::pPortfolio_t;
  using pChartDataView_t = ou::ChartDataView::pChartDataView_t;

  enum class State { Initializing, Positions, Executing, Watching, Canceled, Closing };
  State m_state;

  Strangle();
  Strangle( const Strangle& rhs ) = delete;
  Strangle& operator=( const Strangle& rhs ) = delete;
  Strangle( const Strangle&& rhs );

  void SetPortfolio( pPortfolio_t );
  pPortfolio_t GetPortfolio() { return m_pPortfolio; }

  void SetPositionCall( pPosition_t pCall );
  pPosition_t GetPositionCall();
  void SetPositionPut( pPosition_t pPut );
  pPosition_t GetPositionPut();

  void Tick( bool bInTrend, double dblPriceUnderlying, ptime dt );

  void PlaceOrder( ou::tf::OrderSide::enumOrderSide );
  void CancelOrders();
  void ClosePositions();

  bool AreOrdersActive() const;
  void SaveSeries( const std::string& sPrefix );

  void AddChartDataCall( pChartDataView_t pChartData, ou::Colour::enumColour colour );
  void AddChartDataPut( pChartDataView_t pChartData, ou::Colour::enumColour colour );

  double GetNet( double price );

  void CloseExpiryItm( double price, const boost::gregorian::date date );
  void CloseFarItm( double price );
  void CloseForProfits( double price );
  bool CloseItmLeg( double price );
  void TakeProfits( double price );

private:
  double m_dblStrikeAtm;

  pPortfolio_t m_pPortfolio; // positions need to be associated with portfolio

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
