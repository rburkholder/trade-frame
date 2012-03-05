/////////////////////////////////////////////////////////////////////////////
// Name:        frametest.cpp
// Purpose:     
// Author:      Raymond P. Burkholder
// Modified by: 
// Created:     04/03/2012 13:00:26
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

#include "frametest.h"

////@begin XPM images

////@end XPM images


/*
 * FrameTest type definition
 */

IMPLEMENT_CLASS( FrameTest, wxFrame )


/*
 * FrameTest event table definition
 */

BEGIN_EVENT_TABLE( FrameTest, wxFrame )

////@begin FrameTest event table entries
////@end FrameTest event table entries

END_EVENT_TABLE()


/*
 * FrameTest constructors
 */

FrameTest::FrameTest()
{
    Init();
}

FrameTest::FrameTest( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create( parent, id, caption, pos, size, style );
}


/*
 * FrameTest creator
 */

bool FrameTest::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin FrameTest creation
    wxFrame::Create( parent, id, caption, pos, size, style );

    CreateControls();
    Centre();
////@end FrameTest creation
    return true;
}


/*
 * FrameTest destructor
 */

FrameTest::~FrameTest()
{
////@begin FrameTest destruction
////@end FrameTest destruction
}


/*
 * Member initialisation
 */

void FrameTest::Init()
{
////@begin FrameTest member initialisation
////@end FrameTest member initialisation
}


/*
 * Control creation for FrameTest
 */

void FrameTest::CreateControls()
{    
////@begin FrameTest content construction
    FrameTest* itemFrame1 = this;

////@end FrameTest content construction
}


/*
 * Should we show tooltips?
 */

bool FrameTest::ShowToolTips()
{
    return true;
}

/*
 * Get bitmap resources
 */

wxBitmap FrameTest::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin FrameTest bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end FrameTest bitmap retrieval
}

/*
 * Get icon resources
 */

wxIcon FrameTest::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin FrameTest icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end FrameTest icon retrieval
}
