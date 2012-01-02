/////////////////////////////////////////////////////////////////////////////
// Name:        PanelSimulationControl.cpp
// Purpose:     
// Author:      Raymond P. Burkholder
// Modified by: 
// Created:     06/11/2011 11:46:53
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

#include "PanelSimulationControl.h"

////@begin XPM images
////@end XPM images


/*
 * PanelSimulationControl type definition
 */

IMPLEMENT_DYNAMIC_CLASS( PanelSimulationControl, wxPanel )


/*
 * PanelSimulationControl event table definition
 */

BEGIN_EVENT_TABLE( PanelSimulationControl, wxPanel )

////@begin PanelSimulationControl event table entries
////@end PanelSimulationControl event table entries

END_EVENT_TABLE()


/*
 * PanelSimulationControl constructors
 */

PanelSimulationControl::PanelSimulationControl()
{
    Init();
}

PanelSimulationControl::PanelSimulationControl( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create(parent, id, pos, size, style);
}


/*
 * PanelSimulationControl creator
 */

bool PanelSimulationControl::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
////@begin PanelSimulationControl creation
    wxPanel::Create( parent, id, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end PanelSimulationControl creation
    return true;
}


/*
 * PanelSimulationControl destructor
 */

PanelSimulationControl::~PanelSimulationControl()
{
////@begin PanelSimulationControl destruction
////@end PanelSimulationControl destruction
}


/*
 * Member initialisation
 */

void PanelSimulationControl::Init()
{
////@begin PanelSimulationControl member initialisation
    m_txtInstrumentName = NULL;
    m_txtGroupDirectory = NULL;
    m_staticResult = NULL;
    m_gaugeProgress = NULL;
////@end PanelSimulationControl member initialisation
}


/*
 * Control creation for PanelSimulationControl
 */

void PanelSimulationControl::CreateControls()
{    
////@begin PanelSimulationControl content construction
    PanelSimulationControl* itemPanel1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemPanel1->SetSizer(itemBoxSizer2);

    m_txtInstrumentName = new wxTextCtrl( itemPanel1, ID_TEXT_INSTRUMENTNAME, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer2->Add(m_txtInstrumentName, 0, wxALIGN_LEFT|wxALL, 5);

    m_txtGroupDirectory = new wxTextCtrl( itemPanel1, ID_TEXT_GROUPDIRECTORY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer2->Add(m_txtGroupDirectory, 0, wxALIGN_LEFT|wxALL, 5);

    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer5, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxButton* itemButton6 = new wxButton( itemPanel1, ID_BTN_STARTSIM, _("Start"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer5->Add(itemButton6, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_staticResult = new wxStaticText( itemPanel1, ID_STATIC_RESULT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer5->Add(m_staticResult, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_gaugeProgress = new wxGauge( itemPanel1, ID_GAUGE_PROGRESS, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL );
    m_gaugeProgress->SetValue(1);
    itemBoxSizer2->Add(m_gaugeProgress, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxButton* itemButton9 = new wxButton( itemPanel1, ID_BTN_DRAWCHART, _("Draw Chart"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer2->Add(itemButton9, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

////@end PanelSimulationControl content construction
}


/*
 * Should we show tooltips?
 */

bool PanelSimulationControl::ShowToolTips()
{
    return true;
}

/*
 * Get bitmap resources
 */

wxBitmap PanelSimulationControl::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin PanelSimulationControl bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end PanelSimulationControl bitmap retrieval
}

/*
 * Get icon resources
 */

wxIcon PanelSimulationControl::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin PanelSimulationControl icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end PanelSimulationControl icon retrieval
}
