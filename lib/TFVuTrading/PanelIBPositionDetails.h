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

#include <wx/panel.h>

#include <TFInteractiveBrokers/IBTWS.h>

namespace ou { // One Unified
namespace tf { // TradeFrame

#define SYMBOL_PANEL_AccountDetails_STYLE wxTAB_TRAVERSAL
#define SYMBOL_PANEL_AccountDetails_TITLE _("Panel IB Account Details")
#define SYMBOL_PANEL_AccountDetails_IDNAME ID_PANELACCOUNTDETAILS
#define SYMBOL_PANEL_AccountDetails_SIZE wxSize(400, 300)
#define SYMBOL_PANEL_AccountDetails_POSITION wxDefaultPosition

class IBPositionDetailEvent: public wxEvent {
public:
  IBPositionDetailEvent( wxEventType eventType, const ou::tf::IBTWS::PositionDetail& pd )
  : wxEvent( 0, eventType ), m_pd( pd ) {}
  IBPositionDetailEvent( const IBPositionDetailEvent& event ): wxEvent( event ), m_pd( event.m_pd ) {}
  ~IBPositionDetailEvent( void ) {}
  IBPositionDetailEvent* Clone( void ) const { return new IBPositionDetailEvent( *this ); }
  const ou::tf::IBTWS::PositionDetail& GetIBPositionDetail( void ) const { return m_pd; }
protected:
private:
  const ou::tf::IBTWS::PositionDetail m_pd;
};

class PanelIBPositionDetails_impl;  // Forward declaration

class PanelIBPositionDetails: public wxPanel {
    friend class PanelIBPositionDetails_impl;
    friend class boost::serialization::access;
public:
  PanelIBPositionDetails(void);
  PanelIBPositionDetails( 
    wxWindow* parent, wxWindowID id = SYMBOL_PANEL_AccountDetails_IDNAME, 
    const wxPoint& pos = SYMBOL_PANEL_AccountDetails_POSITION, 
    const wxSize& size = SYMBOL_PANEL_AccountDetails_SIZE, 
    long style = SYMBOL_PANEL_AccountDetails_STYLE );
  ~PanelIBPositionDetails(void);

  bool Create( wxWindow* parent, 
    wxWindowID id = SYMBOL_PANEL_AccountDetails_IDNAME, 
    const wxPoint& pos = SYMBOL_PANEL_AccountDetails_POSITION, 
    const wxSize& size = SYMBOL_PANEL_AccountDetails_SIZE, 
    long style = SYMBOL_PANEL_AccountDetails_STYLE );
  
  void UpdatePositionDetailRow( const ou::tf::IBTWS::PositionDetail& ad );

protected:
    void Init();
private:
  
  enum { ID_Null=wxID_HIGHEST, ID_PANELACCOUNTDETAILS, 
    ID_GRID_ACCOUNTDETAILS
  };
  
  std::unique_ptr<PanelIBPositionDetails_impl> m_pimpl;
  
  void HandleIBPositionDetail( IBPositionDetailEvent& event );

  wxBitmap GetBitmapResource( const wxString& name );
  wxIcon GetIconResource( const wxString& name );
  static bool ShowToolTips() { return true; };
  
    template<class Archive>
    void serialize(Archive & ar, const unsigned int file_version);
};

} // namespace tf
} // namespace ou
