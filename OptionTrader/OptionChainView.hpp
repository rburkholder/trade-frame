/************************************************************************
 * Copyright(c) 2025, One Unified. All rights reserved.                 *
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
 * File:    OptionChainView.hpp
 * Author:  raymond@burkholder.net
 * Project: OptionTrader
 * Created: July 18, 2025 11:30:21
 */

#pragma once

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <wx/grid.h>

#include <TFOptions/Option.h>

#define SYMBOL_OPTIONCHAINVIEW_STYLE wxTAB_TRAVERSAL
#define SYMBOL_OPTIONCHAINVIEW_TITLE _("Option Chain")
#define SYMBOL_OPTIONCHAINVIEW_IDNAME ID_OPTIONCHAINVIEW
#define SYMBOL_OPTIONCHAINVIEW_SIZE wxSize(-1, -1)
#define SYMBOL_OPTIONCHAINVIEW_POSITION wxDefaultPosition

class wxMenu;

class OptionChainView
: public wxGrid
{
  friend class boost::serialization::access;
public:

  OptionChainView();
  OptionChainView(
    wxWindow* parent, wxWindowID id = ID_OPTIONCHAINVIEW,
    const wxPoint& pos = SYMBOL_OPTIONCHAINVIEW_POSITION,
    const wxSize& size = SYMBOL_OPTIONCHAINVIEW_SIZE,
    long style = SYMBOL_OPTIONCHAINVIEW_STYLE,
    const wxString& name = SYMBOL_OPTIONCHAINVIEW_TITLE );
  virtual ~OptionChainView();

  bool Create( wxWindow* parent,
    wxWindowID id = ID_OPTIONCHAINVIEW,
    const wxPoint& pos = SYMBOL_OPTIONCHAINVIEW_POSITION,
    const wxSize& size = SYMBOL_OPTIONCHAINVIEW_SIZE,
    long style = SYMBOL_OPTIONCHAINVIEW_STYLE,
    const wxString& name = SYMBOL_OPTIONCHAINVIEW_TITLE );

  using pOption_t = ou::tf::option::Option::pOption_t;
  using fAddOrder_t = std::function<void( pOption_t&, ou::tf::OrderSide::EOrderSide, int quantity )>;

  void SetAddOrder( fAddOrder_t&& );

  int GetFirstVisibleRow() const;
  int GetVisibleRowCount() const;

  void SetVisible( int ixRow );

protected:
private:

  enum {
    ID_Null=wxID_HIGHEST, ID_OPTIONCHAINVIEW
  };

  wxMenu* m_pMenuAssignWatch;

  fAddOrder_t m_fAddOrder;

  void Init();
  void CreateControls();
  void OnDestroy( wxWindowDestroyEvent& event );

  void OnGridCellLeftClick( wxGridEvent& );
  void OnGridCellRightClick( wxGridEvent& );
  void OnGridSelectCell( wxGridEvent& );

  int GetColumnCount() const;

  wxBitmap GetBitmapResource( const wxString& name );
  wxIcon GetIconResource( const wxString& name );
  static bool ShowToolTips() { return true; };

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    ar & GetColumnCount();
    for ( unsigned int ix = 0; ix < GetColumnCount(); ++ix ) {
      //ar & GetColSize( ix );
      ar & 50;
    }
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    unsigned int nColumns;
    unsigned int width;
    ar & nColumns;

    for ( unsigned int ix = 0; ix < nColumns; ++ix ) {
      ar & width;
      //SetColSize( ix, width );
    }
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};

BOOST_CLASS_VERSION(OptionChainView, 1)