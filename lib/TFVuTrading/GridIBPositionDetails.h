/************************************************************************
 * Copyright(c) 2015, One Unified. All rights reserved.                 *
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
// Started December 30, 2015, 3:40 PM

#pragma once

#include <memory>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <wx/grid.h>

#include <TFInteractiveBrokers/IBTWS.h>

namespace ou { // One Unified
namespace tf { // TradeFrame

#define GRID_PositionDetails_STYLE wxTAB_TRAVERSAL
#define GRID_PositionDetails_TITLE _("Grid IB Position Details")
#define GRID_PositionDetails_IDNAME ID_GRID_IB_POSITION_DETAILS
#define GRID_PositionDetails_SIZE wxSize(400, 300)
#define GRID_PositionDetails_POSITION wxDefaultPosition

class IBPositionDetailEvent: public wxEvent {
public:
  IBPositionDetailEvent( wxEventType eventType, const ou::tf::ib::TWS::PositionDetail& pd )
  : wxEvent( 0, eventType ), m_pd( pd ) {}
  IBPositionDetailEvent( const IBPositionDetailEvent& event ): wxEvent( event ), m_pd( event.m_pd ) {}
  ~IBPositionDetailEvent( void ) {}
  IBPositionDetailEvent* Clone( void ) const { return new IBPositionDetailEvent( *this ); }
  const ou::tf::ib::TWS::PositionDetail& GetIBPositionDetail( void ) const { return m_pd; }
protected:
private:
  const ou::tf::ib::TWS::PositionDetail m_pd;
};

class GridIBPositionDetails_impl;  // Forward declaration

class GridIBPositionDetails: public wxGrid {
    friend class GridIBPositionDetails_impl;
    friend class boost::serialization::access;
public:
  GridIBPositionDetails(void);
  GridIBPositionDetails(
    wxWindow* parent, wxWindowID id = GRID_PositionDetails_IDNAME,
    const wxPoint& pos = GRID_PositionDetails_POSITION,
    const wxSize& size = GRID_PositionDetails_SIZE,
    long style = GRID_PositionDetails_STYLE,
    const wxString& = GRID_PositionDetails_TITLE );
  ~GridIBPositionDetails(void);

  bool Create( wxWindow* parent,
    wxWindowID id = GRID_PositionDetails_IDNAME,
    const wxPoint& pos = GRID_PositionDetails_POSITION,
    const wxSize& size = GRID_PositionDetails_SIZE,
    long style = GRID_PositionDetails_STYLE,
    const wxString& = GRID_PositionDetails_TITLE );

  void UpdatePositionDetailRow( const ou::tf::ib::TWS::PositionDetail& ad );

protected:
    void Init();
    void CreateControls();
private:

  enum { ID_Null=wxID_HIGHEST, ID_GRID_IB_POSITION_DETAILS
  };

  std::unique_ptr<GridIBPositionDetails_impl> m_pimpl;

  void HandleIBPositionDetail( IBPositionDetailEvent& event );

  void OnDestroy( wxWindowDestroyEvent& event );
  wxBitmap GetBitmapResource( const wxString& name );
  wxIcon GetIconResource( const wxString& name );
  static bool ShowToolTips() { return true; };

    template<class Archive>
    void serialize(Archive & ar, const unsigned int file_version);
};

} // namespace tf
} // namespace ou
