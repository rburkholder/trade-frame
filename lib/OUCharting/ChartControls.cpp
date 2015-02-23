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

//#include "stdafx.h"
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