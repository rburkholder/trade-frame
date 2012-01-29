/////////////////////////////////////////////////////////////////////////////
// Name:        PanelOptionSelection.cpp
// Purpose:     
// Author:      Raymond P. Burkholder
// Modified by: 
// Created:     23/01/2012 21:24:46
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

#include "PanelOptionSelection.h"

////@begin XPM images
////@end XPM images


/*
 * PanelOptionSelection type definition
 */

IMPLEMENT_DYNAMIC_CLASS( PanelOptionSelection, wxPanel )


/*
 * PanelOptionSelection event table definition
 */

BEGIN_EVENT_TABLE( PanelOptionSelection, wxPanel )

////@begin PanelOptionSelection event table entries
////@end PanelOptionSelection event table entries

END_EVENT_TABLE()


/*
 * PanelOptionSelection constructors
 */

PanelOptionSelection::PanelOptionSelection()
{
    Init();
}

PanelOptionSelection::PanelOptionSelection( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create(parent, id, pos, size, style);
}


/*
 * PanelOptionSelection creator
 */

bool PanelOptionSelection::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
////@begin PanelOptionSelection creation
    wxPanel::Create( parent, id, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end PanelOptionSelection creation
    return true;
}


/*
 * PanelOptionSelection destructor
 */

PanelOptionSelection::~PanelOptionSelection()
{
////@begin PanelOptionSelection destruction
////@end PanelOptionSelection destruction
}


/*
 * Member initialisation
 */

void PanelOptionSelection::Init()
{
////@begin PanelOptionSelection member initialisation
    m_txtUnderlying = NULL;
    m_ctrlNearDate = NULL;
    m_ctrlFarDate = NULL;
    m_btnStart = NULL;
////@end PanelOptionSelection member initialisation
}


/*
 * Control creation for PanelOptionSelection
 */

void PanelOptionSelection::CreateControls()
{    
////@begin PanelOptionSelection content construction
    PanelOptionSelection* itemPanel1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemPanel1->SetSizer(itemBoxSizer2);

    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer3, 0, wxALIGN_RIGHT|wxALL, 5);

    wxStaticText* itemStaticText4 = new wxStaticText( itemPanel1, ID_LBL_Underlying, _("Underlying:"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
    itemBoxSizer3->Add(itemStaticText4, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_txtUnderlying = new wxTextCtrl( itemPanel1, ID_TXT_Underlying, _("GLD"), wxDefaultPosition, wxDefaultSize, 0 );
    m_txtUnderlying->SetMaxLength(10);
    itemBoxSizer3->Add(m_txtUnderlying, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer6 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer6, 0, wxALIGN_RIGHT|wxALL, 5);

    wxStaticText* itemStaticText7 = new wxStaticText( itemPanel1, ID_LBL_OPTIONNEARDATE, _("Near Date:"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
    itemBoxSizer6->Add(itemStaticText7, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_ctrlNearDate = new wxDatePickerCtrl( itemPanel1, ID_DATE_NearDate, wxDateTime(), wxDefaultPosition, wxDefaultSize, wxDP_DEFAULT );
    itemBoxSizer6->Add(m_ctrlNearDate, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer9 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer9, 0, wxALIGN_RIGHT|wxALL, 5);

    wxStaticText* itemStaticText10 = new wxStaticText( itemPanel1, ID_LBL_OPTIONFARDATE, _("Far Date:"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
    itemBoxSizer9->Add(itemStaticText10, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_ctrlFarDate = new wxDatePickerCtrl( itemPanel1, ID_DATE_FarDate, wxDateTime(), wxDefaultPosition, wxDefaultSize, wxDP_DEFAULT );
    itemBoxSizer9->Add(m_ctrlFarDate, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_btnStart = new wxButton( itemPanel1, ID_BTN_START, _("Start"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer2->Add(m_btnStart, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

////@end PanelOptionSelection content construction
}


/*
 * Should we show tooltips?
 */

bool PanelOptionSelection::ShowToolTips()
{
    return true;
}

/*
 * Get bitmap resources
 */

wxBitmap PanelOptionSelection::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin PanelOptionSelection bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end PanelOptionSelection bitmap retrieval
}

/*
 * Get icon resources
 */

wxIcon PanelOptionSelection::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin PanelOptionSelection icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end PanelOptionSelection icon retrieval
}
