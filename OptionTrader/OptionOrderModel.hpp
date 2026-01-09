/************************************************************************
 * Copyright(c) 2026, One Unified. All rights reserved.                 *
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
 * File:    OptionOrderModel.hpp
 * Author:  raymond@burkholder.net
 * Project: OptionTrader
 * Created: 2026/01/05 10:38:02
 */

// loosely based upon lib/TFVuTrading/GridOptionComboOrder_impl.hpp

#pragma once

#include <wx/grid.h>

#include <TFTrading/Watch.h>
#include <TFOptions/Option.h>

namespace ou { // One Unified
namespace tf { // TradeFrame

class OptionOrderModel_impl;

class OptionOrderModel
: public wxGridTableBase {
  friend OptionOrderModel_impl;
public:

  using pWatch_t = ou::tf::Watch::pWatch_t;
  using pOption_t = ou::tf::option::Option::pOption_t;

  OptionOrderModel();
  virtual ~OptionOrderModel();

  void Add( pWatch_t&, ou::tf::OrderSide::EOrderSide, int quantity ); // underlying
  void Add( pOption_t&, ou::tf::OrderSide::EOrderSide, int quantity ); // option

  using fOrderLeg_t = std::function<void(ou::tf::OrderSide::EOrderSide side, int quan, double price, const std::string& sIQFeedName)>;
  fOrderLeg_t FactoryAddComboOrderLeg();

  using fIterateLegs_t = std::function<void( fOrderLeg_t&& )>;
  using fGatherOrderLegs_t = std::function<void( fIterateLegs_t&& )>;
  void Set( fGatherOrderLegs_t&& );

protected:

private:

  using pOptionOrderModel_impl_t = std::unique_ptr<OptionOrderModel_impl>;
  pOptionOrderModel_impl_t m_pOptionOrderModel_impl;

  void CreateControls();
  void DestroyControls();

  void Refresh();

  void ClearRows();
  void PlaceComboOrder();

  virtual void SetView ( wxGrid *grid ) override;

  virtual int GetNumberRows() override;
  virtual int GetNumberCols() override;
  virtual bool IsEmptyCell( int row, int col ) override;

  virtual bool InsertRows( size_t pos = 0, size_t numRows = 1 ) override;
  virtual bool AppendRows( size_t numRows = 1 ) override;

  virtual wxString GetValue( int row, int col ) override;
  virtual void SetValue( int row, int col, const wxString &value ) override;

  virtual wxGridCellAttr* GetAttr ( int row, int col, wxGridCellAttr::wxAttrKind kind ) override;

  virtual wxString GetColLabelValue( int col ) override;

};

} // namespace tf
} // namespace ou
