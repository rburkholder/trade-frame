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

#include <wx/sizer.h>
#include <wx/icon.h>
//#include <wx/event.h>

#include "PanelIBAccountValues.h"
#include "PanelIBAccountValues_impl.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

wxDEFINE_EVENT( EVT_IBAccountValue, IBAccountValueEvent );

PanelIBAccountValues::PanelIBAccountValues() {
  Init();
}

PanelIBAccountValues::PanelIBAccountValues( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {
    Init();
    Create(parent, id, pos, size, style);
}

PanelIBAccountValues::~PanelIBAccountValues( void ) {
}

void PanelIBAccountValues::Init() {
  Bind( EVT_IBAccountValue, &PanelIBAccountValues::HandleIBAccountValue, this );
  m_pimpl.reset( new PanelIBAccountValues_impl( *this ) ); 
}

bool PanelIBAccountValues::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {
  
    wxPanel::Create( parent, id, pos, size, style );

    m_pimpl->CreateControls();
    if (GetSizer())     {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
    return true;
}

// need to cross a thread boundary here
// TODO: need to check that ad is valid through the event transition
void PanelIBAccountValues::UpdateAccountValueRow( const ou::tf::IBTWS::AccountValue& ad ) {
  auto p( new IBAccountValueEvent( EVT_IBAccountValue, ad ) );
  this->QueueEvent( p );
}

void PanelIBAccountValues::HandleIBAccountValue( IBAccountValueEvent& event ) {
  m_pimpl->UpdateAccountValueRow( event.GetIBAccountValue() );
}

wxBitmap PanelIBAccountValues::GetBitmapResource( const wxString& name ) {
    wxUnusedVar(name);
    return wxNullBitmap;
}

wxIcon PanelIBAccountValues::GetIconResource( const wxString& name ) {
    wxUnusedVar(name);
    return wxNullIcon;
}

template void PanelIBAccountValues::serialize<boost::archive::text_iarchive>(
    boost::archive::text_iarchive & ar, 
    const unsigned int file_version
);

template void PanelIBAccountValues::serialize<boost::archive::text_oarchive>(
    boost::archive::text_oarchive & ar, 
    const unsigned int file_version
);

} // namespace tf
} // namespace ou