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

#include <wx/icon.h>

#include "GridIBPositionDetails.h"
#include "GridIBPositionDetails_impl.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

wxDEFINE_EVENT( EVT_IBPositionDetail, IBPositionDetailEvent );

GridIBPositionDetails::GridIBPositionDetails() {
  Init();
}

GridIBPositionDetails::GridIBPositionDetails( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& sTitle ) {
    Init();
    Create(parent, id, pos, size, style, sTitle);
}

GridIBPositionDetails::~GridIBPositionDetails( void ) {
  // this destructor is called prior to window destruction
  m_pimpl->DestroyControls();
}

void GridIBPositionDetails::Init() {
  m_pimpl.reset( new GridIBPositionDetails_impl( *this ) ); 
}

bool GridIBPositionDetails::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& sTitle ) {
  
  wxGrid::Create(parent, id, pos, size, style, sTitle );

  m_pimpl.reset( new GridIBPositionDetails_impl( *this ) ); 
  CreateControls();

  return true;
}

void GridIBPositionDetails::CreateControls() {    
  
  Bind( EVT_IBPositionDetail, &GridIBPositionDetails::HandleIBPositionDetail, this );
  Bind( wxEVT_DESTROY, &GridIBPositionDetails::OnDestroy, this );
  
  m_pimpl->CreateControls();
  
}

void GridIBPositionDetails::UpdatePositionDetailRow( const ou::tf::IBTWS::PositionDetail& pd ) {
  auto p( new IBPositionDetailEvent( EVT_IBPositionDetail, pd ) );
  this->QueueEvent( p );
}

void GridIBPositionDetails::HandleIBPositionDetail( IBPositionDetailEvent& event ) {
  m_pimpl->UpdatePositionDetailRow( event.GetIBPositionDetail() );
}

wxBitmap GridIBPositionDetails::GetBitmapResource( const wxString& name ) {
    wxUnusedVar(name);
    return wxNullBitmap;
}

void GridIBPositionDetails::OnDestroy( wxWindowDestroyEvent& event ) {
  
  //m_pimpl->DestroyControls();
  //m_timerGuiRefresh.Stop();
  //Unbind( wxEVT_TIMER, &WinChartView::HandleGuiRefresh, this, m_timerGuiRefresh.GetId() );
  
  Unbind( wxEVT_DESTROY, &GridIBPositionDetails::OnDestroy, this );
  
  //Unbind( wxEVT_PAINT, &WinChartView::HandlePaint, this );
  //Unbind( wxEVT_SIZE, &GridOptionDetails::HandleSize, this );
  
  //Unbind( wxEVT_MOTION, &WinChartView::HandleMouse, this );
  //Unbind( wxEVT_MOUSEWHEEL, &WinChartView::HandleMouseWheel, this );
  //Unbind( wxEVT_ENTER_WINDOW, &WinChartView::HandleMouseEnter, this );  
  //Unbind( wxEVT_LEAVE_WINDOW, &WinChartView::HandleMouseLeave, this );

  event.Skip();  // auto followed by Destroy();
}

wxIcon GridIBPositionDetails::GetIconResource( const wxString& name ) {
    wxUnusedVar(name);
    return wxNullIcon;
}

template void GridIBPositionDetails::serialize<boost::archive::text_iarchive>(
    boost::archive::text_iarchive & ar, 
    const unsigned int file_version
);
template void GridIBPositionDetails::serialize<boost::archive::text_oarchive>(
    boost::archive::text_oarchive & ar, 
    const unsigned int file_version
);


} // namespace tf
} // namespace ou