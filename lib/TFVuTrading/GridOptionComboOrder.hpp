/************************************************************************
 * Copyright(c) 2023, One Unified. All rights reserved.                 *
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
 * File:    GridOptionComboOrder.hpp
 * Author:  raymond@burkholder.net
 * Project: TFVuTrading
 * Created: 2023/04/23 17:28:15
 */

#pragma once

#include <memory>
#include <functional>

#include <wx/grid.h>

#include <TFVuTrading/DragDropInstrument.h>

namespace ou { // One Unified
namespace tf { // TradeFrame

#define GRID_OPTIONORDER_STYLE wxWANTS_CHARS
#define GRID_OPTIONORDER_TITLE _("Grid Option Order")
#define GRID_OPTIONORDER_IDNAME ID_GRID_OPTIONORDER
#define GRID_OPTIONORDER_SIZE wxSize(-1, -1)
#define GRID_OPTIONORDER_POSITION wxDefaultPosition

class GridOptionComboOrder_impl;  // Forward Declaration

class GridOptionComboOrder: public wxGrid {
  friend GridOptionComboOrder_impl;
public:

  GridOptionComboOrder();
  GridOptionComboOrder(
    wxWindow* parent, wxWindowID id = GRID_OPTIONORDER_IDNAME,
    const wxPoint& pos = GRID_OPTIONORDER_POSITION,
    const wxSize& size = GRID_OPTIONORDER_SIZE,
    long style = GRID_OPTIONORDER_STYLE,
    const wxString& = GRID_OPTIONORDER_TITLE );
  virtual ~GridOptionComboOrder();

  bool Create( wxWindow* parent,
    wxWindowID id = GRID_OPTIONORDER_IDNAME,
    const wxPoint& pos = GRID_OPTIONORDER_POSITION,
    const wxSize& size = GRID_OPTIONORDER_SIZE,
    long style = GRID_OPTIONORDER_STYLE,
    const wxString& = GRID_OPTIONORDER_TITLE );

  using fAddComboOrder_t = std::function<void(ou::tf::OrderSide::EOrderSide side, int quan, double price, const std::string& sName)>;
  fAddComboOrder_t FunctionAddComboOrder();

  void Refresh();

protected:

  void Init();
  void CreateControls();

private:

  enum {
    ID_Null=wxID_HIGHEST, ID_GRID_OPTIONORDER
  };

  std::unique_ptr<GridOptionComboOrder_impl> m_pimpl;

  void OnDestroy( wxWindowDestroyEvent& event );

  wxBitmap GetBitmapResource( const wxString& name );
  wxIcon GetIconResource( const wxString& name );
  static bool ShowToolTips() { return true; };

};

} // namespace tf
} // namespace ou
