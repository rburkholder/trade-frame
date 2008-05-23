#include "StdAfx.h"
#include "ChartRealTimeContainer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Todo:  the destructor is not called

IMPLEMENT_DYNAMIC(CChartRealTimeContainer, CDialog)

CChartRealTimeContainer::CChartRealTimeContainer(
  const std::string &sSymbol,
  CProviderInterface *pDataProvider,
  CWnd* pParent /*=NULL*/
  ) 
	: CDialog(CChartRealTimeContainer::IDD, pParent),
  m_sSymbol( sSymbol ),
  m_pDataProvider( pDataProvider ),
  m_prtModel( NULL ), m_prtView( NULL ), m_prtControl( NULL )
{
  m_sDialogTitle = sSymbol + " on " + pDataProvider->Name();
  BOOL b = Create(IDD, pParent );
}

CChartRealTimeContainer::~CChartRealTimeContainer(void) {
  DestroyWindow();
}

BOOL CChartRealTimeContainer::OnInitDialog() {
  BOOL b = CDialog::OnInitDialog();
  SetWindowTextA( m_sDialogTitle.c_str() );
  m_prtModel = new CChartRealTimeModel();
  m_prtView = new CChartRealTimeView( m_prtModel );
  m_prtControl = new CChartRealTimeController( );
  m_pDataProvider->AddQuoteHandler( m_sSymbol, MakeDelegate( m_prtModel, &CChartRealTimeModel::AddQuote ) );
  m_pDataProvider->AddTradeHandler( m_sSymbol, MakeDelegate( m_prtModel, &CChartRealTimeModel::AddTrade ) );
  return b;
}

void CChartRealTimeContainer::DoDataExchange(CDataExchange* pDX) {
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_CHART, *m_prtView);
}

BEGIN_MESSAGE_MAP(CChartRealTimeContainer, CDialog)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

void CChartRealTimeContainer::OnDestroy()  {
  m_pDataProvider->RemoveTradeHandler( m_sSymbol, MakeDelegate( m_prtModel, &CChartRealTimeModel::AddTrade ) );
  m_pDataProvider->RemoveQuoteHandler( m_sSymbol, MakeDelegate( m_prtModel, &CChartRealTimeModel::AddQuote ) );
  delete m_prtControl;
  delete m_prtView;
  delete m_prtModel;
	CDialog::OnDestroy();
}

