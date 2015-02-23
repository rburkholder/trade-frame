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

#include <wx/valgen.h>

#include "DialogNewPortfolio.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

IMPLEMENT_DYNAMIC_CLASS( DialogNewPortfolio, wxDialog )

DialogNewPortfolio::DialogNewPortfolio(void) {
  Init();
}

DialogNewPortfolio::DialogNewPortfolio( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style ) {
    Init();
    Create(parent, id, caption, pos, size, style);
}

DialogNewPortfolio::~DialogNewPortfolio(void) {
}

void DialogNewPortfolio::Init() {

    m_txtPortfolioId = NULL;
    m_txtDescription = NULL;
    m_btnOk = NULL;
    m_btnCancel = NULL;

}

void DialogNewPortfolio::SetDataExchange( DataExchange* pde ) {
  DialogBase::SetDataExchange( pde );
  if ( 0 != pde ) {
    m_txtPortfolioId->SetValidator( wxGenericValidator( &pde->sPortfolioId ) );
    m_txtDescription->SetValidator( wxGenericValidator( &pde->sDescription ) );
  }
  else {
  }
}

bool DialogNewPortfolio::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style ) {

    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    DialogBase::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if (GetSizer()) {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
    return true;

}

void DialogNewPortfolio::CreateControls() {  

    DialogNewPortfolio* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    wxFlexGridSizer* itemFlexGridSizer3 = new wxFlexGridSizer(0, 2, 0, 0);
    itemBoxSizer2->Add(itemFlexGridSizer3, 1, wxGROW|wxALL, 2);

    wxStaticText* itemStaticText4 = new wxStaticText( itemDialog1, ID_LblPortfolioId, _("Portfolio Name:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer3->Add(itemStaticText4, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_txtPortfolioId = new wxTextCtrl( itemDialog1, ID_TxtPortfolioId, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer3->Add(m_txtPortfolioId, 1, wxALIGN_LEFT|wxGROW|wxALL, 2);

    wxStaticText* itemStaticText6 = new wxStaticText( itemDialog1, ID_LblDescription, _("Description:"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
    itemFlexGridSizer3->Add(itemStaticText6, 0, wxALIGN_RIGHT|wxALIGN_TOP|wxALL, 5);

    m_txtDescription = new wxTextCtrl( itemDialog1, ID_TxtDescription, wxEmptyString, wxDefaultPosition, wxSize(200, -1), wxTE_MULTILINE );
    itemFlexGridSizer3->Add(m_txtDescription, 1, wxGROW|wxGROW|wxLEFT|wxRIGHT|wxBOTTOM, 2);

    itemFlexGridSizer3->AddGrowableRow(1);
    itemFlexGridSizer3->AddGrowableCol(1);

    wxBoxSizer* itemBoxSizer8 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer8, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    m_btnOk = new wxButton( itemDialog1, wxID_OK, _("OK"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer8->Add(m_btnOk, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_btnCancel = new wxButton( itemDialog1, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer8->Add(m_btnCancel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

}


} // namespace tf
} // namespace ou
