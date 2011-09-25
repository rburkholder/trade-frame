// VuChart.cpp : implementation file
//

#include "stdafx.h"
//#include "GTScalp.h"
#include "VuChart.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CVuChart dialog

IMPLEMENT_DYNAMIC(CVuChart, CDialog)

CVuChart::CVuChart(const std::string &sTitle, CWnd* pParent /*=NULL*/)
	: CDialog(CVuChart::IDD, pParent) {

    m_sChartTitle = sTitle;
  BOOL b = Create(IDD, pParent );
}

CVuChart::~CVuChart() {

  DestroyWindow();
}

BOOL CVuChart::OnInitDialog() {
  BOOL b = CDialog::OnInitDialog();
  SetWindowTextA( m_sChartTitle.c_str() );
  return b;
}

void CVuChart::DoDataExchange(CDataExchange* pDX) {
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_CHART, m_chart);
}


BEGIN_MESSAGE_MAP(CVuChart, CDialog)
  ON_STN_CLICKED(IDC_CHART, &CVuChart::OnStnClickedChart)
END_MESSAGE_MAP()


// CVuChart message handlers

void CVuChart::OnStnClickedChart()
{
  // TODO: Add your control notification handler code here
}
