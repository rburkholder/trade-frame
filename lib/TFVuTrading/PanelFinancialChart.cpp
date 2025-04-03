/************************************************************************
 * Copyright(c) 2011, One Unified. All rights reserved.                 *
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

#include <stdexcept>

#include <wx/icon.h>
#include <wx/sizer.h>
#include <wx/treectrl.h>

#include <TFVuTrading/TreeItem.hpp>

#include "PanelFinancialChart.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

PanelFinancialChart::PanelFinancialChart()
: wxPanel(), m_pWinChartView( nullptr )
{
}

PanelFinancialChart::PanelFinancialChart( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
: wxPanel(), m_pWinChartView( nullptr )
{
  Create(parent, id, pos, size, style);
}

PanelFinancialChart::~PanelFinancialChart() {
  m_pWinChartView->SetChartDataView( nullptr );
}

bool PanelFinancialChart::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {

  wxPanel::Create( parent, id, pos, size, style );

  CreateControls();
  if (GetSizer())     {
      GetSizer()->SetSizeHints(this);
  }
  return true;
}

void PanelFinancialChart::CreateControls() {

  PanelFinancialChart* itemPanel1 = this;

  wxBoxSizer* sizerMain = new wxBoxSizer( wxVERTICAL );
  itemPanel1->SetSizer( sizerMain );

  // splitter
  static const int pxLeftPanelSize = 150;
  m_pSplitter = new wxSplitterWindow( this );
  m_pSplitter->SetMinimumPaneSize( pxLeftPanelSize );
  m_pSplitter->SetSashGravity( 0.0 );
  m_pSplitter->Bind( // seems to be the way for state recovery of sash position
    wxEVT_SPLITTER_SASH_POS_CHANGING,  // during sash dragging
    [this]( wxSplitterEvent& event ){
      const int sash = m_pSplitter->GetSashSize();
      const int min = m_pSplitter->GetMinimumPaneSize();
      const int pos = event.GetSashPosition();
      const int w3 = GetSize().GetWidth();
      const int w4 = m_pSplitter->GetSize().GetWidth();
      const int width = w4 - sash - min;
      const double gravity = (double)pos / (double)width;

      //std::cout
      //  << "wxEVT_SPLITTER_SASH_POS_CHANGING"
      //  << ',' << gravity
      //  << ',' << pos
      //  << ',' << w3 << ',' << w4
      //  << std::endl;
      m_pSplitter->SetSashGravity( gravity );
      event.Skip();
    } );


  // wxTreeCtrl* tree;
  m_pTree = new wxTreeCtrl( m_pSplitter, wxID_ANY, wxDefaultPosition, wxSize(100, 100), wxTR_HAS_BUTTONS |wxTR_TWIST_BUTTONS|wxTR_SINGLE );
  TreeItem::Bind( this, m_pTree );
  m_pTree->ExpandAll();
  m_pTree->Bind( wxEVT_COMMAND_TREE_ITEM_GETTOOLTIP, &PanelFinancialChart::HandleTreeEventItemGetToolTip, this, m_pTree->GetId() ); //wxEVT_COMMAND_TREE_ITEM_GETTOOLTIP     wxEVT_TREE_ITEM_GETTOOLTIP

  // panel for right side of splitter
  m_pWinChartView = new WinChartView( m_pSplitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );

  m_pSplitter->SplitVertically( m_pTree, m_pWinChartView, 0 );
  sizerMain->Add( m_pSplitter, 1, wxEXPAND|wxALL, 2);
  m_pSplitter->SetSashPosition( pxLeftPanelSize );

  Bind( wxEVT_DESTROY, &PanelFinancialChart::OnDestroy, this );
}

TreeItem* PanelFinancialChart::SetRoot( const std::string& sName, pChartDataView_t pChartDataView ) {

  wxTreeItemId id =  m_pTree->GetRootItem();

  if ( id.IsOk() ) {
    throw std::runtime_error( "root item already exists" );
  }
  else {
    m_pTreeItem = new TreeItem( m_pTree, sName );
    m_pTreeItem->SetOnClick(
      [this, pChartDataView]( TreeItem* pti ){
        m_pWinChartView->SetChartDataView( pChartDataView.get() );
      });
    //m_pTree->Expand( id );
  }
  return m_pTreeItem;
}

void PanelFinancialChart::SetChartDataView( pChartDataView_t pChartDataView, bool bReCalcViewPort  ) {
  m_pWinChartView->SetChartDataView( pChartDataView.get(), bReCalcViewPort );
  m_pChartDataView = pChartDataView;
}

void PanelFinancialChart::HandleTreeEventItemGetToolTip( wxTreeEvent& event ) {
  if ( m_pChartDataView ) {
    event.SetToolTip( m_pChartDataView->GetName() );
  }
  event.Skip();
}

void PanelFinancialChart::OnDestroy( wxWindowDestroyEvent& event ) {
  m_pTree->Unbind( wxEVT_COMMAND_TREE_ITEM_GETTOOLTIP, &PanelFinancialChart::HandleTreeEventItemGetToolTip, this, m_pTree->GetId() ); //wxEVT_COMMAND_TREE_ITEM_GETTOOLTIP     wxEVT_TREE_ITEM_GETTOOLTIP
  //TreeItem::UnBind( this, m_pTree );
  m_pTree->DeleteAllItems();
  Unbind( wxEVT_DESTROY, &PanelFinancialChart::OnDestroy, this );
  event.Skip();  // auto followed by Destroy();
}

wxBitmap PanelFinancialChart::GetBitmapResource( const wxString& name ) {
  wxUnusedVar(name);
  return wxNullBitmap;
}

wxIcon PanelFinancialChart::GetIconResource( const wxString& name ) {
  wxUnusedVar(name);
  return wxNullIcon;
}

} // namespace tf
} // namespace ou
