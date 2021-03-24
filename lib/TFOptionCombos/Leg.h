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
 * Project: TFOptionCombos
 * Created on May 25, 2019, 4:46 PM
 */

#ifndef LEG_H
#define LEG_H

#include <OUCharting/ChartDataView.h>
#include <OUCharting/ChartEntryIndicator.h>

#include <TFTrading/Position.h>
#include <TFTrading/MonitorOrder.h>

// TODO: may need option version inheritance
#include <TFOptions/Option.h>

#include "LegNote.h"

namespace ou {
namespace tf {

class Leg {
public:

  using pPosition_t = Position::pPosition_t;
  using pOption_t = ou::tf::option::Option::pOption_t;
  using pChartDataView_t = ou::ChartDataView::pChartDataView_t;

  Leg();
  Leg( Leg&& rhs );
  Leg( const Leg& rhs ) = delete;
  Leg( pPosition_t pPosition );
  Leg& operator=( const Leg&& rhs );
  virtual ~Leg();

  const option::LegNote::values_t& SetPosition( pPosition_t pPosition );
  pPosition_t GetPosition() const;

  void Tick( ptime dt );

  void PlaceOrder( ou::tf::OrderSide::enumOrderSide, boost::uint32_t nOrderQuantity );
  void CancelOrder();
  void ClosePosition();
  bool IsActive() const;
  bool IsOrderActive() const;

  void SaveSeries( const std::string& sPrefix );

  void SetChartData( pChartDataView_t pChartData, ou::Colour::enumColour );
  void DelChartData();

  bool CloseItm( const double price );
  bool CloseItmForProfit( const double price );
  void CloseExpiryItm( const boost::gregorian::date date, const double price );
  void CloseExpiryOtm( const boost::gregorian::date date, const double price );

  double GetNet( double price ) const;
  double ConstructedValue() const;

  const option::LegNote& GetLegNote() const { return m_legNote; }

private:

  bool m_bOption;  // only set upon assignment of appropriate position

  pPosition_t m_pPosition;
  option::LegNote m_legNote;

  ou::tf::MonitorOrder m_monitor;

  pChartDataView_t m_pChartDataView;

  ou::ChartEntryIndicator m_ceProfitLoss;

  ou::ChartEntryIndicator m_ceImpliedVolatility;
  ou::ChartEntryIndicator m_ceDelta;
  ou::ChartEntryIndicator m_ceGamma;
  ou::ChartEntryIndicator m_ceVega;
  ou::ChartEntryIndicator m_ceTheta;

};

} // namespace ou
} // namespace tf

#endif /* LEG_H */

