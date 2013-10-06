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

#include "StdAfx.h"

#include "PanelChartHdf5.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

PanelChartHdf5::PanelChartHdf5(void) {
  Init();
}

PanelChartHdf5::PanelChartHdf5( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {
  Init();
  Create(parent, id, pos, size, style);
}

PanelChartHdf5::~PanelChartHdf5(void) {
}

void PanelChartHdf5::Init() {
}

bool PanelChartHdf5::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {
    wxPanel::Create( parent, id, pos, size, style );

    CreateControls();
    if (GetSizer())     {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
    return true;
}

void PanelChartHdf5::CreateControls() {    

    PanelChartHdf5* itemPanel1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemPanel1->SetSizer(itemBoxSizer2);
    /*
    m_txtLogging = new wxTextCtrl( itemPanel1, ID_TEXTLOGGING, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY );
    //m_txtLogging = new wxTextCtrl( itemPanel1, ID_TEXTLOGGING, wxEmptyString, wxDefaultPosition, wxSize(400, 300), wxTE_MULTILINE|wxTE_READONLY );
    itemBoxSizer2->Add(m_txtLogging, 1, wxALL | wxEXPAND|wxALIGN_LEFT|wxALIGN_RIGHT|wxALIGN_TOP|wxALIGN_BOTTOM, 5);


    Bind( EVT_ConsoleString, &PanelLogging::HandleConsoleLine1, this );
    m_csb.SetOnEmitString( MakeDelegate( this, &PanelLogging::HandleConsoleLine0 ) );
    */
}


wxBitmap PanelChartHdf5::GetBitmapResource( const wxString& name ) {
    wxUnusedVar(name);
    return wxNullBitmap;
}

wxIcon PanelChartHdf5::GetIconResource( const wxString& name ) {
    wxUnusedVar(name);
    return wxNullIcon;
}

} // namespace tf
} // namespace ou
