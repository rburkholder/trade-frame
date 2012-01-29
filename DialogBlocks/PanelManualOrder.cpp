/////////////////////////////////////////////////////////////////////////////
// Name:        PanelManualOrder.cpp
// Purpose:     
// Author:      Raymond P. Burkholder
// Modified by: 
// Created:     29/01/2012 14:36:16
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

#include "PanelManualOrder.h"

////@begin XPM images
////@end XPM images


/*
 * PanelManualOrder type definition
 */

IMPLEMENT_DYNAMIC_CLASS( PanelManualOrder, wxPanel )


/*
 * PanelManualOrder event table definition
 */

BEGIN_EVENT_TABLE( PanelManualOrder, wxPanel )

////@begin PanelManualOrder event table entries
////@end PanelManualOrder event table entries

END_EVENT_TABLE()


/*
 * PanelManualOrder constructors
 */

PanelManualOrder::PanelManualOrder()
{
    Init();
}

PanelManualOrder::PanelManualOrder( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create(parent, id, pos, size, style);
}


/*
 * PanelManualOrder creator
 */

bool PanelManualOrder::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
////@begin PanelManualOrder creation
    wxPanel::Create( parent, id, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end PanelManualOrder creation
    return true;
}


/*
 * PanelManualOrder destructor
 */

PanelManualOrder::~PanelManualOrder()
{
////@begin PanelManualOrder destruction
////@end PanelManualOrder destruction
}


/*
 * Member initialisation
 */

void PanelManualOrder::Init()
{
////@begin PanelManualOrder member initialisation
    m_txtInstrumentSymbol = NULL;
    m_txtInstrumentName = NULL;
    m_btnBuy = NULL;
    m_btnSell = NULL;
    m_btnCancel = NULL;
    m_txtStatus = NULL;
////@end PanelManualOrder member initialisation
}


/*
 * Control creation for PanelManualOrder
 */

