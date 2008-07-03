#include "StdAfx.h"
#include "ChartRealTimeContainer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Todo:  the destructor is not called

IMPLEMENT_DYNAMIC(CChartRealTimeContainer, CGUIFrameBase)

CChartRealTimeContainer::CChartRealTimeContainer(
  const std::string &sSymbol,
  CProviderInterface *pDataProvider,
  CWnd* pParent /*=NULL*/
  ) 
	//: CDialog(CChartRealTimeContainer::IDD, pParent),
  : CGUIFrameBase(),
  m_sSymbol( sSymbol ),
  m_pDataProvider( pDataProvider ),
  m_prtModel( NULL ), m_prtView( NULL ), m_prtControl( NULL )
{
  m_sDialogTitle = sSymbol + " on " + pDataProvider->Name();
  //CRect rectLocation( 0, 0, 900, 400 );
  CGUIFrameBase::SetPosition( 0, 0, 900, 400 );
  //CGUIFrameBase::SetTitleBarText( 
  //BOOL b = Create(IDD, pParent );
  //BOOL b = Create(NULL, m_sDialogTitle.c_str(), WS_OVERLAPPEDWINDOW, rectLocation. );
  CGUIFrameBase::Create( );

  m_prtModel = new CChartRealTimeModel();
  m_prtView = new CChartRealTimeView( m_prtModel );
  CRect clientRect;
  CFrameWnd::GetClientRect(&clientRect);
  CRect chartRect( clientRect.left + 5, clientRect.top + 5, clientRect.right - 5, clientRect.bottom - 5 );

  m_prtView->Create( _T( "" ), SS_BITMAP, chartRect, this );
  //m_prtView->SetChartDimensions( chartRect.Width(), chartRect.Height() );
  m_prtView->ShowWindow( SW_SHOW );

  m_prtControl = new CChartRealTimeController( );
  m_pDataProvider->AddQuoteHandler( m_sSymbol, MakeDelegate( m_prtModel, &CChartRealTimeModel::AddQuote ) );
  m_pDataProvider->AddTradeHandler( m_sSymbol, MakeDelegate( m_prtModel, &CChartRealTimeModel::AddTrade ) );
}

CChartRealTimeContainer::~CChartRealTimeContainer(void) {
  m_pDataProvider->RemoveTradeHandler( m_sSymbol, MakeDelegate( m_prtModel, &CChartRealTimeModel::AddTrade ) );
  m_pDataProvider->RemoveQuoteHandler( m_sSymbol, MakeDelegate( m_prtModel, &CChartRealTimeModel::AddQuote ) );
  //DestroyWindow();
  delete m_prtView;
  delete m_prtControl;
  delete m_prtModel;
}

BEGIN_MESSAGE_MAP(CChartRealTimeContainer, CGUIFrameBase)
	ON_WM_DESTROY()
  ON_WM_SIZE( )
  //ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

void CChartRealTimeContainer::OnDestroy()  {
	CGUIFrameBase::OnDestroy();
}

afx_msg void CChartRealTimeContainer::OnSize(UINT nType, int cx, int cy) {
  CGUIFrameBase::OnSize(nType,cx,cy);
  CRect clientRect;
  CFrameWnd::GetClientRect(&clientRect);
  CRect chartRect( clientRect.left + 5, clientRect.top + 5, clientRect.right - 5, clientRect.bottom - 5 );
  m_prtView->SetChartDimensions( chartRect.Width(), chartRect.Height() );
  //std::cout << "size " << chartRect.Width() << ", " << chartRect.Height() << endl;
}

