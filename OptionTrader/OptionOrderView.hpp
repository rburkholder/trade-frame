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
 * File:    OptionOrderView.hpp
 * Author:  raymond@burkholder.net
 * Project: OptionTrader
 * Created: 2026/01/05 10:38:02
 */

// loosely based upon lib/TFVuTrading/GridOptionComboOrder.hpp

#pragma once

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <wx/grid.h>

#include <TFOptions/Option.h>

class wxMenu;

namespace ou { // One Unified
namespace tf { // TradeFrame

#define OPTIONORDERVIEW_STYLE wxWANTS_CHARS
#define OPTIONORDERVIEW_TITLE _("Option Order View")
#define OPTIONORDERVIEW_IDNAME ID_OPTIONORDERVIEW
#define OPTIONORDERVIEW_SIZE wxSize(-1, -1)
#define OPTIONORDERVIEW_POSITION wxDefaultPosition

class OptionOrderModel;  // Forward Declaration

class OptionOrderView
: public wxGrid
{
  friend class OptionOrderModel;
  friend class boost::serialization::access;
public:

  OptionOrderView();
  OptionOrderView(
    wxWindow* parent, wxWindowID id = OPTIONORDERVIEW_IDNAME,
    const wxPoint& pos = OPTIONORDERVIEW_POSITION,
    const wxSize& size = OPTIONORDERVIEW_SIZE,
    long style = OPTIONORDERVIEW_STYLE,
    const wxString& = OPTIONORDERVIEW_TITLE );
  virtual ~OptionOrderView();

  bool Create( wxWindow* parent,
    wxWindowID id = OPTIONORDERVIEW_IDNAME,
    const wxPoint& pos = OPTIONORDERVIEW_POSITION,
    const wxSize& size = OPTIONORDERVIEW_SIZE,
    long style = OPTIONORDERVIEW_STYLE,
    const wxString& = OPTIONORDERVIEW_TITLE );

  void Refresh();

  void ClearOrders();
  void PlaceComboOrder();

protected:

  void Init();
  void CreateControls();

private:

  enum {
    ID_Null=wxID_HIGHEST, ID_OPTIONORDERVIEW
  , ID_MENUITEM_PlaceOrder, ID_MENUITEM_DeleteOrder, ID_MENUITEM_ClearOrders
  };

  wxMenu* m_pMenuRightClick;

  void OnGridCellRightClick( wxGridEvent& );

  void OnDestroy( wxWindowDestroyEvent& event );

  int GetColumnCount() const;

  wxBitmap GetBitmapResource( const wxString& name );
  wxIcon GetIconResource( const wxString& name );
  static bool ShowToolTips() { return true; };

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    unsigned int n{};
    if ( GetTable() ) {
      n = GetColumnCount();
      ar & n;
      for ( unsigned int ix = 0; ix < n; ++ix ) {
        ar & GetColSize( ix );
      }
    }
    else {
      ar & n;
    }
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    unsigned int nColumns;
    unsigned int width;
    ar & nColumns;
    for ( unsigned int ix = 0; ix < nColumns; ++ix ) {
      ar & width;
      if ( GetTable() ) {
        SetColSize( ix, width );
      }
      else {
        // need to keep it for later
      }
    }
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};

} // namespace tf
} // namespace ou

BOOST_CLASS_VERSION(ou::tf::OptionOrderView, 1)
