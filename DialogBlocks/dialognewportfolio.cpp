/////////////////////////////////////////////////////////////////////////////
// Name:        dialognewportfolio.cpp
// Purpose:     
// Author:      Raymond P. Burkholder
// Modified by: 
// Created:     12/11/2013 22:26:40
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

#include "dialognewportfolio.h"

////@begin XPM images
////@end XPM images


/*
 * DialogNewPortfolio type definition
 */

IMPLEMENT_DYNAMIC_CLASS( DialogNewPortfolio, wxDialog )


/*
 * DialogNewPortfolio event table definition
 */

BEGIN_EVENT_TABLE( DialogNewPortfolio, wxDialog )

////@begin DialogNewPortfolio event table entries
////@end DialogNewPortfolio event table entries

END_EVENT_TABLE()


/*
 * DialogNewPortfolio constructors
 */

DialogNewPortfolio::DialogNewPortfolio()
{
    Init();
}

DialogNewPortfolio::DialogNewPortfolio( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create(parent, id, caption, pos, size, style);
}


/*
 * DialogNewPortfolio creator
 */

bool DialogNewPortfolio::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin DialogNewPortfolio creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end DialogNewPortfolio creation
    return true;
}


/*
 * DialogNewPortfolio destructor
 */

DialogNewPortfolio::~DialogNewPortfolio()
{
////@begin DialogNewPortfolio destruction
////@end DialogNewPortfolio destruction
}


/*
 * Member initialisation
 */

void DialogNewPortfolio::Init()
{
////@begin DialogNewPortfolio member initialisation
    m_txtPortfolioId = NULL;
    m_txtDescription = NULL;
    m_btnOk = NULL;
    m_btnCancel = NULL;
////@end DialogNewPortfolio member initialisation
}


/*
 * Control creation for DialogNewPortfolio
 */

void DialogNewPortfolio::CreateControls()
{    
////@begin DialogNewPortfolio content construction
    DialogNewPortfolio* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    wxFlexGridSizer* itemFlexGridSizer3 = new wxFlexGridSizer(0, 2, 0, 0);
    itemBoxSizer2->Add(itemFlexGridSizer3, 1, wxGROW|wxALL, 5);

    wxStaticText* itemStaticText4 = new wxStaticText( itemDialog1, ID_LblPortfolioId, _("Portfolio Name:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer3->Add(itemStaticText4, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_txtPortfolioId = new wxTextCtrl( itemDialog1, ID_TxtPortfolioId, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer3->Add(m_txtPortfolioId, 1, wxALIGN_LEFT|wxGROW|wxALL, 5);

    wxStaticText* itemStaticText6 = new wxStaticText( itemDialog1, ID_LblDescription, _("Description:"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
    itemFlexGridSizer3->Add(itemStaticText6, 0, wxALIGN_RIGHT|wxALIGN_TOP|wxALL, 5);

    m_txtDescription = new wxTextCtrl( itemDialog1, ID_TxtDescription, wxEmptyString, wxDefaultPosition, wxSize(200, -1), wxTE_MULTILINE );
    itemFlexGridSizer3->Add(m_txtDescription, 1, wxGROW|wxGROW|wxALL, 5);

    itemFlexGridSizer3->AddGrowableRow(1);
    itemFlexGridSizer3->AddGrowableCol(1);

    wxBoxSizer* itemBoxSizer8 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer8, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    m_btnOk = new wxButton( itemDialog1, wxID_OK, _("OK"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer8->Add(m_btnOk, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_btnCancel = new wxButton( itemDialog1, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer8->Add(m_btnCancel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

////@end DialogNewPortfolio content construction
}


/*
 * Should we show tooltips?
 */

bool DialogNewPortfolio::ShowToolTips()
{
    return true;
}

/*
 * Get bitmap resources
 */

wxBitmap DialogNewPortfolio::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin DialogNewPortfolio bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end DialogNewPortfolio bitmap retrieval
}

/*
 * Get icon resources
 */

wxIcon DialogNewPortfolio::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin DialogNewPortfolio icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end DialogNewPortfolio icon retrieval
}
