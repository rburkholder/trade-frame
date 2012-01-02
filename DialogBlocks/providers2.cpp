/////////////////////////////////////////////////////////////////////////////
// Name:        providers2.cpp
// Purpose:     
// Author:      Raymond P. Burkholder
// Modified by: 
// Created:     23/08/2011 23:29:20
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

#include "providers2.h"

////@begin XPM images

////@end XPM images


/*
 * Providers2 type definition
 */

IMPLEMENT_CLASS( Providers2, wxFrame )


/*
 * Providers2 event table definition
 */

BEGIN_EVENT_TABLE( Providers2, wxFrame )

////@begin Providers2 event table entries
////@end Providers2 event table entries

END_EVENT_TABLE()


/*
 * Providers2 constructors
 */

Providers2::Providers2()
{
    Init();
}

Providers2::Providers2( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create( parent, id, caption, pos, size, style );
}


/*
 * Providers2 creator
 */

bool Providers2::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin Providers2 creation
    wxFrame::Create( parent, id, caption, pos, size, style );

    CreateControls();
    Centre();
////@end Providers2 creation
    return true;
}


/*
 * Providers2 destructor
 */

Providers2::~Providers2()
{
////@begin Providers2 destruction
////@end Providers2 destruction
}


/*
 * Member initialisation
 */

void Providers2::Init()
{
////@begin Providers2 member initialisation
////@end Providers2 member initialisation
}


/*
 * Control creation for Providers2
 */

void Providers2::CreateControls()
{    
////@begin Providers2 content construction
    Providers2* itemFrame1 = this;

////@end Providers2 content construction
}


/*
 * Should we show tooltips?
 */

bool Providers2::ShowToolTips()
{
    return true;
}

/*
 * Get bitmap resources
 */

wxBitmap Providers2::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin Providers2 bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end Providers2 bitmap retrieval
}

/*
 * Get icon resources
 */

wxIcon Providers2::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin Providers2 icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end Providers2 icon retrieval
}
