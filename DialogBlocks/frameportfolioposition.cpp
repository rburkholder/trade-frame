/////////////////////////////////////////////////////////////////////////////
// Name:        frameportfolioposition.cpp
// Purpose:     
// Author:      Raymond P. Burkholder
// Modified by: 
// Created:     09/11/2013 20:22:22
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

#include "frameportfolioposition.h"

////@begin XPM images
////@end XPM images


/*
 * FramePortfolioPosition type definition
 */

IMPLEMENT_CLASS( FramePortfolioPosition, wxFrame )


/*
 * FramePortfolioPosition event table definition
 */

BEGIN_EVENT_TABLE( FramePortfolioPosition, wxFrame )

////@begin FramePortfolioPosition event table entries
////@end FramePortfolioPosition event table entries

END_EVENT_TABLE()


/*
 * FramePortfolioPosition constructors
 */

FramePortfolioPosition::FramePortfolioPosition()
{
    Init();
}

FramePortfolioPosition::FramePortfolioPosition( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create( parent, id, caption, pos, size, style );
}


/*
 * FramePortfolioPosition creator
 */

bool FramePortfolioPosition::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin FramePortfolioPosition creation
    wxFrame::Create( parent, id, caption, pos, size, style );

    CreateControls();
    Centre();
////@end FramePortfolioPosition creation
    return true;
}


/*
 * FramePortfolioPosition destructor
 */

FramePortfolioPosition::~FramePortfolioPosition()
{
////@begin FramePortfolioPosition destruction
////@end FramePortfolioPosition destruction
}


/*
 * Member initialisation
 */

void FramePortfolioPosition::Init()
{
////@begin FramePortfolioPosition member initialisation
////@end FramePortfolioPosition member initialisation
}


/*
 * Control creation for FramePortfolioPosition
 */

void FramePortfolioPosition::CreateControls()
{    
////@begin FramePortfolioPosition content construction
    FramePortfolioPosition* itemFrame1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemFrame1->SetSizer(itemBoxSizer2);

    wxScrolledWindow* itemScrolledWindow3 = new wxScrolledWindow( itemFrame1, ID_SCROLLEDWINDOW, wxDefaultPosition, wxSize(100, 100), wxVSCROLL );
    itemBoxSizer2->Add(itemScrolledWindow3, 1, wxGROW|wxALL, 5);
    itemScrolledWindow3->SetScrollbars(1, 1, 0, 0);

////@end FramePortfolioPosition content construction
}


/*
 * Should we show tooltips?
 */

bool FramePortfolioPosition::ShowToolTips()
{
    return true;
}

/*
 * Get bitmap resources
 */

wxBitmap FramePortfolioPosition::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin FramePortfolioPosition bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end FramePortfolioPosition bitmap retrieval
}

/*
 * Get icon resources
 */

wxIcon FramePortfolioPosition::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin FramePortfolioPosition icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end FramePortfolioPosition icon retrieval
}
