/////////////////////////////////////////////////////////////////////////////
// Name:        FrameGeneric.cpp
// Purpose:     
// Author:      Raymond P. Burkholder
// Modified by: 
// Created:     06/11/2011 14:32:35
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

#include "FrameGeneric.h"

////@begin XPM images
////@end XPM images


/*
 * FrameGeneric type definition
 */

IMPLEMENT_CLASS( FrameGeneric, wxFrame )


/*
 * FrameGeneric event table definition
 */

BEGIN_EVENT_TABLE( FrameGeneric, wxFrame )

////@begin FrameGeneric event table entries
////@end FrameGeneric event table entries

END_EVENT_TABLE()


/*
 * FrameGeneric constructors
 */

FrameGeneric::FrameGeneric()
{
    Init();
}

FrameGeneric::FrameGeneric( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create( parent, id, caption, pos, size, style );
}


/*
 * FrameGeneric creator
 */

bool FrameGeneric::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin FrameGeneric creation
    wxFrame::Create( parent, id, caption, pos, size, style );

    CreateControls();
    Centre();
////@end FrameGeneric creation
    return true;
}


/*
 * FrameGeneric destructor
 */

FrameGeneric::~FrameGeneric()
{
////@begin FrameGeneric destruction
////@end FrameGeneric destruction
}


/*
 * Member initialisation
 */

void FrameGeneric::Init()
{
////@begin FrameGeneric member initialisation
    m_panelMain = NULL;
    m_sizerMain = NULL;
    m_statusBar = NULL;
////@end FrameGeneric member initialisation
}


/*
 * Control creation for FrameGeneric
 */

void FrameGeneric::CreateControls()
{    
////@begin FrameGeneric content construction
    FrameGeneric* itemFrame1 = this;

    wxMenuBar* menuBar = new wxMenuBar;
    wxMenu* itemMenu5 = new wxMenu;
    itemMenu5->Append(ID_MENUEXIT, _("Exit"), wxEmptyString, wxITEM_NORMAL);
    menuBar->Append(itemMenu5, _("Menu"));
    itemFrame1->SetMenuBar(menuBar);

    m_panelMain = new wxPanel( itemFrame1, ID_PANELMAIN, wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER|wxTAB_TRAVERSAL );

    m_sizerMain = new wxBoxSizer(wxHORIZONTAL);
    m_panelMain->SetSizer(m_sizerMain);

    m_statusBar = new wxStatusBar( itemFrame1, ID_STATUSBAR, wxST_SIZEGRIP|wxNO_BORDER );
    m_statusBar->SetFieldsCount(2);
    itemFrame1->SetStatusBar(m_statusBar);

////@end FrameGeneric content construction
}


/*
 * Should we show tooltips?
 */

bool FrameGeneric::ShowToolTips()
{
    return true;
}

/*
 * Get bitmap resources
 */

wxBitmap FrameGeneric::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin FrameGeneric bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end FrameGeneric bitmap retrieval
}

/*
 * Get icon resources
 */

wxIcon FrameGeneric::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin FrameGeneric icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end FrameGeneric icon retrieval
}
