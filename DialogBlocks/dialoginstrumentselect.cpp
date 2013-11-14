/////////////////////////////////////////////////////////////////////////////
// Name:        dialoginstrumentselect.cpp
// Purpose:     
// Author:      Raymond P. Burkholder
// Modified by: 
// Created:     11/11/2013 11:32:02
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

#include "dialoginstrumentselect.h"

////@begin XPM images
////@end XPM images


/*
 * DialogInstrumentSelect type definition
 */

IMPLEMENT_DYNAMIC_CLASS( DialogInstrumentSelect, wxDialog )


/*
 * DialogInstrumentSelect event table definition
 */

BEGIN_EVENT_TABLE( DialogInstrumentSelect, wxDialog )

////@begin DialogInstrumentSelect event table entries
////@end DialogInstrumentSelect event table entries

END_EVENT_TABLE()


/*
 * DialogInstrumentSelect constructors
 */

DialogInstrumentSelect::DialogInstrumentSelect()
{
    Init();
}

DialogInstrumentSelect::DialogInstrumentSelect( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create(parent, id, caption, pos, size, style);
}


/*
 * DialogInstrumentSelect creator
 */

bool DialogInstrumentSelect::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin DialogInstrumentSelect creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end DialogInstrumentSelect creation
    return true;
}


/*
 * DialogInstrumentSelect destructor
 */

DialogInstrumentSelect::~DialogInstrumentSelect()
{
////@begin DialogInstrumentSelect destruction
////@end DialogInstrumentSelect destruction
}


/*
 * Member initialisation
 */

void DialogInstrumentSelect::Init()
{
////@begin DialogInstrumentSelect member initialisation
    m_cbSymbol = NULL;
    m_lblDescription = NULL;
    m_btnOk = NULL;
    m_btnCancel = NULL;
////@end DialogInstrumentSelect member initialisation
}


/*
 * Control creation for DialogInstrumentSelect
 */

void DialogInstrumentSelect::CreateControls()
{    
////@begin DialogInstrumentSelect content construction
    DialogInstrumentSelect* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer3, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxArrayString m_cbSymbolStrings;
    m_cbSymbol = new wxComboBox( itemDialog1, ID_CBSymbol, wxEmptyString, wxDefaultPosition, wxSize(100, -1), m_cbSymbolStrings, wxCB_DROPDOWN );
    itemBoxSizer3->Add(m_cbSymbol, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_lblDescription = new wxStaticText( itemDialog1, ID_LblDescription, _("Description"), wxDefaultPosition, wxSize(200, -1), 0 );
    itemBoxSizer3->Add(m_lblDescription, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer6 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer6, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    m_btnOk = new wxButton( itemDialog1, wxID_OK, _("OK"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer6->Add(m_btnOk, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_btnCancel = new wxButton( itemDialog1, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer6->Add(m_btnCancel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

////@end DialogInstrumentSelect content construction
}


/*
 * Should we show tooltips?
 */

bool DialogInstrumentSelect::ShowToolTips()
{
    return true;
}

/*
 * Get bitmap resources
 */

wxBitmap DialogInstrumentSelect::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin DialogInstrumentSelect bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end DialogInstrumentSelect bitmap retrieval
}

/*
 * Get icon resources
 */

wxIcon DialogInstrumentSelect::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin DialogInstrumentSelect icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end DialogInstrumentSelect icon retrieval
}
