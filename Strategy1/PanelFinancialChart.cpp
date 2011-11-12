/************************************************************************
 * Copyright(c) 2011, One Unified. All rights reserved.                 *
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

#include "StdAfx.h"

#include "PanelFinancialChart.h"

//IMPLEMENT_DYNAMIC_CLASS( PanelFinancialChart, wxPanel )

PanelFinancialChart::PanelFinancialChart(void) {
  Init();
}

PanelFinancialChart::PanelFinancialChart( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {
  Init();
  Create(parent, id, pos, size, style);
}

PanelFinancialChart::~PanelFinancialChart(void) {

}

bool PanelFinancialChart::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {
    wxPanel::Create( parent, id, pos, size, style );

    CreateControls();
    if (GetSizer())     {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
    return true;
}

void PanelFinancialChart::Init() {
}

void PanelFinancialChart::CreateControls() {    

    PanelFinancialChart* itemPanel1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemPanel1->SetSizer(itemBoxSizer2);

    m_winChart = new wxWindow( itemPanel1, ID_CHART, wxDefaultPosition, wxSize(800, 450), wxSIMPLE_BORDER );
    itemBoxSizer2->Add(m_winChart, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

}

wxBitmap PanelFinancialChart::GetBitmapResource( const wxString& name ) {
    wxUnusedVar(name);
    return wxNullBitmap;
}

wxIcon PanelFinancialChart::GetIconResource( const wxString& name ) {
    wxUnusedVar(name);
    return wxNullIcon;
}
