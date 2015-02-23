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

//#include "StdAfx.h"

#include "ChartingContainer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// make the container a singleton instead of the tree view, makes for more consistent design

CChartingContainer::CChartingContainer(void) {
  //m_pTreeView = CChartRealTimeTreeView::Register();  // get the tree view started
  m_TreeView.m_Tree.OnClick.Add( MakeDelegate( this, &CChartingContainer::HandleSelectDataView ) );
  m_TreeView.ShowWindow( SW_SHOWNORMAL );
  m_ChartControls.OnBtnNewMasterChart.Add( MakeDelegate( this, &CChartingContainer::HandleCreateNewViewPort ) );
  m_ChartControls.ShowWindow( SW_SHOWNORMAL );
  CreateNewViewPort();
}

CChartingContainer::~CChartingContainer(void) {
  for ( std::vector<CChartViewPort *>::iterator iter = m_vViewPorts.begin(); m_vViewPorts.end() != iter; ++iter ) {
    delete *iter;
  }
  m_ChartControls.OnBtnNewMasterChart.Remove( MakeDelegate( this, &CChartingContainer::HandleCreateNewViewPort ) );
  m_TreeView.m_Tree.OnClick.Remove( MakeDelegate( this, &CChartingContainer::HandleSelectDataView ) );
  //CChartRealTimeTreeView::Deregister();
}

void CChartingContainer::HandleCreateNewViewPort(CChartControls *pControls) {
  CreateNewViewPort();
}

void CChartingContainer::CreateNewViewPort( void ) {
  m_ixActiveViewPort = m_vViewPorts.size();  // make new viewport active
  CChartViewPort *pViewPort = new CChartViewPort( NULL );  // nothing to view yet
  pViewPort->ShowWindow( SW_SHOWNORMAL );
  //pViewPort->SetViewPortIndex( m_ixActiveViewPort );
  m_vViewPorts.push_back( pViewPort );
}

void CChartingContainer::HandleSelectActiveViewPort(CChartViewPort *pViewPort) {
}

void CChartingContainer::HandleSelectDataView(CChartDataView *pDataView) {
  m_vViewPorts[ m_ixActiveViewPort ]->SetChartDataView( pDataView );
}