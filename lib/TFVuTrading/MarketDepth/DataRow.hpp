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

#include "WinRow.hpp"

#include "DataRowElement.hpp"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace l2 { // market depth

class DataRow {
public:

  DataRow( int ix, double price );
  ~DataRow();

  void SetRowElements( WinRow& );
  void DelRowElements();

  void Refresh();

  // doesn't work - cna't do partial specialization, try fusion?
  //template<typename T>
  //void Set(WinRow::EField field, T t ) {}
  //template <> void Set<double>( WinRow::EField field, double value ) {
  //  m_drePrice.Set( value );
  //}

  void SetPrice( double price ) { m_drePrice.Set( price ); }  // specialize in DataRowElement to do the highlight on and off

  unsigned int GetTicks() const { return m_dreTicks.Get(); }
  void SetTicks( unsigned int count ) { m_dreTicks.Set( count ); }

  unsigned int GetVolume() const { return m_dreVolume.Get(); }
  void SetVolume( unsigned int count ) { m_dreVolume.Set( count ); }

  void SetAskVolume( unsigned int nVolume ) { m_dreAskSize.Set( nVolume ); }
  void SetBidVolume( unsigned int nVolume ) { m_dreBidSize.Set( nVolume ); }

protected:
private:

  int m_ix;
  double m_price;

  bool m_bChanged;

  // TODO: boost::fusion?  std::tuple?
  //DataRowElement<double>         m_dreAcctPl;
  DataRowElement<unsigned int>   m_dreBidSize;
  DataRowElement<double>         m_drePrice;
  DataRowElement<unsigned int>   m_dreAskSize;
  DataRowElement<unsigned int>   m_dreTicks;
  DataRowElement<unsigned int>   m_dreVolume;
  DataRowElementIndicatorStatic  m_dreIndicatorStatic;
  DataRowElementIndicatorDynamic m_dreIndicatorDynamic;

  //RowElements* m_pRowElements;  // shared_ptr ?

};

} // market depth
} // namespace tf
} // namespace ou
