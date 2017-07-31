/************************************************************************
 * Copyright(c) 2014, One Unified. All rights reserved.                 *
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

#include "stdafx.h"

#include "GridOptionDetails.h"
#include "GridOptionDetails_impl.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

GridOptionDetails::GridOptionDetails(void) {
  Init();
}

GridOptionDetails::GridOptionDetails( 
  wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& sTitle ) {
  Init();
  Create(parent, id, pos, size, style, sTitle );
}

GridOptionDetails::~GridOptionDetails(void) {
  // this destructor is called prior to window destruction (because of unique_ptr?
}

void GridOptionDetails::Init( void ) {
}

bool GridOptionDetails::Create( 
  wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name ) {
  
  wxGrid::Create(parent, id, pos, size, style, name );
  m_pimpl.reset( new GridOptionDetails_impl( *this ) ); 
  
  CreateControls();
  
  return true;
}

void GridOptionDetails::CreateControls() {    
  
  //Bind( wxEVT_CLOSE_WINDOW, &GridOptionDetails::OnClose, this );  // not called for child windows
  Bind( wxEVT_DESTROY, &GridOptionDetails::OnDestroy, this );
  
  //Bind( wxEVT_PAINT, &WinChartView::HandlePaint, this );
  //Bind( wxEVT_SIZE, &GridOptionDetails::HandleSize, this );
  
  //Bind( wxEVT_MOTION, &WinChartView::HandleMouse, this );
  //Bind( wxEVT_MOUSEWHEEL, &WinChartView::HandleMouseWheel, this );
  //Bind( wxEVT_ENTER_WINDOW, &WinChartView::HandleMouseEnter, this );  
  //Bind( wxEVT_LEAVE_WINDOW, &WinChartView::HandleMouseLeave, this );

  //Bind( EVENT_DRAW_CHART, &WinChartView::HandleGuiDrawChart, this );

  // this GuiRefresh initialization should come after all else
  //m_timerGuiRefresh.SetOwner( this );
  //Bind( wxEVT_TIMER, &GridOptionDetails::HandleGuiRefresh, this, m_timerGuiRefresh.GetId() );
  //m_timerGuiRefresh.Start( 250 );

  m_pimpl->CreateControls();
  
}

void GridOptionDetails::Add( double strike, ou::tf::OptionSide::enumOptionSide side, const std::string& sSymbol ) {
  m_pimpl->Add( strike, side, sSymbol );
}

void GridOptionDetails::SetSelected( double strike, bool bSelected) {
  m_pimpl->SetSelected( strike, bSelected );
}

template void GridOptionDetails::serialize<boost::archive::text_iarchive>(
    boost::archive::text_iarchive & ar, 
    const unsigned int file_version
);

template void GridOptionDetails::serialize<boost::archive::text_oarchive>(
    boost::archive::text_oarchive & ar, 
    const unsigned int file_version
);

void GridOptionDetails::HandleSize( wxSizeEvent& event ) { 
}

void GridOptionDetails::OnDestroy( wxWindowDestroyEvent& event ) {
  
  //m_pimpl->DestroyControls();  // performed in the destructor
  //m_timerGuiRefresh.Stop();
  //Unbind( wxEVT_TIMER, &WinChartView::HandleGuiRefresh, this, m_timerGuiRefresh.GetId() );
  
  m_pimpl->DestroyControls();
  
  Unbind( wxEVT_DESTROY, &GridOptionDetails::OnDestroy, this );
  
  //Unbind( wxEVT_PAINT, &WinChartView::HandlePaint, this );
  //Unbind( wxEVT_SIZE, &GridOptionDetails::HandleSize, this );
  
  //Unbind( wxEVT_MOTION, &WinChartView::HandleMouse, this );
  //Unbind( wxEVT_MOUSEWHEEL, &WinChartView::HandleMouseWheel, this );
  //Unbind( wxEVT_ENTER_WINDOW, &WinChartView::HandleMouseEnter, this );  
  //Unbind( wxEVT_LEAVE_WINDOW, &WinChartView::HandleMouseLeave, this );

  event.Skip();  // auto followed by Destroy();
}

wxBitmap GridOptionDetails::GetBitmapResource( const wxString& name ) {
    wxUnusedVar(name);
    return wxNullBitmap;
}

wxIcon GridOptionDetails::GetIconResource( const wxString& name ) {
    wxUnusedVar(name);
    return wxNullIcon;
}

} // namespace tf
} // namespace ou