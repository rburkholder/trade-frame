// VuMarketDepth.cpp : implementation file
//

#include "stdafx.h"
//#include "GTScalp.h"
#include "VuMarketDepth.h"


// CVuMarketDepth dialog

IMPLEMENT_DYNAMIC(CVuMarketDepth, CDialog)

CVuMarketDepth::CVuMarketDepth( const std::string &sTitle, CWnd* pParent /*=NULL*/ )
	: CDialog(CVuMarketDepth::IDD, pParent) {

    m_sChartTitle = sTitle;

    BOOL b = Create(IDD_VUMARKETDEPTH, pParent );
}

CVuMarketDepth::~CVuMarketDepth() {
  DestroyWindow();
}

void CVuMarketDepth::DoDataExchange(CDataExchange* pDX) {
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_MARKETDEPTHBID, m_lcMarketDepthBid);
  DDX_Control(pDX, IDC_MARKETDEPTHASK, m_lcMarketDepthAsk);
}

BOOL CVuMarketDepth::OnInitDialog() {
  BOOL b = CDialog::OnInitDialog();

  SetWindowTextA( m_sChartTitle.c_str() );

  m_lcMarketDepthAsk.InitControl();
  m_lcMarketDepthAsk.DrawColumns();
  m_lcMarketDepthBid.InitControl();
  m_lcMarketDepthBid.DrawColumns();

  CRect rect1, rect2, rect3;
  GetClientRect( &rect1 );
  rect2.SetRect( 5, 5, ( rect1.Width() - 10 ) / 2 + 5, rect1.bottom - 5 );
  m_lcMarketDepthBid.MoveWindow( &rect2 );
  rect3.SetRect( ( rect1.Width() - 10 ) / 2 + 5, 5, rect1.right - 5, rect1.bottom - 5 );
  m_lcMarketDepthAsk.MoveWindow( &rect3 );

  m_lcMarketDepthBid.Resize();
  m_lcMarketDepthAsk.Resize();

  return b;
}

BEGIN_MESSAGE_MAP(CVuMarketDepth, CDialog)
END_MESSAGE_MAP()


// CVuMarketDepth message handlers
