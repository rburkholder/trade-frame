#include "StdAfx.h"
#include "ChartRealTimeContainer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CChartRealTimeContainer, CDialog)

CChartRealTimeContainer::CChartRealTimeContainer(const std::string &sTitle, CWnd* pParent /*=NULL*/) 
	: CDialog(CChartRealTimeContainer::IDD, pParent) {
  m_sDialogTitle = sTitle;
  BOOL b = Create(IDD, pParent );}

CChartRealTimeContainer::~CChartRealTimeContainer(void) {
  DestroyWindow();
}

BOOL CChartRealTimeContainer::OnInitDialog() {
  BOOL b = CDialog::OnInitDialog();
  SetWindowTextA( m_sDialogTitle.c_str() );
  m_rtModel.OnDataChanged.Add( MakeDelegate( &m_rtView, &CChartRealTimeView::HandleModelChanged ) );
  return b;
}

void CChartRealTimeContainer::DoDataExchange(CDataExchange* pDX) {
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_CHART, m_rtView);
}

BEGIN_MESSAGE_MAP(CChartRealTimeContainer, CDialog)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

void CChartRealTimeContainer::OnDestroy()  {
  m_rtModel.OnDataChanged.Remove( MakeDelegate( &m_rtView, &CChartRealTimeView::HandleModelChanged ) );
	CDialog::OnDestroy();
}

