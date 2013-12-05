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

#include <wx/mstream.h>
#include <wx/bitmap.h>

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
    m_panelIndex = NULL;
    m_panelTick = NULL;
    m_panelArmsVsIndex = NULL;

  m_ixActiveChart = 0;

    

/*    m_vCollections.push_back( collection_t( "DOW" ) );
    collection_t& c( m_vCollections.back() );
    c.vSymbols +=  // http://www.djindexes.com/averages/
      "MMM", "AXP", "T", "BA", "CAT", "CVX", "CSCO",
      "KO", "DD", "XOM", "GE", "GS", "HD", "INTC",
      "IBM", "JNJ", "JPM", "MCD", "MRK", "MSFT",
      "NKE", "PFE", "PG", "TRV", "UTX",
      "UNH", "VZ", "V", "WMT", "DIS";
*/
  bool b = Chart::setLicenseCode( "DEVP-2G22-4QPN-HDS6-925A-95C1" );
  assert( b );

}

void PanelArmsIndex::OnClose( wxCloseEvent& event ) {

  for ( vCollections_t::iterator iter = m_vCollections.begin(); m_vCollections.end() != iter; ++iter ) {
    if ( 0 != iter->pip ) {
      iter->pip->SetOnDrawChartIndex( 0 );
      iter->pip->SetOnDrawChartTick( 0 );
      iter->pip->SetOnDrawChartArms( 0 );
//      delete m_pip;
//      m_pip = 0;
    }
  }

  // todo:  don't close if dialog is still open.

  // Exit Steps: #2 -> FrameMain::OnClose
//  if ( 0 != OnPanelClosing ) OnPanelClosing();
  // event.Veto();  // possible call, if needed
  // event.CanVeto(); // if not a 
  event.Skip();  // auto followed by Destroy();

}

void PanelArmsIndex::CreateControls() {    

    PanelArmsIndex* itemPanel1 = this;

    m_sizerPanelArmsIndex = new wxBoxSizer(wxVERTICAL);
    itemPanel1->SetSizer(m_sizerPanelArmsIndex);

    m_splitterArmsIndex = new wxSplitterWindow( itemPanel1, ID_SplitterArmsIndex, wxDefaultPosition, wxDefaultSize, wxSP_3DBORDER|wxSP_3DSASH );
    m_splitterArmsIndex->SetMinimumPaneSize(100);

    wxPanel* itemPanel4 = new wxPanel( m_splitterArmsIndex, ID_PANEL8, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxArrayString m_lbArmsIndexStrings;
    m_lbArmsIndex = new wxListBox( itemPanel4, ID_LbArmsIndex, wxDefaultPosition, wxSize(90, 50), m_lbArmsIndexStrings, wxLB_SINGLE );

    wxPanel* itemPanel6 = new wxPanel( m_splitterArmsIndex, ID_PANEL9, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    m_sizerCharts = new wxBoxSizer(wxVERTICAL);
    itemPanel6->SetSizer(m_sizerCharts);

    m_panelIndex = new wxPanel( itemPanel6, ID_PanelIndex, wxDefaultPosition, wxSize(600, 200), wxSIMPLE_BORDER );
    m_sizerCharts->Add(m_panelIndex, 1, wxGROW|wxALL, 2);

    m_panelTick = new wxPanel( itemPanel6, ID_PanelTick, wxDefaultPosition, wxSize(600, 200), wxSIMPLE_BORDER );
    m_sizerCharts->Add(m_panelTick, 1, wxGROW|wxALL, 2);

    m_panelArmsVsIndex = new wxPanel( itemPanel6, ID_PanelArmsVsIndex, wxDefaultPosition, wxSize(600, 200), wxSIMPLE_BORDER );
    m_sizerCharts->Add(m_panelArmsVsIndex, 1, wxGROW|wxALL, 2);

    m_splitterArmsIndex->SplitVertically(itemPanel4, itemPanel6, 110);
    m_sizerPanelArmsIndex->Add(m_splitterArmsIndex, 1, wxGROW|wxALL, 2);

  Bind( wxEVT_SIZE, &PanelArmsIndex::HandleOnSize, this, this->GetId() );
  Bind( wxEVT_COMMAND_LISTBOX_SELECTED, &PanelArmsIndex::HandleListBoxSelection, this, this->GetId() );

}

void PanelArmsIndex::HandleOnSize( wxSizeEvent& event ) {
  for ( vCollections_t::iterator iter = m_vCollections.begin(); m_vCollections.end() != iter; ++iter ) {
    iter->pip->SetChartDimensions( m_panelArmsVsIndex->GetSize().GetWidth(), m_panelArmsVsIndex->GetSize().GetHeight() );
    //iter->pip->SetChartDimensions( 600, 200 );
  }
  event.Skip();
}

void PanelArmsIndex::HandleListBoxSelection( wxCommandEvent& event ) {
  int nSelection = m_lbArmsIndex->GetSelection();
  if ( wxNOT_FOUND != nSelection ) {
    m_ixActiveChart = nSelection;
  }
  
}

void PanelArmsIndex::SetProvider( pProvider_t pProvider ) {

  if ( 0 != m_vCollections.size() ) {
    throw std::runtime_error( "provider already set" );
  }
  else {
    m_pProvider = pProvider; 

    m_vCollections.push_back( collection_t( pProvider, "Dow INDU", "@YM#", "JT1T.Z", "RI1T.Z" ) );

    int ix( 0 );
    for ( vCollections_t::iterator iter = m_vCollections.begin(); m_vCollections.end() != iter; ++iter ) {
      iter->pip->SetOnDrawChartIndex( MakeDelegate( this, &PanelArmsIndex::DrawChartIndex ) );
      iter->pip->SetOnDrawChartTick( MakeDelegate( this, &PanelArmsIndex::DrawChartTick ) );
      iter->pip->SetOnDrawChartArms( MakeDelegate( this, &PanelArmsIndex::DrawChartArms ) );
      m_lbArmsIndex->Insert( iter->sName, ix );
      ++ix;
    }
  }
};

void PanelArmsIndex::UpdateGUI( void ) {
  if ( 0 != m_vCollections.size() ) {
    m_vCollections[ m_ixActiveChart ].pip->DrawCharts();
  }
}

void PanelArmsIndex::DrawChartIndex( const MemBlock& m ) {
  DrawChart( m, m_panelIndex );
}

void PanelArmsIndex::DrawChartTick( const MemBlock& m ) {
  DrawChart( m, m_panelTick );  
}

void PanelArmsIndex::DrawChartArms( const MemBlock& m ) {
  DrawChart( m, m_panelArmsVsIndex );
}

void PanelArmsIndex::DrawChart( const MemBlock& m, wxPanel* pPanel ) {
  wxMemoryInputStream in( m.data, m.len );  // need this
  wxBitmap* p = new wxBitmap( wxImage( in, wxBITMAP_TYPE_BMP) ); // and need this to keep the drawn bitmap, then memblock can be reclaimed
//  if ( 0 != m_pChartBitmap ) delete m_pChartBitmap;
//  m_pChartBitmap = event.GetBitmap();
  wxClientDC dc( pPanel );
  dc.DrawBitmap( *p, 0, 0);
  delete p;
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
