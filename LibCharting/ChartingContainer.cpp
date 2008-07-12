#include "StdAfx.h"
#include "ChartingContainer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// make the container a singleton instead of the tree view, makes for more consistent design

CChartingContainer::CChartingContainer(void) {
  m_pTreeView = CChartRealTimeTreeView::Register();  // get the tree view started
  m_ChartControls.OnBtnNewMasterChart.Add( MakeDelegate( this, &CChartingContainer::HandleCreateNewViewPort ) );
  CreateNewViewPort();
}

CChartingContainer::~CChartingContainer(void) {
  m_ChartControls.OnBtnNewMasterChart.Remove( MakeDelegate( this, &CChartingContainer::HandleCreateNewViewPort ) );
  CChartRealTimeTreeView::Deregister();
}

void CChartingContainer::HandleCreateNewViewPort(CChartControls *pControls) {
  CreateNewViewPort();
}

void CChartingContainer::CreateNewViewPort( void ) {
  m_ixActiveViewPort = m_vViewPorts.size();
  CChartViewPort *pViewPort = new CChartViewPort( m_ixActiveViewPort );
  //pViewPort->SetViewPortIndex( m_ixActiveViewPort );
  m_vViewPorts.push_back( pViewPort );
}