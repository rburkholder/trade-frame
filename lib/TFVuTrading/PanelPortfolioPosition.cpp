/************************************************************************
 * Copyright(c) 2013, One Unified. All rights reserved.                 *
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

#include "PanelPortfolioPosition.h"
#include "PanelPortfolioPosition_impl.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

PanelPortfolioPosition::PanelPortfolioPosition(void) {
  Init();
}

PanelPortfolioPosition::PanelPortfolioPosition( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {
  Init();
  Create(parent, id, pos, size, style);
}

PanelPortfolioPosition::~PanelPortfolioPosition(void) {
  //std::cout << "PanelPortfolioPosition deleted" << std::endl;
}

void PanelPortfolioPosition::Init() {
  m_pimpl.reset( new PanelPortfolioPosition_impl( *this ) ); 
}

bool PanelPortfolioPosition::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {

    wxPanel::Create( parent, id, pos, size, style );

    m_pimpl->CreateControls();
    if (GetSizer())     {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
    return true;
}

ou::tf::Portfolio::pPortfolio_t& PanelPortfolioPosition::GetPortfolio( void ) { 
  return m_pimpl->m_pPortfolio; 
}

void PanelPortfolioPosition::SetPortfolio( pPortfolio_t pPortfolio ) {
  m_pimpl->SetPortfolio( pPortfolio );
}

void PanelPortfolioPosition::AddPosition( pPosition_t pPosition ) {
  m_pimpl->AddPosition( pPosition );
}

void PanelPortfolioPosition::SaveColumnSizes( ou::tf::GridColumnSizer& gcs ) const {
  m_pimpl->SaveColumnSizes( gcs );
}

void PanelPortfolioPosition::SetColumnSizes( ou::tf::GridColumnSizer& gcs ) {
  m_pimpl->SetColumnSizes( gcs );
}
	
void PanelPortfolioPosition::UpdateGui( void ) {
  m_pimpl->UpdateGui();
}

wxBitmap PanelPortfolioPosition::GetBitmapResource( const wxString& name ) {
    wxUnusedVar(name);
    return wxNullBitmap;
}

wxIcon PanelPortfolioPosition::GetIconResource( const wxString& name ) {
    wxUnusedVar(name);
    return wxNullIcon;
}

} // namespace tf
} // namespace ou
