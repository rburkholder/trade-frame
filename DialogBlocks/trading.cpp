/////////////////////////////////////////////////////////////////////////////
// Name:        trading.cpp
// Purpose:     
// Author:      Raymond P. Burkholder
// Modified by: 
// Created:     04/09/2011 15:19:36
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

#include "trading.h"

////@begin XPM images
////@end XPM images


/*
 * FrameManualOrder type definition
 */

IMPLEMENT_CLASS( FrameManualOrder, wxFrame )


/*
 * FrameManualOrder event table definition
 */

BEGIN_EVENT_TABLE( FrameManualOrder, wxFrame )

////@begin FrameManualOrder event table entries
    EVT_TEXT( ID_TxtInstrumentSymbol, FrameManualOrder::OnTxtInstrumentSymbolTextUpdated )
    EVT_TEXT_ENTER( ID_TxtInstrumentSymbol, FrameManualOrder::OnTxtInstrumentSymbolEnter )

////@end FrameManualOrder event table entries

END_EVENT_TABLE()


/*
 * FrameManualOrder constructors
 */

FrameManualOrder::FrameManualOrder()
{
    Init();
}

FrameManualOrder::FrameManualOrder( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create( parent, id, caption, pos, size, style );
}


/*
 * FrameManualOrder creator
 */

bool FrameManualOrder::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin FrameManualOrder creation
    wxFrame::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end FrameManualOrder creation
    return true;
}


/*
 * FrameManualOrder destructor
 */

FrameManualOrder::~FrameManualOrder()
{
////@begin FrameManualOrder destruction
////@end FrameManualOrder destruction
}


/*
 * Member initialisation
 */

void FrameManualOrder::Init()
{
////@begin FrameManualOrder member initialisation
    m_txtInstrumentSymbol = NULL;
    m_txtInstrumentName = NULL;
    m_btnBuy = NULL;
    m_btnSell = NULL;
    m_btnCancel = NULL;
////@end FrameManualOrder member initialisation
}


/*
 * Control creation for FrameManualOrder
 */

