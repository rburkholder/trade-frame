/************************************************************************
 * Copyright(c) 2016, One Unified. All rights reserved.                 *
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
// Started January 3, 2016, 3:44 PM

#pragma once

#include <memory>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <wx/grid.h>

#include <TFInteractiveBrokers/IBTWS.h>

namespace ou { // One Unified
namespace tf { // TradeFrame

#define GRID_AccountValues_STYLE wxTAB_TRAVERSAL
#define GRID_AccountValues_TITLE _("Grid IB Account Values")
#define GRID_AccountValues_IDNAME ID_PANELACCOUNTVALUES
#define GRID_AccountValues_SIZE wxSize(400, 300)
#define GRID_AccountValues_POSITION wxDefaultPosition

class IBAccountValueEvent: public wxEvent {
public:
  IBAccountValueEvent( wxEventType eventType, const ou::tf::IBTWS::AccountValue& ad )
  : wxEvent( 0, eventType ), m_ad( ad ) {}
  IBAccountValueEvent( const IBAccountValueEvent& event ): wxEvent( event ), m_ad( event.m_ad ) {}
  ~IBAccountValueEvent( void ) {}
  IBAccountValueEvent* Clone( void ) const { return new IBAccountValueEvent( *this ); }
  const ou::tf::IBTWS::AccountValue& GetIBAccountValue( void ) const { return m_ad; }
protected:
private:
  const ou::tf::IBTWS::AccountValue m_ad;
};

class GridIBAccountValues_impl;  // Forward declaration

class GridIBAccountValues: public wxGrid {
    friend class GridIBAccountValues_impl;
    friend class boost::serialization::access;
public:
  GridIBAccountValues(void);
  GridIBAccountValues( 
    wxWindow* parent, wxWindowID id = GRID_AccountValues_IDNAME, 
    const wxPoint& pos = GRID_AccountValues_POSITION, 
    const wxSize& size = GRID_AccountValues_SIZE, 
    long style = GRID_AccountValues_STYLE,
    const wxString& = GRID_AccountValues_TITLE
    );
  ~GridIBAccountValues(void);

  bool Create( wxWindow* parent, 
    wxWindowID id = GRID_AccountValues_IDNAME, 
    const wxPoint& pos = GRID_AccountValues_POSITION, 
    const wxSize& size = GRID_AccountValues_SIZE, 
    long style = GRID_AccountValues_STYLE,
    const wxString& = GRID_AccountValues_TITLE
  );
  
  void UpdateAccountValueRow( const ou::tf::IBTWS::AccountValue& ad );

protected:
    void Init();
    void CreateControls();
private:
  
  enum { ID_Null=wxID_HIGHEST, ID_PANELACCOUNTVALUES, 
    ID_GRID_ACCOUNTVALUES
  };
  
  std::unique_ptr<GridIBAccountValues_impl> m_pimpl;
  
  void HandleIBAccountValue( IBAccountValueEvent& event );

  void OnDestroy( wxWindowDestroyEvent& event );
  wxBitmap GetBitmapResource( const wxString& name );
  wxIcon GetIconResource( const wxString& name );
  static bool ShowToolTips() { return true; };
  
  template<class Archive>
  void serialize(Archive & ar, const unsigned int file_version);
};

} // namespace tf
} // namespace ou
