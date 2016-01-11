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

#include <wx/sizer.h>
#include <wx/icon.h>

#include "PanelIBPositionDetails.h"
#include "PanelIBPositionDetails_impl.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

wxDEFINE_EVENT( EVT_IBPositionDetail, IBPositionDetailEvent );

PanelIBPositionDetails::PanelIBPositionDetails() {
  Init();
}

PanelIBPositionDetails::PanelIBPositionDetails( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {
    Init();
    Create(parent, id, pos, size, style);
}

PanelIBPositionDetails::~PanelIBPositionDetails( void ) {
}

void PanelIBPositionDetails::Init() {
  m_pimpl.reset( new PanelIBPositionDetails_impl( *this ) ); 
  Bind( EVT_IBPositionDetail, &PanelIBPositionDetails::HandleIBPositionDetail, this );
}

bool PanelIBPositionDetails::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {
  
    wxPanel::Create( parent, id, pos, size, style );

    m_pimpl->CreateControls();
    if (GetSizer())     {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
    return true;
}

void PanelIBPositionDetails::UpdatePositionDetailRow( const ou::tf::IBTWS::PositionDetail& pd ) {
  auto p( new IBPositionDetailEvent( EVT_IBPositionDetail, pd ) );
  this->QueueEvent( p );
}

void PanelIBPositionDetails::HandleIBPositionDetail( IBPositionDetailEvent& event ) {
  m_pimpl->UpdatePositionDetailRow( event.GetIBPositionDetail() );
}

wxBitmap PanelIBPositionDetails::GetBitmapResource( const wxString& name ) {
    wxUnusedVar(name);
    return wxNullBitmap;
}

wxIcon PanelIBPositionDetails::GetIconResource( const wxString& name ) {
    wxUnusedVar(name);
    return wxNullIcon;
}

template void PanelIBPositionDetails::serialize<boost::archive::text_iarchive>(
    boost::archive::text_iarchive & ar, 
    const unsigned int file_version
);
template void PanelIBPositionDetails::serialize<boost::archive::text_oarchive>(
    boost::archive::text_oarchive & ar, 
    const unsigned int file_version
);


} // namespace tf
} // namespace ou