void FrameManualOrder::CreateControls()
{    
////@begin FrameManualOrder content construction
    FrameManualOrder* itemFrame1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemFrame1->SetSizer(itemBoxSizer2);

    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer3, 0, wxALIGN_LEFT|wxALL, 4);

    wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer3->Add(itemBoxSizer4, 0, wxALIGN_TOP|wxRIGHT, 2);

    wxStaticText* itemStaticText5 = new wxStaticText( itemFrame1, ID_LblInstrumentSymbol, _("Instrument:"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
    itemBoxSizer4->Add(itemStaticText5, 0, wxALIGN_LEFT|wxLEFT|wxBOTTOM, 3);

    wxBoxSizer* itemBoxSizer6 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer4->Add(itemBoxSizer6, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    m_txtInstrumentSymbol = new wxTextCtrl( itemFrame1, ID_TxtInstrumentSymbol, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, InstrumentNameValidator( &m_order.sSymbol) );
    m_txtInstrumentSymbol->SetMaxLength(20);
    if (FrameManualOrder::ShowToolTips())
        m_txtInstrumentSymbol->SetToolTip(_("Instrument Symbol"));
    itemBoxSizer6->Add(m_txtInstrumentSymbol, 0, wxALIGN_CENTER_VERTICAL, 2);

    m_txtInstrumentName = new wxStaticText( itemFrame1, ID_LblInstrumentName, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer6->Add(m_txtInstrumentName, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer9 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer9, 0, wxALIGN_LEFT|wxALL, 5);

    wxBoxSizer* itemBoxSizer10 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer9->Add(itemBoxSizer10, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, 2);

    wxStaticText* itemStaticText11 = new wxStaticText( itemFrame1, ID_LblQuantity, _("Quantity:"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
    itemBoxSizer10->Add(itemStaticText11, 0, wxALIGN_LEFT|wxLEFT|wxBOTTOM, 3);

    wxTextCtrl* itemTextCtrl12 = new wxTextCtrl( itemFrame1, ID_TxtQuantity, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxIntegerValidator<unsigned long>( &m_order.nQuantity, wxNUM_VAL_ZERO_AS_BLANK ) );
    itemTextCtrl12->SetMaxLength(20);
    if (FrameManualOrder::ShowToolTips())
        itemTextCtrl12->SetToolTip(_("Quantity"));
    itemBoxSizer10->Add(itemTextCtrl12, 0, wxALIGN_CENTER_HORIZONTAL, 2);

    wxBoxSizer* itemBoxSizer13 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer9->Add(itemBoxSizer13, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, 2);

    wxStaticText* itemStaticText14 = new wxStaticText( itemFrame1, ID_LblPrice1, _("Price 1:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer13->Add(itemStaticText14, 0, wxALIGN_LEFT|wxLEFT|wxBOTTOM, 3);

    wxTextCtrl* itemTextCtrl15 = new wxTextCtrl( itemFrame1, ID_TxtPrice1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxFloatingPointValidator<double>( 4, &m_order.dblPrice1, wxNUM_VAL_ZERO_AS_BLANK ) );
    itemTextCtrl15->SetMaxLength(20);
    if (FrameManualOrder::ShowToolTips())
        itemTextCtrl15->SetToolTip(_("Limit Price"));
    itemBoxSizer13->Add(itemTextCtrl15, 0, wxALIGN_CENTER_HORIZONTAL, 2);

    wxBoxSizer* itemBoxSizer16 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer9->Add(itemBoxSizer16, 0, wxALIGN_CENTER_VERTICAL, 2);

    wxStaticText* itemStaticText17 = new wxStaticText( itemFrame1, ID_LblPrice2, _("Price 2:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer16->Add(itemStaticText17, 0, wxALIGN_LEFT|wxLEFT|wxBOTTOM, 3);

    wxTextCtrl* itemTextCtrl18 = new wxTextCtrl( itemFrame1, ID_TxtPrice2, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxFloatingPointValidator<double>( 4, &m_order.dblPrice2, wxNUM_VAL_ZERO_AS_BLANK ) );
    itemTextCtrl18->SetMaxLength(20);
    itemBoxSizer16->Add(itemTextCtrl18, 0, wxALIGN_CENTER_HORIZONTAL, 5);

    wxBoxSizer* itemBoxSizer19 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer19, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 2);

    wxBoxSizer* itemBoxSizer20 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer19->Add(itemBoxSizer20, 0, wxALIGN_CENTER_VERTICAL, 5);

    wxRadioButton* itemRadioButton21 = new wxRadioButton( itemFrame1, ID_BtnOrderTypeMarket, _("Market"), wxDefaultPosition, wxDefaultSize, 0 );
    itemRadioButton21->SetValue(false);
    itemBoxSizer20->Add(itemRadioButton21, 0, wxALIGN_CENTER_VERTICAL, 5);

    wxRadioButton* itemRadioButton22 = new wxRadioButton( itemFrame1, ID_BtnOrderTypeLimit, _("&Limit"), wxDefaultPosition, wxDefaultSize, 0 );
    itemRadioButton22->SetValue(true);
    itemBoxSizer20->Add(itemRadioButton22, 0, wxALIGN_CENTER_VERTICAL, 5);

    wxRadioButton* itemRadioButton23 = new wxRadioButton( itemFrame1, ID_BtnOrderTypeStop, _("S&top"), wxDefaultPosition, wxDefaultSize, 0 );
    itemRadioButton23->SetValue(false);
    itemBoxSizer20->Add(itemRadioButton23, 0, wxALIGN_CENTER_VERTICAL, 5);

    itemBoxSizer19->Add(5, 5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer25 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer19->Add(itemBoxSizer25, 0, wxALIGN_CENTER_VERTICAL, 5);

    m_btnBuy = new wxButton( itemFrame1, ID_BtnBuy, _("&Buy"), wxDefaultPosition, wxDefaultSize, 0 );
    m_btnBuy->Enable(false);
    itemBoxSizer25->Add(m_btnBuy, 0, wxALIGN_CENTER_VERTICAL, 3);

    m_btnSell = new wxButton( itemFrame1, ID_BtnSell, _("&Sell"), wxDefaultPosition, wxDefaultSize, 0 );
    m_btnSell->Enable(false);
    itemBoxSizer25->Add(m_btnSell, 0, wxALIGN_CENTER_VERTICAL, 3);

    m_btnCancel = new wxButton( itemFrame1, ID_BtnCancel, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    m_btnCancel->Enable(false);
    itemBoxSizer25->Add(m_btnCancel, 0, wxALIGN_CENTER_VERTICAL, 5);

    // Connect events and objects
    m_txtInstrumentSymbol->Connect(ID_TxtInstrumentSymbol, wxEVT_IDLE, wxIdleEventHandler(FrameManualOrder::OnIdle), NULL, this);
    itemRadioButton21->Connect(ID_BtnOrderTypeMarket, wxEVT_LEFT_UP, wxMouseEventHandler(FrameManualOrder::OnLeftUp), NULL, this);
////@end FrameManualOrder content construction
}


/*
 * wxEVT_LEFT_UP event handler for ID_BtnOrderTypeMarket
 */

void FrameManualOrder::OnLeftUp( wxMouseEvent& event )
{
////@begin wxEVT_LEFT_UP event handler for ID_BtnOrderTypeMarket in FrameManualOrder.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_LEFT_UP event handler for ID_BtnOrderTypeMarket in FrameManualOrder. 
}


/*
 * Should we show tooltips?
 */

bool FrameManualOrder::ShowToolTips()
{
    return true;
}

/*
 * Get bitmap resources
 */

wxBitmap FrameManualOrder::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin FrameManualOrder bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end FrameManualOrder bitmap retrieval
}

/*
 * Get icon resources
 */

wxIcon FrameManualOrder::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin FrameManualOrder icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end FrameManualOrder icon retrieval
}


/*
 * wxEVT_COMMAND_TEXT_UPDATED event handler for ID_TxtInstrumentSymbol
 */

void FrameManualOrder::OnTxtInstrumentSymbolTextUpdated( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_TEXT_UPDATED event handler for ID_TxtInstrumentSymbol in FrameManualOrder.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_COMMAND_TEXT_UPDATED event handler for ID_TxtInstrumentSymbol in FrameManualOrder. 
}


/*
 * wxEVT_COMMAND_TEXT_ENTER event handler for ID_TxtInstrumentSymbol
 */

void FrameManualOrder::OnTxtInstrumentSymbolEnter( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_TEXT_ENTER event handler for ID_TxtInstrumentSymbol in FrameManualOrder.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_COMMAND_TEXT_ENTER event handler for ID_TxtInstrumentSymbol in FrameManualOrder. 
}


/*
 * wxEVT_IDLE event handler for ID_TxtInstrumentSymbol
 */

void FrameManualOrder::OnIdle( wxIdleEvent& event )
{
////@begin wxEVT_IDLE event handler for ID_TxtInstrumentSymbol in FrameManualOrder.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_IDLE event handler for ID_TxtInstrumentSymbol in FrameManualOrder. 
}

