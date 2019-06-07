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
 * File:    Combo.h
 * Author:  raymond@burkholder.net
 * Project: TFOptions
 * Created on June 7, 2019, 5:08 PM
 */

#ifndef COMBO_H
#define COMBO_H

#include <TFTrading/Portfolio.h>
#include <TFTrading/Position.h>

#include "Leg.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

class Combo {
public:

  using pPosition_t = ou::tf::Position::pPosition_t;
  using pPortfolio_t = ou::tf::Portfolio::pPortfolio_t;
  using pChartDataView_t = ou::ChartDataView::pChartDataView_t;

  enum class State { Initializing, Positions, Executing, Watching, Canceled, Closing };
  State m_state;

  Combo( );
  Combo( const Combo& rhs ) = delete;
  Combo& operator=( const Combo& rhs ) = delete;
  Combo( Combo&& rhs );
  virtual ~Combo( );

  void SetPortfolio( pPortfolio_t );
  pPortfolio_t GetPortfolio() { return m_pPortfolio; }

  virtual void AddPosition( pPosition_t, pChartDataView_t pChartData, ou::Colour::enumColour );

  void Tick( bool bInTrend, double dblPriceUnderlying, ptime dt );

  virtual void PlaceOrder( ou::tf::OrderSide::enumOrderSide ) = 0;

  double GetNet( double price );

  void CloseForProfits( double price );
  void TakeProfits( double price );
  bool CloseItmLeg( double price );
  void CloseExpiryItm( double price, const boost::gregorian::date date );


  void CancelOrders();
  void ClosePositions();

  bool AreOrdersActive() const;
  void SaveSeries( const std::string& sPrefix );


protected:

  pPortfolio_t m_pPortfolio; // positions need to be associated with portfolio

  using vLeg_t = std::vector<ou::tf::Leg>;
  vLeg_t m_vLeg;

private:

  void Update( bool bTrending, double dblPrice );

};

} // namespace option
} // namespace tf
} // namespace ou

#endif /* COMBO_H */

