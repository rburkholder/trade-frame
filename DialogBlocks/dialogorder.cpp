/////////////////////////////////////////////////////////////////////////////
// Name:        dialogorder.cpp
// Purpose:     
// Author:      Raymond P. Burkholder
// Modified by: 
// Created:     13/11/2013 16:16:39
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

#include "dialogorder.h"

////@begin XPM images
////@end XPM images


/*
 * DialogOrder type definition
 */

IMPLEMENT_DYNAMIC_CLASS( DialogOrder, wxDialog )


/*
 * DialogOrder event table definition
 */

BEGIN_EVENT_TABLE( DialogOrder, wxDialog )

////@begin DialogOrder event table entries
////@end DialogOrder event table entries

END_EVENT_TABLE()


/*
 * DialogOrder constructors
 */

DialogOrder::DialogOrder()
{
    Init();
}

DialogOrder::DialogOrder( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create(parent, id, caption, pos, size, style);
}


/*
 * DialogOrder creator
 */

bool DialogOrder::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin DialogOrder creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end DialogOrder creation
    return true;
}


/*
 * DialogOrder destructor
 */

DialogOrder::~DialogOrder()
{
////@begin DialogOrder destruction
////@end DialogOrder destruction
}


/*
 * Member initialisation
 */

void DialogOrder::Init()
{
////@begin DialogOrder member initialisation
    m_choiceBuySell = NULL;
    m_txtQuantity = NULL;
    m_choiceDayGtc = NULL;
    m_choiceLmtMktStp = NULL;
    m_txtPrice1 = NULL;
    m_btnSubmit = NULL;
////@end DialogOrder member initialisation
}


/*
 * Control creation for DialogOrder
 */

void DialogOrder::CreateControls()
{    
////@begin DialogOrder content construction
    DialogOrder* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    wxArrayString m_choiceBuySellStrings;
    m_choiceBuySellStrings.Add(_("BUY"));
    m_choiceBuySellStrings.Add(_("SELL"));
    m_choiceBuySell = new wxChoice( itemDialog1, ID_ChcBuySell, wxDefaultPosition, wxDefaultSize, m_choiceBuySellStrings, 0 );
    m_choiceBuySell->SetStringSelection(_("BUY"));
    itemBoxSizer2->Add(m_choiceBuySell, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    m_txtQuantity = new wxTextCtrl( itemDialog1, ID_TxtQuantity, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer2->Add(m_txtQuantity, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxArrayString m_choiceDayGtcStrings;
    m_choiceDayGtcStrings.Add(_("DAY"));
    m_choiceDayGtcStrings.Add(_("GTC"));
    m_choiceDayGtc = new wxChoice( itemDialog1, ID_ChcDayGtc, wxDefaultPosition, wxDefaultSize, m_choiceDayGtcStrings, 0 );
    m_choiceDayGtc->SetStringSelection(_("DAY"));
    itemBoxSizer2->Add(m_choiceDayGtc, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxArrayString m_choiceLmtMktStpStrings;
    m_choiceLmtMktStpStrings.Add(_("LMT"));
    m_choiceLmtMktStpStrings.Add(_("MKT"));
    m_choiceLmtMktStpStrings.Add(_("STP"));
    m_choiceLmtMktStp = new wxChoice( itemDialog1, ID_ChcLmtMktStp, wxDefaultPosition, wxDefaultSize, m_choiceLmtMktStpStrings, 0 );
    m_choiceLmtMktStp->SetStringSelection(_("LIMIT"));
    itemBoxSizer2->Add(m_choiceLmtMktStp, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    m_txtPrice1 = new wxTextCtrl( itemDialog1, ID_TxtPrice1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_txtPrice1->Enable(false);
    itemBoxSizer2->Add(m_txtPrice1, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxStaticLine* itemStaticLine8 = new wxStaticLine( itemDialog1, wxID_STATIC, wxDefaultPosition, wxSize(-1, 8), wxLI_VERTICAL );
    itemBoxSizer2->Add(itemStaticLine8, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    m_btnSubmit = new wxButton( itemDialog1, wxID_OK, _("OK"), wxDefaultPosition, wxSize(30, -1), 0 );
    itemBoxSizer2->Add(m_btnSubmit, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxButton* itemButton10 = new wxButton( itemDialog1, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxSize(30, -1), 0 );
    itemBoxSizer2->Add(itemButton10, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

////@end DialogOrder content construction
}


/*
 * Should we show tooltips?
 */

bool DialogOrder::ShowToolTips()
{
    return true;
}

/*
 * Get bitmap resources
 */

wxBitmap DialogOrder::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin DialogOrder bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end DialogOrder bitmap retrieval
}

/*
 * Get icon resources
 */

wxIcon DialogOrder::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin DialogOrder icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end DialogOrder icon retrieval
}
