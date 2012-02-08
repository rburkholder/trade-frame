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
 * PanelOptionsParameters type definition
 */

IMPLEMENT_DYNAMIC_CLASS( PanelOptionsParameters, wxPanel )


/*
 * PanelOptionsParameters event table definition
 */

BEGIN_EVENT_TABLE( PanelOptionsParameters, wxPanel )

////@begin PanelOptionsParameters event table entries
////@end PanelOptionsParameters event table entries

END_EVENT_TABLE()


/*
 * PanelOptionsParameters constructors
 */

PanelOptionsParameters::PanelOptionsParameters()
{
    Init();
}

PanelOptionsParameters::PanelOptionsParameters( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create(parent, id, pos, size, style);
}


/*
 * PanelOptionSelection creator
 */

bool PanelOptionsParameters::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
////@begin PanelOptionsParameters creation
    wxPanel::Create( parent, id, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end PanelOptionsParameters creation
    return true;
}


/*
 * PanelOptionsParameters destructor
 */

PanelOptionsParameters::~PanelOptionsParameters()
{
////@begin PanelOptionsParameters destruction
////@end PanelOptionsParameters destruction
}


/*
 * Member initialisation
 */

void PanelOptionsParameters::Init()
{
////@begin PanelOptionsParameters member initialisation
    m_txtUnderlying = NULL;
    m_ctrlNearDate = NULL;
    m_ctrlFarDate = NULL;
    m_btnStart = NULL;
    m_btnStop = NULL;
    m_btnSave = NULL;
////@end PanelOptionsParameters member initialisation
}


/*
 * Control creation for PanelOptionSelection
 */

void PanelOptionsParameters::CreateControls()
{    
////@begin PanelOptionsParameters content construction
    PanelOptionsParameters* itemPanel1 = this;

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

    wxBoxSizer* itemBoxSizer12 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer12, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    m_btnStart = new wxButton( itemPanel1, ID_BTN_START, _("Start"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer12->Add(m_btnStart, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_btnStop = new wxButton( itemPanel1, ID_BTN_STOP, _("Stop"), wxDefaultPosition, wxDefaultSize, 0 );
    m_btnStop->Enable(false);
    itemBoxSizer12->Add(m_btnStop, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer15 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer15, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    m_btnSave = new wxButton( itemPanel1, ID_BTN_SAVE, _("Save"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer15->Add(m_btnSave, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

////@end PanelOptionsParameters content construction
}


/*
 * Should we show tooltips?
 */

bool PanelOptionsParameters::ShowToolTips()
{
    return true;
}

/*
 * Get bitmap resources
 */

wxBitmap PanelOptionsParameters::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin PanelOptionsParameters bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end PanelOptionsParameters bitmap retrieval
}

/*
 * Get icon resources
 */

wxIcon PanelOptionsParameters::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin PanelOptionsParameters icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end PanelOptionsParameters icon retrieval
}
