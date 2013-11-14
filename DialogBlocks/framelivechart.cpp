/////////////////////////////////////////////////////////////////////////////
// Name:        framelivechart.cpp
// Purpose:     
// Author:      Raymond P. Burkholder
// Modified by: 
// Created:     29/09/2013 16:21:50
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
#include "wx/imaglist.h"
////@end includes

#include "framelivechart.h"

////@begin XPM images
////@end XPM images


/*
 * FrameLiveChart type definition
 */

IMPLEMENT_CLASS( FrameLiveChart, wxFrame )


/*
 * FrameLiveChart event table definition
 */

BEGIN_EVENT_TABLE( FrameLiveChart, wxFrame )

////@begin FrameLiveChart event table entries
////@end FrameLiveChart event table entries

END_EVENT_TABLE()


/*
 * FrameLiveChart constructors
 */

FrameLiveChart::FrameLiveChart()
{
    Init();
}

FrameLiveChart::FrameLiveChart( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create( parent, id, caption, pos, size, style );
}


/*
 * FrameLiveChart creator
 */

bool FrameLiveChart::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin FrameLiveChart creation
    wxFrame::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end FrameLiveChart creation
    return true;
}


/*
 * FrameLiveChart destructor
 */

FrameLiveChart::~FrameLiveChart()
{
////@begin FrameLiveChart destruction
////@end FrameLiveChart destruction
}


/*
 * Member initialisation
 */

void FrameLiveChart::Init()
{
////@begin FrameLiveChart member initialisation
    sizerMainH = NULL;
    splitter = NULL;
    tree = NULL;
    panelSplitterRightPanel = NULL;
    sizerRight = NULL;
    m_sizerControls = NULL;
    m_pPanelProviderControl = NULL;
    staticText = NULL;
    m_pPanelLogging = NULL;
    m_winChart = NULL;
////@end FrameLiveChart member initialisation
}


/*
 * Control creation for FrameLiveChart
 */

void FrameLiveChart::CreateControls()
{    
////@begin FrameLiveChart content construction
    FrameLiveChart* itemFrame1 = this;

    sizerMainH = new wxBoxSizer(wxHORIZONTAL);
    itemFrame1->SetSizer(sizerMainH);

    splitter = new wxSplitterWindow( itemFrame1, IDsplitter, wxDefaultPosition, wxDefaultSize, wxSP_3DBORDER|wxSP_3DSASH|wxNO_BORDER|wxEXPAND );
    splitter->SetMinimumPaneSize(10);
    splitter->SetSashGravity(0.5);

    tree = new wxTreeCtrl( splitter, IDtreeLeft, wxDefaultPosition, wxDefaultSize, wxTR_SINGLE );

    panelSplitterRightPanel = new wxPanel( splitter, IDpanelRight, wxDefaultPosition, wxDefaultSize, wxNO_BORDER|wxTAB_TRAVERSAL );
    sizerRight = new wxBoxSizer(wxVERTICAL);
    panelSplitterRightPanel->SetSizer(sizerRight);

    m_sizerControls = new wxBoxSizer(wxHORIZONTAL);
    sizerRight->Add(m_sizerControls, 0, wxALIGN_LEFT|wxALL, 5);
    m_pPanelProviderControl = new wxPanel( panelSplitterRightPanel, IDpanelMiniLeft, wxDefaultPosition, wxSize(50, -100), wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    m_sizerControls->Add(m_pPanelProviderControl, 0, wxALIGN_TOP|wxALL, 1);
    staticText = new wxStaticText( m_pPanelProviderControl, wxID_STATIC, _("Static text"), wxDefaultPosition, wxDefaultSize, 0 );

    m_pPanelLogging = new wxPanel( panelSplitterRightPanel, IDpanelMiniRight, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    m_sizerControls->Add(m_pPanelLogging, 1, wxGROW|wxALL, 1);

    m_winChart = new wxPanel( panelSplitterRightPanel, IDpanelrightBottom, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    sizerRight->Add(m_winChart, 1, wxGROW|wxALL, 5);

    splitter->SplitVertically(tree, panelSplitterRightPanel, 0);
    sizerMainH->Add(splitter, 1, wxGROW|wxALL, 5);

////@end FrameLiveChart content construction
}


/*
 * Should we show tooltips?
 */

bool FrameLiveChart::ShowToolTips()
{
    return true;
}

/*
 * Get bitmap resources
 */

wxBitmap FrameLiveChart::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin FrameLiveChart bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end FrameLiveChart bitmap retrieval
}

/*
 * Get icon resources
 */

wxIcon FrameLiveChart::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin FrameLiveChart icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end FrameLiveChart icon retrieval
}
