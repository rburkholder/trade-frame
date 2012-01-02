/////////////////////////////////////////////////////////////////////////////
// Name:        project1app.cpp
// Purpose:     
// Author:      Raymond P. Burkholder
// Modified by: 
// Created:     14/08/2011 20:05:27
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

#include "project1app.h"

////@begin XPM images
////@end XPM images


/*
 * Application instance implementation
 */

////@begin implement app
IMPLEMENT_APP( Project1App )
////@end implement app


/*
 * Project1App type definition
 */

IMPLEMENT_CLASS( Project1App, wxApp )


/*
 * Project1App event table definition
 */

BEGIN_EVENT_TABLE( Project1App, wxApp )

////@begin Project1App event table entries
////@end Project1App event table entries

END_EVENT_TABLE()


/*
 * Constructor for Project1App
 */

Project1App::Project1App()
{
    Init();
}


/*
 * Member initialisation
 */

void Project1App::Init()
{
////@begin Project1App member initialisation
////@end Project1App member initialisation
}

/*
 * Initialisation for Project1App
 */

bool Project1App::OnInit()
{    
////@begin Project1App initialisation
	// Remove the comment markers above and below this block
	// to make permanent changes to the code.

#if wxUSE_XPM
	wxImage::AddHandler(new wxXPMHandler);
#endif
#if wxUSE_LIBPNG
	wxImage::AddHandler(new wxPNGHandler);
#endif
#if wxUSE_LIBJPEG
	wxImage::AddHandler(new wxJPEGHandler);
#endif
#if wxUSE_GIF
	wxImage::AddHandler(new wxGIFHandler);
#endif
////@end Project1App initialisation

    return true;
}


/*
 * Cleanup for Project1App
 */

int Project1App::OnExit()
{    
////@begin Project1App cleanup
	return wxApp::OnExit();
////@end Project1App cleanup
}

