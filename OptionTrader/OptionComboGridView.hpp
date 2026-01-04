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
 * File:    OptionComboGridView.hpp
 * Author:  raymond@burkholder.net
 * Project: OptionTrader
 * Created: December 28 2025 20:27:57
 */

#pragma once

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <wx/grid.h>

#define SYMBOL_OptionComboGridView_STYLE wxTAB_TRAVERSAL
#define SYMBOL_OptionComboGridView_TITLE _( "Option Combo" )
#define SYMBOL_OptionComboGridView_IDNAME ID_OptionComboGridView
#define SYMBOL_OptionComboGridView_SIZE wxDefaultSize
#define SYMBOL_OptionComboGridView_POSITION wxDefaultPosition

namespace ou { // One Unified
namespace tf { // TradeFrame

class OptionComboGridView
: public wxGrid
{
  friend class boost::serialization::access;
public:

  OptionComboGridView();
  OptionComboGridView(
    wxWindow* parent, wxWindowID id = SYMBOL_OptionComboGridView_IDNAME,
    const wxPoint& pos = SYMBOL_OptionComboGridView_POSITION,
    const wxSize& size = SYMBOL_OptionComboGridView_SIZE,
    long style = SYMBOL_OptionComboGridView_STYLE,
    const wxString& name = SYMBOL_OptionComboGridView_TITLE );
  virtual ~OptionComboGridView();

  bool Create( wxWindow* parent,
    wxWindowID id = SYMBOL_OptionComboGridView_IDNAME,
    const wxPoint& pos = SYMBOL_OptionComboGridView_POSITION,
    const wxSize& size = SYMBOL_OptionComboGridView_SIZE,
    long style = SYMBOL_OptionComboGridView_STYLE,
    const wxString& name = SYMBOL_OptionComboGridView_TITLE );

protected:
private:

  enum {
    ID_Null=wxID_HIGHEST
  , ID_OptionComboGridView
  };

  void Init();
  void CreateControls();
  void OnDestroy( wxWindowDestroyEvent& event );

  wxBitmap GetBitmapResource( const wxString& name );
  wxIcon GetIconResource( const wxString& name );
  static bool ShowToolTips() { return true; };

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};

} // namespace tf
} // namespace ou

BOOST_CLASS_VERSION(ou::tf::OptionComboGridView, 1)
