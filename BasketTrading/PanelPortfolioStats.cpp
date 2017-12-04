/************************************************************************
 * Copyright(c) 2012, One Unified. All rights reserved.                 *
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

#include "PanelPortfolioStats.h"



PanelPortfolioStats::PanelPortfolioStats(void) {
  Init();
}

PanelPortfolioStats::PanelPortfolioStats(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style) {
  Init();
  Create(parent, id, pos, size, style);
}

PanelPortfolioStats::~PanelPortfolioStats(void) {
}

bool PanelPortfolioStats::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {
  wxPanel::Create( parent, id, pos, size, style );
  CreateControls();
  if (GetSizer())
  {
      GetSizer()->SetSizeHints(this);
  }
//  Centre();
  return true;
}

void PanelPortfolioStats::Init( void ) {
    m_txtPortfolioLow = NULL;
    m_txtPortfolioCurrent = NULL;
    m_txtPortfolioHigh = NULL;
}

void PanelPortfolioStats::CreateControls() {    
    PanelPortfolioStats* itemPanel1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
    itemPanel1->SetSizer(itemBoxSizer2);

    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer3, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxStaticText* itemStaticText4 = new wxStaticText( itemPanel1, wxID_STATIC, _("Portfolio Profit:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer3->Add(itemStaticText4, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer3->Add(itemBoxSizer5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxStaticText* itemStaticText6 = new wxStaticText( itemPanel1, wxID_STATIC, _("Low"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer5->Add(itemStaticText6, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_txtPortfolioLow = new wxTextCtrl( itemPanel1, ID_TxtPortfolioLow, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer5->Add(m_txtPortfolioLow, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    wxBoxSizer* itemBoxSizer8 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer3->Add(itemBoxSizer8, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxStaticText* itemStaticText9 = new wxStaticText( itemPanel1, wxID_STATIC, _("Current"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer8->Add(itemStaticText9, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_txtPortfolioCurrent = new wxTextCtrl( itemPanel1, ID_TxtPortfolioCurrent, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer8->Add(m_txtPortfolioCurrent, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    wxBoxSizer* itemBoxSizer11 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer3->Add(itemBoxSizer11, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxStaticText* itemStaticText12 = new wxStaticText( itemPanel1, wxID_STATIC, _("High"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer11->Add(itemStaticText12, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_txtPortfolioHigh = new wxTextCtrl( itemPanel1, ID_TxtPortfolioHigh, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer11->Add(m_txtPortfolioHigh, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

}

void PanelPortfolioStats::SetStats( const std::string& sMin, const std::string& sCurrent, const std::string& sMax ) {
  m_txtPortfolioLow->SetValue( sMin );
  m_txtPortfolioCurrent->SetValue( sCurrent );
  m_txtPortfolioHigh->SetValue( sMax );
}