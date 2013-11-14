/////////////////////////////////////////////////////////////////////////////
// Name:        panelportfoliostats.cpp
// Purpose:     
// Author:      Raymond P. Burkholder
// Modified by: 
// Created:     23/12/2012 21:42:27
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

#include "panelportfoliostats.h"

////@begin XPM images
////@end XPM images


/*
 * PanelPortfolioStats type definition
 */

IMPLEMENT_DYNAMIC_CLASS( PanelPortfolioStats, wxPanel )


/*
 * PanelPortfolioStats event table definition
 */

BEGIN_EVENT_TABLE( PanelPortfolioStats, wxPanel )

////@begin PanelPortfolioStats event table entries
////@end PanelPortfolioStats event table entries

END_EVENT_TABLE()


/*
 * PanelPortfolioStats constructors
 */

PanelPortfolioStats::PanelPortfolioStats()
{
    Init();
}

PanelPortfolioStats::PanelPortfolioStats( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create(parent, id, pos, size, style);
}


/*
 * PanelPortfolioStats creator
 */

bool PanelPortfolioStats::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
////@begin PanelPortfolioStats creation
    wxPanel::Create( parent, id, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end PanelPortfolioStats creation
    return true;
}


/*
 * PanelPortfolioStats destructor
 */

PanelPortfolioStats::~PanelPortfolioStats()
{
////@begin PanelPortfolioStats destruction
////@end PanelPortfolioStats destruction
}


/*
 * Member initialisation
 */

void PanelPortfolioStats::Init()
{
////@begin PanelPortfolioStats member initialisation
    m_txtPortfolioLow = NULL;
    m_txtPortfolioCurrent = NULL;
    m_txtPortfolioHigh = NULL;
////@end PanelPortfolioStats member initialisation
}


/*
 * Control creation for PanelPortfolioStats
 */

void PanelPortfolioStats::CreateControls()
{    
////@begin PanelPortfolioStats content construction
    PanelPortfolioStats* itemPanel1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
    itemPanel1->SetSizer(itemBoxSizer2);

    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer3, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxStaticText* itemStaticText4 = new wxStaticText( itemPanel1, wxID_STATIC, _("Portfolio Profit:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer3->Add(itemStaticText4, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer3->Add(itemBoxSizer5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxStaticText* itemStaticText6 = new wxStaticText( itemPanel1, wxID_STATIC, _("Low"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer5->Add(itemStaticText6, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_txtPortfolioLow = new wxTextCtrl( itemPanel1, ID_TxtPortfolioLow, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer5->Add(m_txtPortfolioLow, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    wxBoxSizer* itemBoxSizer8 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer3->Add(itemBoxSizer8, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxStaticText* itemStaticText9 = new wxStaticText( itemPanel1, wxID_STATIC, _("Current"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer8->Add(itemStaticText9, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_txtPortfolioCurrent = new wxTextCtrl( itemPanel1, ID_TxtPortfolioCurrent, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer8->Add(m_txtPortfolioCurrent, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    wxBoxSizer* itemBoxSizer11 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer3->Add(itemBoxSizer11, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxStaticText* itemStaticText12 = new wxStaticText( itemPanel1, wxID_STATIC, _("High"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer11->Add(itemStaticText12, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_txtPortfolioHigh = new wxTextCtrl( itemPanel1, ID_TxtPortfolioHigh, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer11->Add(m_txtPortfolioHigh, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

////@end PanelPortfolioStats content construction
}


/*
 * Should we show tooltips?
 */

bool PanelPortfolioStats::ShowToolTips()
{
    return true;
}

/*
 * Get bitmap resources
 */

wxBitmap PanelPortfolioStats::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin PanelPortfolioStats bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end PanelPortfolioStats bitmap retrieval
}

/*
 * Get icon resources
 */

wxIcon PanelPortfolioStats::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin PanelPortfolioStats icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end PanelPortfolioStats icon retrieval
}
