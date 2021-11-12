/************************************************************************
 * Copyright(c) 2021, One Unified. All rights reserved.                 *
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
 * File:    DataRow.h
 * Author:  raymond@burkholder.net
 * Project: TFVuTrading/MarketDepth
 * Created: November 11, 2021 09:08
 */

#pragma once

#include "RowElements.h"
#include "DataRowElement.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace l2 { // market depth

class DataRow {
public:

  DataRow( int ix, double price );
  ~DataRow();

  void SetRowElements( RowElements& );
  void Refresh();
  void DelRowElements();

protected:
private:

  int m_ix;
  double m_price;

  bool m_bChanged;

  // TODO: boost::fusion?  std::tuple?
  DataRowElement<double>         m_dreAcctPl;
  DataRowElement<unsigned int>   m_dreBidVolume;
  DataRowElement<double>         m_dreBid;
  DataRowElement<double>         m_drePrice;
  DataRowElement<double>         m_dreAsk;
  DataRowElement<unsigned int>   m_dreAskVolume;
  DataRowElement<unsigned int>   m_dreTicks;
  DataRowElement<unsigned int>   m_dreVolume;
  DataRowElementIndicatorStatic  m_dreIndicatorStatic;
  DataRowElementIndicatorDynamic m_dreIndicatorDynamic;

  //RowElements* m_pRowElements;  // shared_ptr ?

};

} // market depth
} // namespace tf
} // namespace ou
