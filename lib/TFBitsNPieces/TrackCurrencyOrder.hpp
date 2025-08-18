/************************************************************************
 * Copyright(c) 2024, One Unified. All rights reserved.                 *
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
 * File:    TrackCurrencyOrder.hpp
 * Author:  raymond@burkholder.net
 * Project: TFBitsNPieces
 * Created: April 28, 2024 10:39:16
 */

#pragma once

#include "TrackOrderBase.hpp"

namespace ou { // namespace oneunified
namespace tf { // namespace tradeframe

class TrackCurrencyOrder: public TrackOrderBase {
public:

  using fFillPrice_t = std::function<void(double,double)>; // exchange rate, commission

  using fTransferFunds_t = std::function<void(
    ou::tf::Currency::ECurrency, double debit,
    ou::tf::Currency::ECurrency, double credit,
    double commission
    )>;

  TrackCurrencyOrder();
  virtual ~TrackCurrencyOrder();

  void Set( fFillPrice_t&& f );
  void Set( fTransferFunds_t& );
  virtual void Set( pPosition_t, ou::ChartDataView&, int slot ) override;

protected:
private:

  fFillPrice_t m_fFillPrice;
  fTransferFunds_t m_fTransferFunds; // used to update results of currency transaction

  ou::tf::Currency::ECurrency m_curBase;
  ou::tf::Currency::ECurrency m_curQuote;

  virtual void HandleOrderFilled( const ou::tf::Order& ) override;

};

} // namespace tradeframe
} // namespace oneunified
