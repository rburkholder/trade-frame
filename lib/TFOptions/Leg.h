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
 * File:    Leg.h
 * Author:  raymond@burkholder.net
 * Project: TFBitsNPieces
 * Created on May 25, 2019, 4:46 PM
 */

#ifndef LEG_H
#define LEG_H

#include <OUCharting/ChartDataView.h>
#include <OUCharting/ChartEntryIndicator.h>

#include <TFTrading/Position.h>
#include <TFTrading/MonitorOrder.h>

#include <TFOptions/Option.h>

namespace ou {
namespace tf {

class Leg {
public:

  using pPosition_t = Position::pPosition_t;
  using pOption_t = ou::tf::option::Option::pOption_t;
  using pChartDataView_t = ou::ChartDataView::pChartDataView_t;

  Leg();
  Leg( pPosition_t pPosition );
  Leg( const Leg& rhs ) = delete;
  Leg& operator=( const Leg& rhs ) = delete;
  Leg( const Leg&& rhs );

  void SetPosition( pPosition_t pPosition );
  pPosition_t GetPosition();

  void Tick( ptime dt );

  void PlaceOrder( ou::tf::OrderSide::enumOrderSide, boost::uint32_t nOrderQuantity );
  void CancelOrder();
  void ClosePosition();
  bool IsActive() const;
  bool IsOrderActive() const;

  void SaveSeries( const std::string& sPrefix );

  void SetColour( ou::Colour::enumColour colour );

  void AddChartData( pChartDataView_t pChartData );

  bool CloseItm( const double price );
  bool CloseItmForProfit( const double price );
  void CloseExpiryItm( const boost::gregorian::date date, const double price );
  void CloseExpiryOtm( const boost::gregorian::date date, const double price );

  double GetNet( double price );
  double ConstructedValue() const;

private:
  bool m_bOption;
  pPosition_t m_pPosition;
  ou::tf::MonitorOrder m_monitor;
  ou::ChartEntryIndicator m_ceProfitLoss;

  ou::ChartEntryIndicator m_ceImpliedVolatility;
  ou::ChartEntryIndicator m_ceDelta;
  ou::ChartEntryIndicator m_ceGamma;
  ou::ChartEntryIndicator m_ceVega;
  ou::ChartEntryIndicator m_ceTheta;

  void Init();
};

} // namespace ou
} // namespace tf

#endif /* LEG_H */

