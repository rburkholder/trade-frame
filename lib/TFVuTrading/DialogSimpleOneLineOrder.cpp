/************************************************************************
 * Copyright(c) 2013, One Unified. All rights reserved.                 *
 * email: info@oneunified.net                                           *
 *                                                                      *
 * This file is provided as is WITHOUT ANY WARRANTY                     *
 *  without even the implied warranty of                                *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                *
 *                                                                      *
 * This software may not be used nor distributed without proper license *
 * agreement.                                                           *
 *                                                                      *
 * See the file LICENSE.txt for redistribution information.             *
 ************************************************************************/

#include "StdAfx.h"

#include <wx/statline.h>
#include <wx/valnum.h>

#include "DialogSimpleOneLineOrder.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

IMPLEMENT_DYNAMIC_CLASS( DialogSimpleOneLineOrder, wxDialog )

DialogSimpleOneLineOrder::DialogSimpleOneLineOrder(void) {
  Init();
}

DialogSimpleOneLineOrder::DialogSimpleOneLineOrder( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style ) {
    Init();
    Create(parent, id, caption, pos, size, style);
}

DialogSimpleOneLineOrder::~DialogSimpleOneLineOrder(void) {
}

void DialogSimpleOneLineOrder::Init() {

    m_choiceBuySell = NULL;
    m_txtQuantity = NULL;
    m_choiceDayGtc = NULL;
    m_choiceLmtMktStp = NULL;
    m_txtPrice1 = NULL;
    m_btnSubmit = NULL;

}

void DialogSimpleOneLineOrder::SetDataExchange( DataExchange* pde ) {
  DialogBase::SetDataExchange( pde );
  if ( 0 != pde ) {

    wxArrayString asBuySell;
    asBuySell.Add( "BUY" );  asBuySell.Add( "SELL" );
    wxTextValidator tvBuySell( wxFILTER_INCLUDE_LIST, &pde->sBuySell );
    tvBuySell.SetIncludes( asBuySell );
    m_choiceBuySell->SetValidator( tvBuySell );

    m_txtQuantity->SetValidator( wxIntegerValidator<unsigned int>( &pde->nQuantity, wxNUM_VAL_DEFAULT ) );

    wxArrayString asDayGtc;
    asDayGtc.Add( "DAY" ); asDayGtc.Add( "GTC" );
    wxTextValidator tvDayGtc( wxFILTER_INCLUDE_LIST, &pde->sDayGtc );
    tvDayGtc.SetIncludes( asDayGtc );
    m_choiceDayGtc->SetValidator( tvDayGtc );

    wxArrayString asLMS;
    asLMS.Add( "LMT" );  asLMS.Add( "MKT" ); asLMS.Add( "STP" );
    wxTextValidator tvLMS( wxFILTER_INCLUDE_LIST, &pde->sLmtMktStp );
    tvLMS.SetIncludes( asLMS );
    m_choiceLmtMktStp->SetValidator( tvLMS );

    m_txtPrice1->SetValidator( wxFloatingPointValidator<double>( 4, &pde->dblPrice1, wxNUM_VAL_DEFAULT  ) );

  }
  else {
//    m_cbSymbol->SetValidator( wxDefaultValidator );
  }
}

bool DialogSimpleOneLineOrder::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style ) {

    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    DialogBase::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if (GetSizer()) {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
    return true;

}

void DialogSimpleOneLineOrder::CreateControls() {  

    DialogSimpleOneLineOrder* itemDialog1 = this;

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
    m_choiceLmtMktStp->SetStringSelection(_("LMT"));
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

}

} // namespace tf
} // namespace ou
