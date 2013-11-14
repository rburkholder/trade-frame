/////////////////////////////////////////////////////////////////////////////
// Name:        panelbaskettradingmain.cpp
// Purpose:     
// Author:      Raymond P. Burkholder
// Modified by: 
// Created:     22/12/2012 21:48:11
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

#include "panelbaskettradingmain.h"

////@begin XPM images
////@end XPM images


/*
 * PanelBasketTradingMain type definition
 */

IMPLEMENT_DYNAMIC_CLASS( PanelBasketTradingMain, wxPanel )


/*
 * PanelBasketTradingMain event table definition
 */

BEGIN_EVENT_TABLE( PanelBasketTradingMain, wxPanel )

////@begin PanelBasketTradingMain event table entries
////@end PanelBasketTradingMain event table entries

END_EVENT_TABLE()


/*
 * PanelBasketTradingMain constructors
 */

PanelBasketTradingMain::PanelBasketTradingMain()
{
    Init();
}

PanelBasketTradingMain::PanelBasketTradingMain( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create(parent, id, pos, size, style);
}


/*
 * PanelBasketTradingMain creator
 */

bool PanelBasketTradingMain::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
////@begin PanelBasketTradingMain creation
    wxPanel::Create( parent, id, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end PanelBasketTradingMain creation
    return true;
}


/*
 * PanelBasketTradingMain destructor
 */

PanelBasketTradingMain::~PanelBasketTradingMain()
{
////@begin PanelBasketTradingMain destruction
////@end PanelBasketTradingMain destruction
}


/*
 * Member initialisation
 */

void PanelBasketTradingMain::Init()
{
////@begin PanelBasketTradingMain member initialisation
    m_btnStart = NULL;
    m_btnExitPositions = NULL;
    m_btnStop = NULL;
    m_btnSave = NULL;
////@end PanelBasketTradingMain member initialisation
}


/*
 * Control creation for PanelBasketTradingMain
 */

void PanelBasketTradingMain::CreateControls()
{    
////@begin PanelBasketTradingMain content construction
    PanelBasketTradingMain* itemPanel1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemPanel1->SetSizer(itemBoxSizer2);

    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer3, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 2);

    m_btnStart = new wxButton( itemPanel1, ID_BtnStart, _("Start"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer3->Add(m_btnStart, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer5, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 2);

    m_btnExitPositions = new wxButton( itemPanel1, ID_BtnExitPositions, _("Exit Positions"), wxDefaultPosition, wxDefaultSize, 0 );
    m_btnExitPositions->Enable(false);
    itemBoxSizer5->Add(m_btnExitPositions, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    wxBoxSizer* itemBoxSizer7 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer7, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 2);

    m_btnStop = new wxButton( itemPanel1, ID_BtnStop, _("Stop"), wxDefaultPosition, wxDefaultSize, 0 );
    m_btnStop->Enable(false);
    itemBoxSizer7->Add(m_btnStop, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    wxBoxSizer* itemBoxSizer9 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer9, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 2);

    m_btnSave = new wxButton( itemPanel1, ID_BtnSave, _("Save Series"), wxDefaultPosition, wxDefaultSize, 0 );
    m_btnSave->Enable(false);
    itemBoxSizer9->Add(m_btnSave, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

////@end PanelBasketTradingMain content construction
}


/*
 * Should we show tooltips?
 */

bool PanelBasketTradingMain::ShowToolTips()
{
    return true;
}

/*
 * Get bitmap resources
 */

wxBitmap PanelBasketTradingMain::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin PanelBasketTradingMain bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end PanelBasketTradingMain bitmap retrieval
}

/*
 * Get icon resources
 */

wxIcon PanelBasketTradingMain::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin PanelBasketTradingMain icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end PanelBasketTradingMain icon retrieval
}
