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

    wxBoxSizer* itemBoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer1, 0, wxALIGN_LEFT|wxALL, 1);

    wxStaticText* itemStaticText1 = new wxStaticText( itemPanel1, wxID_STATIC, wxEmptyString, wxDefaultPosition, wxSize(55, -1), 0 );
    itemBoxSizer1->Add(itemStaticText1, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    m_txtTicks = new wxTextCtrl( itemPanel1, id_txtTicks, wxEmptyString, wxDefaultPosition, wxSize(50, -1), wxTE_READONLY|wxTE_RIGHT|wxNO_BORDER );
    itemBoxSizer1->Add(m_txtTicks, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxStaticText* itemStaticText4 = new wxStaticText( itemPanel1, wxID_STATIC, _("ticks"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer1->Add(itemStaticText4, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

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

    wxBoxSizer* itemBoxSizer28 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer28, 0, wxALIGN_LEFT|wxALL, 1);

    wxStaticText* itemStaticText29 = new wxStaticText( itemPanel1, wxID_STATIC, _("l2 del:"), wxDefaultPosition, wxSize(55, -1), 0 );
    itemBoxSizer28->Add(itemStaticText29, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    m_txtL2DelBid = new wxTextCtrl( itemPanel1, id_txtL2DelBid, wxEmptyString, wxDefaultPosition, wxSize(50, -1), wxTE_READONLY|wxTE_RIGHT|wxNO_BORDER );
    itemBoxSizer28->Add(m_txtL2DelBid, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxStaticText* itemStaticText22 = new wxStaticText( itemPanel1, wxID_STATIC, _("bid"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer28->Add(itemStaticText22, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    m_txtL2DelAsk = new wxTextCtrl( itemPanel1, id_txtL2DelAsk, wxEmptyString, wxDefaultPosition, wxSize(50, -1), wxTE_READONLY|wxTE_RIGHT|wxNO_BORDER );
    itemBoxSizer28->Add(m_txtL2DelAsk, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxStaticText* itemStaticText24 = new wxStaticText( itemPanel1, wxID_STATIC, _("ask"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer28->Add(itemStaticText24, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    m_txtL2DelTtl = new wxTextCtrl( itemPanel1, id_txtL2DelTtl, wxEmptyString, wxDefaultPosition, wxSize(50, -1), wxTE_READONLY|wxTE_RIGHT|wxNO_BORDER );
    itemBoxSizer28->Add(m_txtL2DelTtl, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxStaticText* itemStaticText26 = new wxStaticText( itemPanel1, wxID_STATIC, _("ttl"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer28->Add(itemStaticText26, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

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
  m_txtL2DelBid->SetValue( boost::lexical_cast<std::string>( values.nL2DelBid ) );
  m_txtL2DelAsk->SetValue( boost::lexical_cast<std::string>( values.nL2DelAsk ) );
  m_txtL2DelTtl->SetValue( boost::lexical_cast<std::string>( values.nL2DelTtl ) );
}