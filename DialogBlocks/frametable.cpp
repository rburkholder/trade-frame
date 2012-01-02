/////////////////////////////////////////////////////////////////////////////
// Name:        frametable.cpp
// Purpose:     
// Author:      Raymond P. Burkholder
// Modified by: 
// Created:     24/09/2011 18:45:38
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

#include "frametable.h"

////@begin XPM images
////@end XPM images


/*
 * FrameTable type definition
 */

IMPLEMENT_CLASS( FrameTable, wxFrame )


/*
 * FrameTable event table definition
 */

BEGIN_EVENT_TABLE( FrameTable, wxFrame )

////@begin FrameTable event table entries
////@end FrameTable event table entries

END_EVENT_TABLE()


/*
 * FrameTable constructors
 */

FrameTable::FrameTable()
{
    Init();
}

FrameTable::FrameTable( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create( parent, id, caption, pos, size, style );
}


/*
 * FrameTable creator
 */

bool FrameTable::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin FrameTable creation
    wxFrame::Create( parent, id, caption, pos, size, style );

    CreateControls();
    Centre();
////@end FrameTable creation
    return true;
}


/*
 * FrameTable destructor
 */

FrameTable::~FrameTable()
{
////@begin FrameTable destruction
////@end FrameTable destruction
}


/*
 * Member initialisation
 */

void FrameTable::Init()
{
////@begin FrameTable member initialisation
////@end FrameTable member initialisation
}


/*
 * Control creation for FrameTable
 */

void FrameTable::CreateControls()
{    
////@begin FrameTable content construction
    FrameTable* itemFrame1 = this;

    wxGrid* itemGrid2 = new wxGrid( itemFrame1, ID_GRID, wxDefaultPosition, wxSize(200, 150), wxSUNKEN_BORDER|wxHSCROLL|wxVSCROLL );
    itemGrid2->SetDefaultColSize(50);
    itemGrid2->SetDefaultRowSize(25);
    itemGrid2->SetColLabelSize(25);
    itemGrid2->SetRowLabelSize(50);
    itemGrid2->CreateGrid(5, 5, wxGrid::wxGridSelectCells);

////@end FrameTable content construction
}


/*
 * Should we show tooltips?
 */

bool FrameTable::ShowToolTips()
{
    return true;
}

/*
 * Get bitmap resources
 */

wxBitmap FrameTable::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin FrameTable bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end FrameTable bitmap retrieval
}

/*
 * Get icon resources
 */

wxIcon FrameTable::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin FrameTable icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end FrameTable icon retrieval
}
