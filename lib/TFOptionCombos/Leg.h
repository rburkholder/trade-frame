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
  Leg& operator=( Leg&& rhs );
  virtual ~Leg();

  const option::LegNote::values_t& SetPosition( pPosition_t pPosition );
  pPosition_t GetPosition() const;

  void Tick( ptime dt, double price );

  bool IsActive() const;

  void SaveSeries( const std::string& sPrefix );

  void SetChartData( pChartDataView_t pChartData, ou::Colour::EColour );
  void DelChartData();

  double GetNet( double price ) const;
  double ConstructedValue() const;
  void NetGreeks( double& delta, double& gamma ) const;

  const option::LegNote& GetLegNote() const { return m_legNote; }

private:

  bool m_bOption;  // only set upon assignment of appropriate position

  pPosition_t m_pPosition;
  option::LegNote m_legNote;

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