void PanelManualOrder::CreateControls()
{    
////@begin PanelManualOrder content construction
    PanelManualOrder* itemPanel1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemPanel1->SetSizer(itemBoxSizer2);

    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer3, 0, wxALIGN_LEFT|wxALL, 4);

    wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer3->Add(itemBoxSizer4, 0, wxALIGN_TOP|wxRIGHT, 2);

    wxStaticText* itemStaticText5 = new wxStaticText( itemPanel1, ID_LblInstrumentSymbol, _("Instrument:"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
    itemBoxSizer4->Add(itemStaticText5, 0, wxALIGN_LEFT|wxLEFT|wxBOTTOM, 3);

    wxBoxSizer* itemBoxSizer6 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer4->Add(itemBoxSizer6, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    m_txtInstrumentSymbol = new wxTextCtrl( itemPanel1, ID_TxtInstrumentSymbol, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, InstrumentNameValidator( &m_order.sSymbol) );
    m_txtInstrumentSymbol->SetMaxLength(20);
    if (PanelManualOrder::ShowToolTips())
        m_txtInstrumentSymbol->SetToolTip(_("Instrument Symbol"));
    itemBoxSizer6->Add(m_txtInstrumentSymbol, 0, wxALIGN_CENTER_VERTICAL, 2);

    m_txtInstrumentName = new wxStaticText( itemPanel1, ID_LblInstrumentName, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer6->Add(m_txtInstrumentName, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer9 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer9, 0, wxALIGN_LEFT|wxALL, 5);

    wxBoxSizer* itemBoxSizer10 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer9->Add(itemBoxSizer10, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, 2);

    wxStaticText* itemStaticText11 = new wxStaticText( itemPanel1, ID_LblQuantity, _("Quantity:"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
    itemBoxSizer10->Add(itemStaticText11, 0, wxALIGN_LEFT|wxLEFT|wxBOTTOM, 3);

    wxTextCtrl* itemTextCtrl12 = new wxTextCtrl( itemPanel1, ID_TxtQuantity, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxIntegerValidator<unsigned long>( &m_order.nQuantity, wxNUM_VAL_ZERO_AS_BLANK ) );
    itemTextCtrl12->SetMaxLength(20);
    if (PanelManualOrder::ShowToolTips())
        itemTextCtrl12->SetToolTip(_("Quantity"));
    itemBoxSizer10->Add(itemTextCtrl12, 0, wxALIGN_CENTER_HORIZONTAL, 2);

    wxBoxSizer* itemBoxSizer13 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer9->Add(itemBoxSizer13, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, 2);

    wxStaticText* itemStaticText14 = new wxStaticText( itemPanel1, ID_LblPrice1, _("Price 1:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer13->Add(itemStaticText14, 0, wxALIGN_LEFT|wxLEFT|wxBOTTOM, 3);

    wxTextCtrl* itemTextCtrl15 = new wxTextCtrl( itemPanel1, ID_TxtPrice1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxFloatingPointValidator<double>( 4, &m_order.dblPrice1, wxNUM_VAL_ZERO_AS_BLANK ) );
    itemTextCtrl15->SetMaxLength(20);
    if (PanelManualOrder::ShowToolTips())
        itemTextCtrl15->SetToolTip(_("Limit Price"));
    itemBoxSizer13->Add(itemTextCtrl15, 0, wxALIGN_CENTER_HORIZONTAL, 2);

    wxBoxSizer* itemBoxSizer16 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer9->Add(itemBoxSizer16, 0, wxALIGN_CENTER_VERTICAL, 2);

    wxStaticText* itemStaticText17 = new wxStaticText( itemPanel1, ID_LblPrice2, _("Price 2:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer16->Add(itemStaticText17, 0, wxALIGN_LEFT|wxLEFT|wxBOTTOM, 3);

    wxTextCtrl* itemTextCtrl18 = new wxTextCtrl( itemPanel1, ID_TxtPrice2, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxFloatingPointValidator<double>( 4, &m_order.dblPrice2, wxNUM_VAL_ZERO_AS_BLANK ) );
    itemTextCtrl18->SetMaxLength(20);
    itemBoxSizer16->Add(itemTextCtrl18, 0, wxALIGN_CENTER_HORIZONTAL, 5);

    wxBoxSizer* itemBoxSizer19 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer19, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 2);

    wxBoxSizer* itemBoxSizer20 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer19->Add(itemBoxSizer20, 0, wxALIGN_CENTER_VERTICAL, 5);

    wxRadioButton* itemRadioButton21 = new wxRadioButton( itemPanel1, ID_BtnOrderTypeMarket, _("Market"), wxDefaultPosition, wxDefaultSize, 0 );
    itemRadioButton21->SetValue(false);
    itemBoxSizer20->Add(itemRadioButton21, 0, wxALIGN_CENTER_VERTICAL, 5);

    wxRadioButton* itemRadioButton22 = new wxRadioButton( itemPanel1, ID_BtnOrderTypeLimit, _("&Limit"), wxDefaultPosition, wxDefaultSize, 0 );
    itemRadioButton22->SetValue(true);
    itemBoxSizer20->Add(itemRadioButton22, 0, wxALIGN_CENTER_VERTICAL, 5);

    wxRadioButton* itemRadioButton23 = new wxRadioButton( itemPanel1, ID_BtnOrderTypeStop, _("S&top"), wxDefaultPosition, wxDefaultSize, 0 );
    itemRadioButton23->SetValue(false);
    itemBoxSizer20->Add(itemRadioButton23, 0, wxALIGN_CENTER_VERTICAL, 5);

    itemBoxSizer19->Add(5, 5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer25 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer19->Add(itemBoxSizer25, 0, wxALIGN_CENTER_VERTICAL, 5);

    m_btnBuy = new wxButton( itemPanel1, ID_BtnBuy, _("&Buy"), wxDefaultPosition, wxDefaultSize, 0 );
    m_btnBuy->Enable(false);
    itemBoxSizer25->Add(m_btnBuy, 0, wxALIGN_CENTER_VERTICAL, 3);

    m_btnSell = new wxButton( itemPanel1, ID_BtnSell, _("&Sell"), wxDefaultPosition, wxDefaultSize, 0 );
    m_btnSell->Enable(false);
    itemBoxSizer25->Add(m_btnSell, 0, wxALIGN_CENTER_VERTICAL, 3);

    m_btnCancel = new wxButton( itemPanel1, ID_BtnCancel, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    m_btnCancel->Show(false);
    m_btnCancel->Enable(false);
    itemBoxSizer25->Add(m_btnCancel, 0, wxALIGN_CENTER_VERTICAL, 5);

    m_txtStatus = new wxStaticText( itemPanel1, wxID_STATIC, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
    itemBoxSizer2->Add(m_txtStatus, 0, wxALIGN_LEFT|wxALL, 5);

////@end PanelManualOrder content construction
}


/*
 * Should we show tooltips?
 */

bool PanelManualOrder::ShowToolTips()
{
    return true;
}

/*
 * Get bitmap resources
 */

wxBitmap PanelManualOrder::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin PanelManualOrder bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end PanelManualOrder bitmap retrieval
}

/*
 * Get icon resources
 */

wxIcon PanelManualOrder::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin PanelManualOrder icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end PanelManualOrder icon retrieval
}
