#include "StdAfx.h"
#include "ChartControls.h"

#include <iostream>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CChartControls, CGUIFrameBase)

CChartControls::CChartControls(void) 
: CGUIFrameBase()
{
  CGUIFrameBase::SetPosition( 0, 0, 300, 100 );
  CGUIFrameBase::Create();
  m_btnNewMasterChart.Create(  _T( "New Chart" ), 0, CRect( 5, 5, 50, 20 ), this, 100 );
}

CChartControls::~CChartControls(void) {
}

BEGIN_MESSAGE_MAP(CChartControls, CGUIFrameBase)
  ON_BN_CLICKED( 100, &CChartControls::OnBtnNewMasterChartClicked )
END_MESSAGE_MAP()

void CChartControls::OnBtnNewMasterChartClicked() {
  OnBtnNewMasterChart( this );
}