/////////////////////////////////////////////////////////////////////////////
// Name:        PanelProviderControl.cpp
// Purpose:     
// Author:      Raymond P. Burkholder
// Modified by: 
// Created:     29/01/2012 14:37:58
// RCS-ID:      
// Copyright:   (c) 2011 One Unified
// Licence:     
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

////@begin includes
////@end includes

#include "PanelProviderControl.h"

////@begin XPM images
////@end XPM images


/*
 * PanelProviderControl type definition
 */

IMPLEMENT_DYNAMIC_CLASS( PanelProviderControl, wxPanel )


/*
 * PanelProviderControl event table definition
 */

BEGIN_EVENT_TABLE( PanelProviderControl, wxPanel )

////@begin PanelProviderControl event table entries
////@end PanelProviderControl event table entries

END_EVENT_TABLE()


/*
 * PanelProviderControl constructors
 */

PanelProviderControl::PanelProviderControl()
{
    Init();
}

PanelProviderControl::PanelProviderControl( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create(parent, id, pos, size, style);
}


/*
 * PanelProviderControl creator
 */

bool PanelProviderControl::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
////@begin PanelProviderControl creation
    wxPanel::Create( parent, id, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end PanelProviderControl creation
    return true;
}


/*
 * PanelProviderControl destructor
 */

PanelProviderControl::~PanelProviderControl()
{
////@begin PanelProviderControl destruction
////@end PanelProviderControl destruction
}


/*
 * Member initialisation
 */

void PanelProviderControl::Init()
{
////@begin PanelProviderControl member initialisation
    m_cbIQFeedD1 = NULL;
    m_cbIQFeedD2 = NULL;
    m_cbIQFeedX = NULL;
    m_btnIQFeed = NULL;
    m_cbIBD1 = NULL;
    m_cbIBD2 = NULL;
    m_cbIBX = NULL;
    m_btnIB = NULL;
    m_cbSimD1 = NULL;
    m_cbSimD2 = NULL;
    m_cbSimX = NULL;
    m_btnSimulator = NULL;
////@end PanelProviderControl member initialisation
}


/*
 * Control creation for PanelProviderControl
 */

void PanelProviderControl::CreateControls()
{    
////@begin PanelProviderControl content construction
    PanelProviderControl* itemPanel1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemPanel1->SetSizer(itemBoxSizer2);

    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer3, 0, wxALIGN_LEFT|wxALL, 5);

    m_cbIQFeedD1 = new wxCheckBox( itemPanel1, ID_CB_IQF_D1, _("D1"), wxDefaultPosition, wxDefaultSize, 0 );
    m_cbIQFeedD1->SetValue(true);
    itemBoxSizer3->Add(m_cbIQFeedD1, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    m_cbIQFeedD2 = new wxCheckBox( itemPanel1, ID_CB_IQF_D2, _("D2"), wxDefaultPosition, wxDefaultSize, 0 );
    m_cbIQFeedD2->SetValue(false);
    itemBoxSizer3->Add(m_cbIQFeedD2, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 5);

    m_cbIQFeedX = new wxCheckBox( itemPanel1, ID_CB_IQF_X, _("X"), wxDefaultPosition, wxDefaultSize, 0 );
    m_cbIQFeedX->SetValue(false);
    m_cbIQFeedX->Enable(false);
    itemBoxSizer3->Add(m_cbIQFeedX, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 5);

    m_btnIQFeed = new wxButton( itemPanel1, ID_BtnIQFeed, _("Turn On"), wxDefaultPosition, wxDefaultSize, 0 );
    m_btnIQFeed->Enable(false);
    itemBoxSizer3->Add(m_btnIQFeed, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText8 = new wxStaticText( itemPanel1, wxID_LblIQFeed, _("IQF"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer3->Add(itemStaticText8, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer9 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer9, 0, wxALIGN_LEFT|wxALL, 5);

    m_cbIBD1 = new wxCheckBox( itemPanel1, ID_CB_IB_D1, _("D1"), wxDefaultPosition, wxDefaultSize, 0 );
    m_cbIBD1->SetValue(false);
    itemBoxSizer9->Add(m_cbIBD1, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    m_cbIBD2 = new wxCheckBox( itemPanel1, ID_CB_IB_D2, _("D2"), wxDefaultPosition, wxDefaultSize, 0 );
    m_cbIBD2->SetValue(true);
    itemBoxSizer9->Add(m_cbIBD2, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 5);

    m_cbIBX = new wxCheckBox( itemPanel1, ID_CB_IB_X, _("X"), wxDefaultPosition, wxDefaultSize, 0 );
    m_cbIBX->SetValue(true);
    itemBoxSizer9->Add(m_cbIBX, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 5);

    m_btnIB = new wxButton( itemPanel1, ID_BtnInteractiveBrokers, _("Turn On"), wxDefaultPosition, wxDefaultSize, 0 );
    m_btnIB->Enable(false);
    itemBoxSizer9->Add(m_btnIB, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText14 = new wxStaticText( itemPanel1, wxID_LblInteractiveBrokers, _("IB"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer9->Add(itemStaticText14, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer15 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer15, 0, wxALIGN_LEFT|wxALL, 5);

    m_cbSimD1 = new wxCheckBox( itemPanel1, ID_CB_SIM_D1, _("D1"), wxDefaultPosition, wxDefaultSize, 0 );
    m_cbSimD1->SetValue(false);
    itemBoxSizer15->Add(m_cbSimD1, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    m_cbSimD2 = new wxCheckBox( itemPanel1, ID_CB_SIM_D2, _("D2"), wxDefaultPosition, wxDefaultSize, 0 );
    m_cbSimD2->SetValue(false);
    m_cbSimD2->Enable(false);
    itemBoxSizer15->Add(m_cbSimD2, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 5);

    m_cbSimX = new wxCheckBox( itemPanel1, ID_CB_SIM_X, _("X"), wxDefaultPosition, wxDefaultSize, 0 );
    m_cbSimX->SetValue(false);
    itemBoxSizer15->Add(m_cbSimX, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 5);

    m_btnSimulator = new wxButton( itemPanel1, ID_BtnSimulation, _("Turn On"), wxDefaultPosition, wxDefaultSize, 0 );
    m_btnSimulator->Enable(false);
    itemBoxSizer15->Add(m_btnSimulator, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText20 = new wxStaticText( itemPanel1, wxID_LblSimulation, _("Sim"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer15->Add(itemStaticText20, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

////@end PanelProviderControl content construction
}


/*
 * Should we show tooltips?
 */

bool PanelProviderControl::ShowToolTips()
{
    return true;
}

/*
 * Get bitmap resources
 */

wxBitmap PanelProviderControl::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin PanelProviderControl bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end PanelProviderControl bitmap retrieval
}

/*
 * Get icon resources
 */

wxIcon PanelProviderControl::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin PanelProviderControl icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end PanelProviderControl icon retrieval
}
