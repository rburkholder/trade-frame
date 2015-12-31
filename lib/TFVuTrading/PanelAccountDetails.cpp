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

#include "PanelAccountDetails.h"
#include "PanelAccountDetails_impl.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

PanelAccountDetails::PanelAccountDetails() {
  Init();
}

PanelAccountDetails::PanelAccountDetails( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {
    Init();
    Create(parent, id, pos, size, style);
}

PanelAccountDetails::~PanelAccountDetails( void ) {
}

void PanelAccountDetails::Init() {
  m_pimpl.reset( new PanelAccountDetails_impl( *this ) ); 
}

bool PanelAccountDetails::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {
  
    wxPanel::Create( parent, id, pos, size, style );

    m_pimpl->CreateControls();
    if (GetSizer())     {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
    return true;
}

void PanelAccountDetails::UpdateAccountDetailRow( const ou::tf::IBTWS::AccountDetails& ad ) {
  m_pimpl->UpdateAccountDetailRow( ad );
}

wxBitmap PanelAccountDetails::GetBitmapResource( const wxString& name ) {
    wxUnusedVar(name);
    return wxNullBitmap;
}

wxIcon PanelAccountDetails::GetIconResource( const wxString& name ) {
    wxUnusedVar(name);
    return wxNullIcon;
}

} // namespace tf
} // namespace ou