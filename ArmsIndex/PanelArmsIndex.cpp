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

#include <boost/assign/std/vector.hpp>
using namespace boost::assign;

#include "PanelArmsIndex.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

PanelArmsIndex::PanelArmsIndex(void) {
  Init();
}

PanelArmsIndex::PanelArmsIndex( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {
  Init();
  Create(parent, id, pos, size, style);
}

PanelArmsIndex::~PanelArmsIndex(void) {
}

bool PanelArmsIndex::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {

    wxPanel::Create( parent, id, pos, size, style );

    CreateControls();
    if (GetSizer())     {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
    return true;
}

void PanelArmsIndex::Init() {

    m_sizerPanelArmsIndex = NULL;
    m_splitterArmsIndex = NULL;
    m_lbArmsIndex = NULL;
    m_sizerCharts = NULL;
    m_panelArmsVsIndex = NULL;
    m_panelTick = NULL;

    m_vCollections.push_back( collection_t( "DOW" ) );
    collection_t& c( m_vCollections.back() );
    c.vSymbols +=  // http://www.djindexes.com/averages/
      "MMM", "AXP", "T", "BA", "CAT", "CVX", "CSCO",
      "KO", "DD", "XOM", "GE", "GS", "HD", "INTC",
      "IBM", "JNJ", "JPM", "MCD", "MRK", "MSFT",
      "NKE", "PFE", "PG", "TRV", "UTX",
      "UNH", "VZ", "V", "WMT", "DIS";
}

void PanelArmsIndex::OnClose( wxCloseEvent& event ) {

  // todo:  don't close if dialog is still open.

  // Exit Steps: #2 -> FrameMain::OnClose
//  if ( 0 != OnPanelClosing ) OnPanelClosing();
  // event.Veto();  // possible call, if needed
  // event.CanVeto(); // if not a 
  event.Skip();  // auto followed by Destroy();

}

void PanelArmsIndex::CreateControls() {    

    PanelArmsIndex* itemPanel1 = this;

    m_sizerPanelArmsIndex = new wxBoxSizer(wxHORIZONTAL);
    itemPanel1->SetSizer(m_sizerPanelArmsIndex);

    m_splitterArmsIndex = new wxSplitterWindow( itemPanel1, ID_SplitterArmsIndex, wxDefaultPosition, wxSize(100, 100), wxSP_3DBORDER|wxSP_3DSASH|wxNO_BORDER );
    m_splitterArmsIndex->SetMinimumPaneSize(20);

    wxPanel* itemPanel4 = new wxPanel( m_splitterArmsIndex, ID_PANEL8, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxArrayString m_lbArmsIndexStrings;
    m_lbArmsIndex = new wxListBox( itemPanel4, ID_LbArmsIndex, wxDefaultPosition, wxDefaultSize, m_lbArmsIndexStrings, wxLB_SINGLE );

    wxPanel* itemPanel6 = new wxPanel( m_splitterArmsIndex, ID_PANEL9, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    m_sizerCharts = new wxBoxSizer(wxVERTICAL);
    itemPanel6->SetSizer(m_sizerCharts);

    m_panelArmsVsIndex = new wxPanel( itemPanel6, ID_PanelArmsVsIndex, wxDefaultPosition, wxSize(200, 100), 0 );
    m_sizerCharts->Add(m_panelArmsVsIndex, 1, wxGROW|wxALL, 2);

    m_panelTick = new wxPanel( itemPanel6, ID_PanelTick, wxDefaultPosition, wxSize(200, 100), 0 );
    m_sizerCharts->Add(m_panelTick, 1, wxGROW|wxALL, 2);

    m_splitterArmsIndex->SplitVertically(itemPanel4, itemPanel6, 50);

}

wxBitmap PanelArmsIndex::GetBitmapResource( const wxString& name ) {
    wxUnusedVar(name);
    return wxNullBitmap;
}

wxIcon PanelArmsIndex::GetIconResource( const wxString& name ) {
    wxUnusedVar(name);
    return wxNullIcon;
}

} // namespace tf
} // namespace ou
