/////////////////////////////////////////////////////////////////////////////
// Name:        framehedgedbollinger.cpp
// Purpose:     
// Author:      Raymond P. Burkholder
// Modified by: 
// Created:     23/10/2013 21:11:39
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

#include "framehedgedbollinger.h"

////@begin XPM images
////@end XPM images


/*
 * FrameHedgedBollinger type definition
 */

IMPLEMENT_CLASS( FrameHedgedBollinger, wxFrame )


/*
 * FrameHedgedBollinger event table definition
 */

BEGIN_EVENT_TABLE( FrameHedgedBollinger, wxFrame )

////@begin FrameHedgedBollinger event table entries
////@end FrameHedgedBollinger event table entries

END_EVENT_TABLE()


/*
 * FrameHedgedBollinger constructors
 */

FrameHedgedBollinger::FrameHedgedBollinger()
{
    Init();
}

FrameHedgedBollinger::FrameHedgedBollinger( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create( parent, id, caption, pos, size, style );
}


/*
 * FrameHedgedBollinger creator
 */

bool FrameHedgedBollinger::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin FrameHedgedBollinger creation
    wxFrame::Create( parent, id, caption, pos, size, style );

    CreateControls();
    Centre();
////@end FrameHedgedBollinger creation
    return true;
}


/*
 * FrameHedgedBollinger destructor
 */

FrameHedgedBollinger::~FrameHedgedBollinger()
{
////@begin FrameHedgedBollinger destruction
////@end FrameHedgedBollinger destruction
}


/*
 * Member initialisation
 */

void FrameHedgedBollinger::Init()
{
////@begin FrameHedgedBollinger member initialisation
    m_sizerFrame = NULL;
    m_sizerFrameRow1 = NULL;
    m_splitterRow1 = NULL;
    m_panelSplitterRight = NULL;
    m_sizerSplitterRight = NULL;
    m_panelProvider = NULL;
    m_panelLog = NULL;
    m_sizerChart = NULL;
////@end FrameHedgedBollinger member initialisation
}


/*
 * Control creation for FrameHedgedBollinger
 */

void FrameHedgedBollinger::CreateControls()
{    
////@begin FrameHedgedBollinger content construction
    FrameHedgedBollinger* itemFrame1 = this;

    m_sizerFrame = new wxBoxSizer(wxVERTICAL);
    itemFrame1->SetSizer(m_sizerFrame);

    m_sizerFrameRow1 = new wxBoxSizer(wxHORIZONTAL);
    m_sizerFrame->Add(m_sizerFrameRow1, 1, wxGROW|wxALL, 5);

    m_splitterRow1 = new wxSplitterWindow( itemFrame1, ID_SPLITTERWINDOW1, wxDefaultPosition, wxSize(100, 100), wxSP_3DBORDER|wxSP_3DSASH|wxNO_BORDER );
    m_splitterRow1->SetMinimumPaneSize(10);
    m_splitterRow1->SetSashGravity(0.3);
    m_splitterRow1->SetName(_T("Splitter1"));

    wxTreeCtrl* itemTreeCtrl5 = new wxTreeCtrl( m_splitterRow1, ID_TREECTRL, wxDefaultPosition, wxSize(100, 100), wxTR_SINGLE );
    itemTreeCtrl5->SetName(_T("tree"));

    m_panelSplitterRight = new wxPanel( m_splitterRow1, ID_PANEL4, wxDefaultPosition, wxSize(300, -1), wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    m_sizerSplitterRight = new wxBoxSizer(wxHORIZONTAL);
    m_panelSplitterRight->SetSizer(m_sizerSplitterRight);

    m_panelProvider = new wxPanel( m_panelSplitterRight, ID_PANEL5, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    m_sizerSplitterRight->Add(m_panelProvider, 0, wxGROW|wxALL, 5);
    wxStaticText* itemStaticText9 = new wxStaticText( m_panelProvider, wxID_STATIC, _("providers"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE );

    m_panelLog = new wxPanel( m_panelSplitterRight, ID_PANEL6, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    m_sizerSplitterRight->Add(m_panelLog, 1, wxGROW|wxALL, 5);
    wxStaticText* itemStaticText11 = new wxStaticText( m_panelLog, wxID_STATIC, _("log"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE );

    m_splitterRow1->SplitVertically(itemTreeCtrl5, m_panelSplitterRight, 10);
    m_sizerFrameRow1->Add(m_splitterRow1, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_sizerChart = new wxBoxSizer(wxHORIZONTAL);
    m_sizerFrame->Add(m_sizerChart, 1, wxGROW|wxALL, 5);

    wxPanel* itemPanel13 = new wxPanel( itemFrame1, ID_PANEL7, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    m_sizerChart->Add(itemPanel13, 1, wxGROW|wxALL, 5);

    wxGauge* itemGauge14 = new wxGauge( itemPanel13, ID_GAUGE, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL );
    itemGauge14->SetValue(1);

////@end FrameHedgedBollinger content construction
}


/*
 * Should we show tooltips?
 */

bool FrameHedgedBollinger::ShowToolTips()
{
    return true;
}

/*
 * Get bitmap resources
 */

wxBitmap FrameHedgedBollinger::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin FrameHedgedBollinger bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end FrameHedgedBollinger bitmap retrieval
}

/*
 * Get icon resources
 */

wxIcon FrameHedgedBollinger::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin FrameHedgedBollinger icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end FrameHedgedBollinger icon retrieval
}
