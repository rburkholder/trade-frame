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

#include <wx/icon.h>
//#include <wx/event.h>

#include "GridIBAccountValues.h"
#include "GridIBAccountValues_impl.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

wxDEFINE_EVENT( EVT_IBAccountValue, IBAccountValueEvent );

GridIBAccountValues::GridIBAccountValues() {
  Init();
}

GridIBAccountValues::GridIBAccountValues( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& sTitle ) {
    Init();
    Create(parent, id, pos, size, style, sTitle );
}

GridIBAccountValues::~GridIBAccountValues( void ) {
  // this destructor is called prior to window destruction
  m_pimpl->DestroyControls();
}

void GridIBAccountValues::Init() {
}

bool GridIBAccountValues::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& sTitle ) {
  
  wxGrid::Create(parent, id, pos, size, style, sTitle );
  m_pimpl.reset( new GridIBAccountValues_impl( *this ) ); 

  CreateControls();
    
  return true;
}

void GridIBAccountValues::CreateControls() {    
  
  Bind( EVT_IBAccountValue, &GridIBAccountValues::HandleIBAccountValue, this );
  Bind( wxEVT_DESTROY, &GridIBAccountValues::OnDestroy, this );
  
  m_pimpl->CreateControls();
  
}

// need to cross a thread boundary here
// TODO: need to check that ad is valid through the event transition
void GridIBAccountValues::UpdateAccountValueRow( const ou::tf::IBTWS::AccountValue& ad ) {
  auto p( new IBAccountValueEvent( EVT_IBAccountValue, ad ) );
  this->QueueEvent( p );
}

void GridIBAccountValues::HandleIBAccountValue( IBAccountValueEvent& event ) {
  m_pimpl->UpdateAccountValueRow( event.GetIBAccountValue() );
}

void GridIBAccountValues::OnDestroy( wxWindowDestroyEvent& event ) {
  
  //m_pimpl->DestroyControls();
  //m_timerGuiRefresh.Stop();
  //Unbind( wxEVT_TIMER, &WinChartView::HandleGuiRefresh, this, m_timerGuiRefresh.GetId() );
  
  Unbind( wxEVT_DESTROY, &GridIBAccountValues::OnDestroy, this );
  
  //Unbind( wxEVT_PAINT, &WinChartView::HandlePaint, this );
  //Unbind( wxEVT_SIZE, &GridOptionDetails::HandleSize, this );
  
  //Unbind( wxEVT_MOTION, &WinChartView::HandleMouse, this );
  //Unbind( wxEVT_MOUSEWHEEL, &WinChartView::HandleMouseWheel, this );
  //Unbind( wxEVT_ENTER_WINDOW, &WinChartView::HandleMouseEnter, this );  
  //Unbind( wxEVT_LEAVE_WINDOW, &WinChartView::HandleMouseLeave, this );

  event.Skip();  // auto followed by Destroy();
}

wxBitmap GridIBAccountValues::GetBitmapResource( const wxString& name ) {
    wxUnusedVar(name);
    return wxNullBitmap;
}

wxIcon GridIBAccountValues::GetIconResource( const wxString& name ) {
    wxUnusedVar(name);
    return wxNullIcon;
}

template void GridIBAccountValues::serialize<boost::archive::text_iarchive>(
    boost::archive::text_iarchive & ar, 
    const unsigned int file_version
);

template void GridIBAccountValues::serialize<boost::archive::text_oarchive>(
    boost::archive::text_oarchive & ar, 
    const unsigned int file_version
);

} // namespace tf
} // namespace ou