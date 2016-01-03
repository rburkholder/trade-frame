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

void PanelIBPositionDetails::UpdatePositionDetailRow( const ou::tf::IBTWS::PositionDetail& ad ) {
  m_pimpl->UpdatePositionDetailRow( ad );
}

wxBitmap PanelIBPositionDetails::GetBitmapResource( const wxString& name ) {
    wxUnusedVar(name);
    return wxNullBitmap;
}

wxIcon PanelIBPositionDetails::GetIconResource( const wxString& name ) {
    wxUnusedVar(name);
    return wxNullIcon;
}

} // namespace tf
} // namespace ou