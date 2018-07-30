/************************************************************************
 * Copyright(c) 2018, One Unified. All rights reserved.                 *
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

#include "PanelOptionCombo.h"
#include "PanelOptionCombo_impl.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

PanelOptionCombo::PanelOptionCombo(void) {
  Init();
}

PanelOptionCombo::PanelOptionCombo( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {
  Init();
  Create(parent, id, pos, size, style);
}

PanelOptionCombo::~PanelOptionCombo(void) {
  //std::cout << "PanelOptionCombo deleted" << std::endl;
}

void PanelOptionCombo::Init() {
  m_pimpl.reset( new PanelOptionCombo_impl( *this ) ); 
}

bool PanelOptionCombo::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {

    wxPanel::Create( parent, id, pos, size, style );

    m_pimpl->CreateControls();
    if (GetSizer()) {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
    return true;
}

ou::tf::Portfolio::pPortfolio_t& PanelOptionCombo::GetPortfolio( void ) { 
  return m_pimpl->m_pPortfolio; 
}

void PanelOptionCombo::SetPortfolio( pPortfolio_t pPortfolio ) {
  m_pimpl->SetPortfolio( pPortfolio );
}

void PanelOptionCombo::AddPosition( pPosition_t pPosition ) {
  m_pimpl->AddPosition( pPosition );
}

void PanelOptionCombo::UpdateGui( void ) {
  m_pimpl->UpdateGui();
}

wxBitmap PanelOptionCombo::GetBitmapResource( const wxString& name ) {
    wxUnusedVar(name);
    return wxNullBitmap;
}

wxIcon PanelOptionCombo::GetIconResource( const wxString& name ) {
    wxUnusedVar(name);
    return wxNullIcon;
}

} // namespace tf
} // namespace ou
