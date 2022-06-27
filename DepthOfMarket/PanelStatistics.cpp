/************************************************************************
 * Copyright(c) 2022, One Unified. All rights reserved.                 *
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

/*
 * File:    PanelStatistics.cpp
 * Author:  raymond@burkholder.net
 * Project: DepthOfMarket
 * Created: April 11, 2022  13:01
 */

#include <boost/format.hpp>

#include <boost/lexical_cast.hpp>

#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/stattext.h>

#include "PanelStatistics.hpp"

PanelStatistics::PanelStatistics() {
  Init();
}

PanelStatistics::PanelStatistics(
  wxWindow* parent, wxWindowID id,
  const wxPoint& pos, const wxSize& size, long style
) {
  Init();
  Create(parent, id, pos, size, style);
}

bool PanelStatistics::Create(
  wxWindow* parent, wxWindowID id,
  const wxPoint& pos, const wxSize& size, long style
){

    SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
    wxPanel::Create( parent, id, pos, size, style );

    CreateControls();
    if (GetSizer()) {
        GetSizer()->SetSizeHints(this);
    }
    //Centre();
    return true;
}

PanelStatistics::~PanelStatistics() {
}

void PanelStatistics::Init() {
}

void PanelStatistics::CreateControls() {

  PanelStatistics* itemPanel1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemPanel1->SetSizer(itemBoxSizer2);

    wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer4, 0, wxALIGN_LEFT|wxALL, 1);

    wxStaticText* itemStaticText5 = new wxStaticText( itemPanel1, wxID_STATIC, _("l1 msgs:"), wxDefaultPosition, wxSize(55, -1), 0 );
    itemBoxSizer4->Add(itemStaticText5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    m_txtL1MsgBid = new wxTextCtrl( itemPanel1, id_txtL1MsgBid, wxEmptyString, wxDefaultPosition, wxSize(50, -1), wxTE_READONLY|wxTE_RIGHT|wxNO_BORDER );
    itemBoxSizer4->Add(m_txtL1MsgBid, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxStaticText* itemStaticText3 = new wxStaticText( itemPanel1, wxID_STATIC, _("bid"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer4->Add(itemStaticText3, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    m_txtL1MsgAsk = new wxTextCtrl( itemPanel1, id_txtL1MsgAsk, wxEmptyString, wxDefaultPosition, wxSize(50, -1), wxTE_READONLY|wxTE_RIGHT|wxNO_BORDER );
    itemBoxSizer4->Add(m_txtL1MsgAsk, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxStaticText* itemStaticText6 = new wxStaticText( itemPanel1, wxID_STATIC, _("ask"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer4->Add(itemStaticText6, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    m_txtL1MsgTtl = new wxTextCtrl( itemPanel1, id_txtL1MsgTtl, wxEmptyString, wxDefaultPosition, wxSize(50, -1), wxTE_READONLY|wxTE_RIGHT|wxNO_BORDER );
    itemBoxSizer4->Add(m_txtL1MsgTtl, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxStaticText* itemStaticText8 = new wxStaticText( itemPanel1, wxID_STATIC, _("ttl"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer4->Add(itemStaticText8, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxBoxSizer* itemBoxSizer12 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer12, 0, wxALIGN_LEFT|wxALL, 1);

    wxStaticText* itemStaticText13 = new wxStaticText( itemPanel1, wxID_STATIC, _("l2 msgs:"), wxDefaultPosition, wxSize(55, -1), 0 );
    itemBoxSizer12->Add(itemStaticText13, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    m_txtL2MsgBid = new wxTextCtrl( itemPanel1, id_txtL2MsgBid, wxEmptyString, wxDefaultPosition, wxSize(50, -1), wxTE_READONLY|wxTE_RIGHT|wxNO_BORDER );
    itemBoxSizer12->Add(m_txtL2MsgBid, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxStaticText* itemStaticText10 = new wxStaticText( itemPanel1, wxID_STATIC, _("bid"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer12->Add(itemStaticText10, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    m_txtL2MsgAsk = new wxTextCtrl( itemPanel1, id_txtL2MsgAsk, wxEmptyString, wxDefaultPosition, wxSize(50, -1), wxTE_READONLY|wxTE_RIGHT|wxNO_BORDER );
    itemBoxSizer12->Add(m_txtL2MsgAsk, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxStaticText* itemStaticText12 = new wxStaticText( itemPanel1, wxID_STATIC, _("ask"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer12->Add(itemStaticText12, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    m_txtL2MsgTtl = new wxTextCtrl( itemPanel1, id_txtL2MsgTtl, wxEmptyString, wxDefaultPosition, wxSize(50, -1), wxTE_READONLY|wxTE_RIGHT|wxNO_BORDER );
    itemBoxSizer12->Add(m_txtL2MsgTtl, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxStaticText* itemStaticText14 = new wxStaticText( itemPanel1, wxID_STATIC, _("ttl"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer12->Add(itemStaticText14, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxBoxSizer* itemBoxSizer20 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer20, 0, wxALIGN_LEFT|wxALL, 1);

    wxStaticText* itemStaticText21 = new wxStaticText( itemPanel1, wxID_STATIC, _("l2 add:"), wxDefaultPosition, wxSize(55, -1), 0 );
    itemBoxSizer20->Add(itemStaticText21, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    m_txtL2AddBid = new wxTextCtrl( itemPanel1, id_txtL2AddBid, wxEmptyString, wxDefaultPosition, wxSize(50, -1), wxTE_READONLY|wxTE_RIGHT|wxNO_BORDER );
    itemBoxSizer20->Add(m_txtL2AddBid, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxStaticText* itemStaticText16 = new wxStaticText( itemPanel1, wxID_STATIC, _("bid"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer20->Add(itemStaticText16, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    m_txtL2AddAsk = new wxTextCtrl( itemPanel1, id_txtL2AddAsk, wxEmptyString, wxDefaultPosition, wxSize(50, -1), wxTE_READONLY|wxTE_RIGHT|wxNO_BORDER );
    itemBoxSizer20->Add(m_txtL2AddAsk, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxStaticText* itemStaticText18 = new wxStaticText( itemPanel1, wxID_STATIC, _("ask"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer20->Add(itemStaticText18, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    m_txtL2AddTtl = new wxTextCtrl( itemPanel1, id_txtL2AddTtl, wxEmptyString, wxDefaultPosition, wxSize(50, -1), wxTE_READONLY|wxTE_RIGHT|wxNO_BORDER );
    itemBoxSizer20->Add(m_txtL2AddTtl, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxStaticText* itemStaticText20 = new wxStaticText( itemPanel1, wxID_STATIC, _("ttl"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer20->Add(itemStaticText20, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer3, 0, wxALIGN_LEFT|wxALL, 1);

    wxStaticText* itemStaticText7 = new wxStaticText( itemPanel1, wxID_STATIC, _("l2 upd:"), wxDefaultPosition, wxSize(55, -1), 0 );
    itemBoxSizer3->Add(itemStaticText7, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    m_txtL2UpdBid = new wxTextCtrl( itemPanel1, id_txtL2UpdBid, wxEmptyString, wxDefaultPosition, wxSize(50, -1), wxTE_READONLY|wxTE_RIGHT|wxNO_BORDER );
    itemBoxSizer3->Add(m_txtL2UpdBid, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxStaticText* itemStaticText9 = new wxStaticText( itemPanel1, wxID_STATIC, _("bid"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer3->Add(itemStaticText9, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    m_txtL2UpdAsk = new wxTextCtrl( itemPanel1, id_txtL2UpdAsk, wxEmptyString, wxDefaultPosition, wxSize(50, -1), wxTE_READONLY|wxTE_RIGHT|wxNO_BORDER );
    itemBoxSizer3->Add(m_txtL2UpdAsk, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxStaticText* itemStaticText11 = new wxStaticText( itemPanel1, wxID_STATIC, _("ask"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer3->Add(itemStaticText11, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    m_txtL2UpdTtl = new wxTextCtrl( itemPanel1, id_txtL2UpdTtl, wxEmptyString, wxDefaultPosition, wxSize(50, -1), wxTE_READONLY|wxTE_RIGHT|wxNO_BORDER );
    itemBoxSizer3->Add(m_txtL2UpdTtl, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxStaticText* itemStaticText15 = new wxStaticText( itemPanel1, wxID_STATIC, _("ttl"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer3->Add(itemStaticText15, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxBoxSizer* itemBoxSizer16 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer16, 0, wxALIGN_LEFT|wxALL, 1);

    wxStaticText* itemStaticText17 = new wxStaticText( itemPanel1, wxID_STATIC, _("l2 del:"), wxDefaultPosition, wxSize(55, -1), 0 );
    itemBoxSizer16->Add(itemStaticText17, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    m_txtL2DelBid = new wxTextCtrl( itemPanel1, id_txtL2DelBid, wxEmptyString, wxDefaultPosition, wxSize(50, -1), wxTE_READONLY|wxTE_RIGHT|wxNO_BORDER );
    itemBoxSizer16->Add(m_txtL2DelBid, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxStaticText* itemStaticText19 = new wxStaticText( itemPanel1, wxID_STATIC, _("bid"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer16->Add(itemStaticText19, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    m_txtL2DelAsk = new wxTextCtrl( itemPanel1, id_txtL2DelAsk, wxEmptyString, wxDefaultPosition, wxSize(50, -1), wxTE_READONLY|wxTE_RIGHT|wxNO_BORDER );
    itemBoxSizer16->Add(m_txtL2DelAsk, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxStaticText* itemStaticText22 = new wxStaticText( itemPanel1, wxID_STATIC, _("ask"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer16->Add(itemStaticText22, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    m_txtL2DelTtl = new wxTextCtrl( itemPanel1, id_txtL2DelTtl, wxEmptyString, wxDefaultPosition, wxSize(50, -1), wxTE_READONLY|wxTE_RIGHT|wxNO_BORDER );
    itemBoxSizer16->Add(m_txtL2DelTtl, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxStaticText* itemStaticText24 = new wxStaticText( itemPanel1, wxID_STATIC, _("ttl"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer16->Add(itemStaticText24, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer5, 0, wxALIGN_LEFT|wxALL, 1);

    wxStaticText* itemStaticText23 = new wxStaticText( itemPanel1, wxID_STATIC, _("bst ask:"), wxDefaultPosition, wxSize(55, -1), 0 );
    itemBoxSizer5->Add(itemStaticText23, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    m_txtLvl1AskTick = new wxTextCtrl( itemPanel1, id_txtLvl1AskTick, wxEmptyString, wxDefaultPosition, wxSize(50, -1), wxTE_READONLY|wxTE_RIGHT|wxNO_BORDER );
    itemBoxSizer5->Add(m_txtLvl1AskTick, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxStaticText* itemStaticText25 = new wxStaticText( itemPanel1, wxID_STATIC, _("tick"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer5->Add(itemStaticText25, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    m_txtLvl1AskAdd = new wxTextCtrl( itemPanel1, id_txtLvl1AskAdd, wxEmptyString, wxDefaultPosition, wxSize(50, -1), wxTE_READONLY|wxTE_RIGHT|wxNO_BORDER );
    itemBoxSizer5->Add(m_txtLvl1AskAdd, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxStaticText* itemStaticText27 = new wxStaticText( itemPanel1, wxID_STATIC, _("add"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer5->Add(itemStaticText27, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    m_txtLvl1AskDel = new wxTextCtrl( itemPanel1, id_txtLvl1AskDel, wxEmptyString, wxDefaultPosition, wxSize(50, -1), wxTE_READONLY|wxTE_RIGHT|wxNO_BORDER );
    itemBoxSizer5->Add(m_txtLvl1AskDel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxStaticText* itemStaticText29 = new wxStaticText( itemPanel1, wxID_STATIC, _("del"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer5->Add(itemStaticText29, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxBoxSizer* itemBoxSizer30 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer30, 0, wxALIGN_LEFT|wxALL, 1);

    wxStaticText* itemStaticText31 = new wxStaticText( itemPanel1, wxID_STATIC, _("unkn:"), wxDefaultPosition, wxSize(55, -1), 0 );
    itemBoxSizer30->Add(itemStaticText31, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    m_txtLvl1UknTick = new wxTextCtrl( itemPanel1, id_txtLvl1UknTick, wxEmptyString, wxDefaultPosition, wxSize(50, -1), wxTE_READONLY|wxTE_RIGHT|wxNO_BORDER );
    itemBoxSizer30->Add(m_txtLvl1UknTick, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxStaticText* itemStaticText33 = new wxStaticText( itemPanel1, wxID_STATIC, _("tick"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer30->Add(itemStaticText33, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    m_txtLvl1UknAdd = new wxTextCtrl( itemPanel1, id_txtLvl1UknAdd, wxEmptyString, wxDefaultPosition, wxSize(50, -1), wxTE_READONLY|wxTE_RIGHT|wxNO_BORDER );
    itemBoxSizer30->Add(m_txtLvl1UknAdd, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxStaticText* itemStaticText35 = new wxStaticText( itemPanel1, wxID_STATIC, _("add"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer30->Add(itemStaticText35, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    m_txtLvl1UknDel = new wxTextCtrl( itemPanel1, id_txtLvl1UknDel, wxEmptyString, wxDefaultPosition, wxSize(50, -1), wxTE_READONLY|wxTE_RIGHT|wxNO_BORDER );
    itemBoxSizer30->Add(m_txtLvl1UknDel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxStaticText* itemStaticText37 = new wxStaticText( itemPanel1, wxID_STATIC, _("del"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer30->Add(itemStaticText37, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxBoxSizer* itemBoxSizer38 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer38, 0, wxALIGN_LEFT|wxALL, 1);

    wxStaticText* itemStaticText39 = new wxStaticText( itemPanel1, wxID_STATIC, _("bst bid:"), wxDefaultPosition, wxSize(55, -1), 0 );
    itemBoxSizer38->Add(itemStaticText39, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    m_txtLvl1BidTick = new wxTextCtrl( itemPanel1, id_txtLvl1BidTick, wxEmptyString, wxDefaultPosition, wxSize(50, -1), wxTE_READONLY|wxTE_RIGHT|wxNO_BORDER );
    itemBoxSizer38->Add(m_txtLvl1BidTick, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxStaticText* itemStaticText41 = new wxStaticText( itemPanel1, wxID_STATIC, _("tick"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer38->Add(itemStaticText41, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    m_txtLvl1BidAdd = new wxTextCtrl( itemPanel1, id_txtLvl1BidAdd, wxEmptyString, wxDefaultPosition, wxSize(50, -1), wxTE_READONLY|wxTE_RIGHT|wxNO_BORDER );
    itemBoxSizer38->Add(m_txtLvl1BidAdd, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxStaticText* itemStaticText43 = new wxStaticText( itemPanel1, wxID_STATIC, _("add"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer38->Add(itemStaticText43, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    m_txtLvl1BidDel = new wxTextCtrl( itemPanel1, id_txtLvl1BidDel, wxEmptyString, wxDefaultPosition, wxSize(50, -1), wxTE_READONLY|wxTE_RIGHT|wxNO_BORDER );
    itemBoxSizer38->Add(m_txtLvl1BidDel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxStaticText* itemStaticText45 = new wxStaticText( itemPanel1, wxID_STATIC, _("del"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer38->Add(itemStaticText45, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxBoxSizer* itemBoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer1, 0, wxALIGN_LEFT|wxALL, 1);

    wxStaticText* itemStaticText2 = new wxStaticText( itemPanel1, wxID_STATIC, _("ticks"), wxDefaultPosition, wxSize(55, -1), 0 );
    itemBoxSizer1->Add(itemStaticText2, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    m_txtTicks = new wxTextCtrl( itemPanel1, id_txtTicks, wxEmptyString, wxDefaultPosition, wxSize(50, -1), wxTE_READONLY|wxTE_RIGHT|wxNO_BORDER );
    itemBoxSizer1->Add(m_txtTicks, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxStaticText* itemStaticText4 = new wxStaticText( itemPanel1, wxID_STATIC, _("ttl"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer1->Add(itemStaticText4, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxBoxSizer* itemBoxSizer6 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer6, 0, wxALIGN_LEFT|wxALL, 1);

    wxStaticText* itemStaticText26 = new wxStaticText( itemPanel1, wxID_STATIC, _("imbalance:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer6->Add(itemStaticText26, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    m_txtB0 = new wxTextCtrl( itemPanel1, id_txtB0, wxEmptyString, wxDefaultPosition, wxSize(50, -1), wxTE_READONLY|wxTE_RIGHT|wxNO_BORDER );
    itemBoxSizer6->Add(m_txtB0, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxStaticText* itemStaticText28 = new wxStaticText( itemPanel1, wxID_STATIC, _("b0"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer6->Add(itemStaticText28, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    m_txtB1 = new wxTextCtrl( itemPanel1, id_txtB1, wxEmptyString, wxDefaultPosition, wxSize(50, -1), wxTE_READONLY|wxTE_RIGHT|wxNO_BORDER );
    itemBoxSizer6->Add(m_txtB1, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxStaticText* itemStaticText30 = new wxStaticText( itemPanel1, wxID_STATIC, _("b1"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer6->Add(itemStaticText30, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    m_txtR = new wxTextCtrl( itemPanel1, id_txtR, wxEmptyString, wxDefaultPosition, wxSize(50, -1), wxTE_READONLY|wxTE_RIGHT|wxNO_BORDER );
    itemBoxSizer6->Add(m_txtR, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxStaticText* itemStaticText32 = new wxStaticText( itemPanel1, wxID_STATIC, _("r"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer6->Add(itemStaticText32, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

}

void PanelStatistics::Update( const values_t& values ) {
  m_txtTicks->SetValue( boost::lexical_cast<std::string>( values.nTicks ) );
  m_txtL1MsgBid->SetValue( boost::lexical_cast<std::string>( values.nL1MsgBid ) );
  m_txtL1MsgAsk->SetValue( boost::lexical_cast<std::string>( values.nL1MsgAsk ) );
  m_txtL1MsgTtl->SetValue( boost::lexical_cast<std::string>( values.nL1MsgTtl ) );

  m_txtL2MsgBid->SetValue( boost::lexical_cast<std::string>( values.nL2MsgBid ) );
  m_txtL2MsgAsk->SetValue( boost::lexical_cast<std::string>( values.nL2MsgAsk ) );
  m_txtL2MsgTtl->SetValue( boost::lexical_cast<std::string>( values.nL2MsgTtl ) );

  m_txtL2AddBid->SetValue( boost::lexical_cast<std::string>( values.nL2AddBid ) );
  m_txtL2AddAsk->SetValue( boost::lexical_cast<std::string>( values.nL2AddAsk ) );
  m_txtL2AddTtl->SetValue( boost::lexical_cast<std::string>( values.nL2AddTtl ) );

  m_txtL2UpdBid->SetValue( boost::lexical_cast<std::string>( values.nL2UpdBid ) );
  m_txtL2UpdAsk->SetValue( boost::lexical_cast<std::string>( values.nL2UpdAsk ) );
  m_txtL2UpdTtl->SetValue( boost::lexical_cast<std::string>( values.nL2UpdTtl ) );

  m_txtL2DelBid->SetValue( boost::lexical_cast<std::string>( values.nL2DelBid ) );
  m_txtL2DelAsk->SetValue( boost::lexical_cast<std::string>( values.nL2DelAsk ) );
  m_txtL2DelTtl->SetValue( boost::lexical_cast<std::string>( values.nL2DelTtl ) );

  m_txtLvl1AskTick->SetValue( boost::lexical_cast<std::string>( values.nLvl1AskTick ) );
  m_txtLvl1AskAdd->SetValue( boost::lexical_cast<std::string>( values.nLvl1AskAdd ) );
  m_txtLvl1AskDel->SetValue( boost::lexical_cast<std::string>( values.nLvl1AskDel ) );

  m_txtLvl1UknTick->SetValue( boost::lexical_cast<std::string>( values.nLvl1UnkTick ) );
  //m_txtLvl1UknAdd->SetValue( boost::lexical_cast<std::string>( values.nLvl1UnkAdd ) );
  //m_txtLvl1UknDel->SetValue( boost::lexical_cast<std::string>( values.nLvl1UnkDel ) );

  m_txtLvl1BidTick->SetValue( boost::lexical_cast<std::string>( values.nLvl1BidTick ) );
  m_txtLvl1BidAdd->SetValue( boost::lexical_cast<std::string>( values.nLvl1BidAdd ) );
  m_txtLvl1BidDel->SetValue( boost::lexical_cast<std::string>( values.nLvl1BidDel ) );

  boost::format format( "%0.3f" );

  format % values.dblB0;
  m_txtB0->SetValue( format.str() );
  format % values.dblB1;
  m_txtB1->SetValue( format.str() );
  format % values.dblR;
  m_txtR->SetValue( format.str() );
}