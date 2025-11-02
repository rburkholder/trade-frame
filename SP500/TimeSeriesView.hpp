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
 * File:    TimeSeriesView.hpp
 * Author:  raymond@burkholder.net
 * Project: SP500
 * Created: November 1, 2025 19:12:28
 */

#pragma once

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <wx/grid.h>

#define SYMBOL_TIMESERIESVIEW_STYLE wxTAB_TRAVERSAL
#define SYMBOL_TIMESERIESVIEW_TITLE _("Time Series Viewer")
#define SYMBOL_TIMESERIESVIEW_IDNAME ID_TIMESERIESVIEW
#define SYMBOL_TIMESERIESVIEW_SIZE wxSize(-1, -1)
#define SYMBOL_OTIMESERIESVIEW_POSITION wxDefaultPosition

class TimeSeriesView
: public wxGrid
{
  friend class boost::serialization::access;
public:

  TimeSeriesView();
  TimeSeriesView(
    wxWindow* parent, wxWindowID id = ID_TIMSERIESVIEW,
    const wxPoint& pos = SYMBOL_OTIMESERIESVIEW_POSITION,
    const wxSize& size = SYMBOL_TIMESERIESVIEW_SIZE,
    long style = SYMBOL_TIMESERIESVIEW_STYLE,
    const wxString& name = SYMBOL_TIMESERIESVIEW_TITLE );

  virtual ~TimeSeriesView();

  bool Create( wxWindow* parent,
    wxWindowID id = ID_TIMSERIESVIEW,
    const wxPoint& pos = SYMBOL_OTIMESERIESVIEW_POSITION,
    const wxSize& size = SYMBOL_TIMESERIESVIEW_SIZE,
    long style = SYMBOL_TIMESERIESVIEW_STYLE,
    const wxString& name = SYMBOL_TIMESERIESVIEW_TITLE );

protected:
private:

  enum {
    ID_Null=wxID_HIGHEST, ID_TIMSERIESVIEW
  };

  enum EColId { dt, bid_vol, bid_prc, trd_vol, trd_prc, ask_vol, ask_prc, _col_id_count };

  void Init();
  void CreateControls();
  void OnDestroy( wxWindowDestroyEvent& event );

  wxBitmap GetBitmapResource( const wxString& name );
  wxIcon GetIconResource( const wxString& name );
  static bool ShowToolTips() { return true; };

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    ar & EColId::_col_id_count;
    for ( int ix = 0; ix < EColId::_col_id_count; ++ix ) {
      ar & GetColSize( ix );
    }
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    unsigned int nColumns;
    ar & nColumns;
    assert( EColId::_col_id_count == nColumns );
    unsigned int width;
    for ( int ix = 0; ix < EColId::_col_id_count; ++ix ) {
      ar & width;
      SetColSize( ix, width );
    }
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};

BOOST_CLASS_VERSION(TimeSeriesView, 1)