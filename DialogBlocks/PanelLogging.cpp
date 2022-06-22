/////////////////////////////////////////////////////////////////////////////
// Name:        PanelLogging.cpp
// Purpose:     
// Author:      Raymond P. Burkholder
// Modified by: 
// Created:     04/03/2012 10:26:41
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

#include "PanelLogging.h"

////@begin XPM images
////@end XPM images


/*
 * PanelLogging type definition
 */

IMPLEMENT_DYNAMIC_CLASS( PanelLogging, wxPanel )


/*
 * PanelLogging event table definition
 */

BEGIN_EVENT_TABLE( PanelLogging, wxPanel )

////@begin PanelLogging event table entries
////@end PanelLogging event table entries

END_EVENT_TABLE()


/*
 * PanelLogging constructors
 */

PanelLogging::PanelLogging()
{
    Init();
}

PanelLogging::PanelLogging( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create(parent, id, pos, size, style);
}


/*
 * PanelLogging creator
 */

bool PanelLogging::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
////@begin PanelLogging creation
    wxPanel::Create( parent, id, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end PanelLogging creation
    return true;
}


/*
 * PanelLogging destructor
 */

PanelLogging::~PanelLogging()
{
////@begin PanelLogging destruction
////@end PanelLogging destruction
}


/*
 * Member initialisation
 */

void PanelLogging::Init()
{
////@begin PanelLogging member initialisation
    m_txtLogging = NULL;
////@end PanelLogging member initialisation
}


/*
 * Control creation for PanelLogging
 */

void PanelLogging::CreateControls()
{    
////@begin PanelLogging content construction
    PanelLogging* itemPanel1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemPanel1->SetSizer(itemBoxSizer2);

    m_txtLogging = new wxTextCtrl( itemPanel1, ID_TEXTLOGGING, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY );
    itemBoxSizer2->Add(m_txtLogging, 1, wxGROW|wxALL, 3);

////@end PanelLogging content construction
}


/*
 * Should we show tooltips?
 */

bool PanelLogging::ShowToolTips()
{
    return true;
}

/*
 * Get bitmap resources
 */

wxBitmap PanelLogging::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin PanelLogging bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end PanelLogging bitmap retrieval
}

/*
 * Get icon resources
 */

wxIcon PanelLogging::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin PanelLogging icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end PanelLogging icon retrieval
}
