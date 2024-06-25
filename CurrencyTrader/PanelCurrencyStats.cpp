/************************************************************************
 * Copyright(c) 2024, One Unified. All rights reserved.                 *
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
 * File:    PanelCurrencyStats.cpp
 * Author:  raymond@burkholder.net
 * Project: CurrencyTrader
 * Created: May 12, 2024 14:37:24
 */

#include <fmt/core.h>

#include <wx/sizer.h>
#include <wx/stattext.h>

#include "PanelCurrencyStats.hpp"

PanelCurrencyStats::PanelCurrencyStats() {
  Init();
}

PanelCurrencyStats::PanelCurrencyStats( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {
  Init();
  Create(parent, id, pos, size, style);
}

PanelCurrencyStats::~PanelCurrencyStats() {
}

bool PanelCurrencyStats::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {
  wxPanel::Create( parent, id, pos, size, style );
  CreateControls();
  if ( GetSizer() ) {
    GetSizer()->SetSizeHints(this);
  }
  return true;
}

void PanelCurrencyStats::Init() {
}

PanelCurrencyStats::fUpdateCurrency_t PanelCurrencyStats::AddCurrency( const std::string& sName ) {

  mapCurrency_t::iterator iter = m_mapCurrency.find( sName );
  assert( m_mapCurrency.end() == iter );

  auto result = m_mapCurrency.emplace( sName, Currency() );
  assert( result.second );
  iter = result.first;
  Currency& currency( iter->second );

  currency.m_sizer = new wxBoxSizer( wxHORIZONTAL );
  m_sizerCurrencies->Add( currency.m_sizer, 0, wxGROW|wxLEFT|wxRIGHT|wxTOP, 2 );

  wxStaticText* name = new wxStaticText( this, wxID_ANY, sName, wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
  currency.m_sizer->Add( name, 1, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 2 );

  wxStaticText* textAmount = new wxStaticText( this, wxID_ANY, _("amount"), wxDefaultPosition, wxSize(80, -1), wxALIGN_RIGHT );
  currency.m_textAmount = textAmount;
  currency.m_sizer->Add( textAmount, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 2 );

  wxStaticText* textExtended = new wxStaticText( this, wxID_ANY, _("extended"), wxDefaultPosition, wxSize(80, -1), wxALIGN_RIGHT );
  currency.m_textExtended = textExtended;
  currency.m_sizer->Add( textExtended, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 2 );

  if ( GetSizer() ) {
    GetSizer()->SetSizeHints(this);
  }

  fUpdateCurrency_t f =
    [this, textAmount, textExtended]( double amount, double extended ){
      auto sAmount = fmt::format( "{:.{}f}", amount, 2 );
      textAmount->SetLabel( sAmount );
      auto sExtended = fmt::format( "{:.{}f}", extended, 2 );
      textExtended->SetLabel( sExtended );
    };
  return std::move( f );
}

PanelCurrencyStats::fUpdatePair_t PanelCurrencyStats::AddPair( const std::string& sName ) {

  mapPair_t::iterator iter = m_mapPair.find( sName );
  assert( m_mapPair.end() == iter );

  auto result = m_mapPair.emplace( sName, Pair() );
  assert( result.second );
  iter = result.first;
  Pair& pair( iter->second );

  pair.m_sizer = new wxBoxSizer( wxHORIZONTAL );
  m_sizerPairs->Add( pair.m_sizer, 1, wxEXPAND, 2 );

  wxStaticText* textBid = new wxStaticText( this, wxID_ANY, _("bid"), wxDefaultPosition, wxSize(70, -1), wxALIGN_RIGHT );
  pair.m_textBid = textBid;
  pair.m_sizer->Add( textBid, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3 );

  wxStaticText* textName = new wxStaticText( this, wxID_ANY, sName, wxDefaultPosition, wxSize(50, -1), wxALIGN_CENTRE );
  pair.m_sizer->Add( textName, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3 );

  wxStaticText* textAsk = new wxStaticText( this, wxID_ANY, _("ask"), wxDefaultPosition, wxSize(70, -1), wxALIGN_RIGHT );
  pair.m_textAsk = textAsk;
  pair.m_sizer->Add( textAsk, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3 );

  wxStaticText* textTradeCount = new wxStaticText( this, wxID_ANY, _("trades"), wxDefaultPosition, wxSize(30, -1 ), wxALIGN_RIGHT );
  pair.m_textTradeCount = textTradeCount;
  pair.m_sizer->Add( textTradeCount, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3 );

  wxStaticText* textCommission = new wxStaticText( this, wxID_ANY, _("commission"), wxDefaultPosition, wxSize(30, -1 ), wxALIGN_RIGHT );
  pair.m_textCommission = textCommission;
  pair.m_sizer->Add( textCommission, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3 );

  if ( GetSizer() ) {
    GetSizer()->SetSizeHints(this);
  }

  fUpdatePair_t f =
    [this, textBid, textAsk, textTradeCount, textCommission]( double bid, double ask, size_t count, double commission ){
      textBid->SetLabel( fmt::format( "{:.{}f}", bid, 5 ) );
      textAsk->SetLabel( fmt::format( "{:.{}f}", ask, 5 ) );
      textTradeCount->SetLabel( fmt::format( "{}", count ) );
      textCommission->SetLabel( fmt::format( "{:.{}f}", commission, 2 ) );
    };
  return std::move( f );
}

void PanelCurrencyStats::CreateControls() {

  PanelCurrencyStats* itemFrame1 = this;

  m_sizerPanel = new wxBoxSizer( wxHORIZONTAL );
  itemFrame1->SetSizer( m_sizerPanel );

  m_sizerCurrencies = new wxBoxSizer( wxVERTICAL );
  m_sizerPanel->Add( m_sizerCurrencies, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2 );

  m_sizerPairs = new wxBoxSizer( wxVERTICAL );
  m_sizerPanel->Add( m_sizerPairs, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2 );

}