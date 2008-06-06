// GTScalpDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GTScalp.h"
#include "GTScalpDlg.h"
#include "Color.h"
#include "IQFeed.h" 
//#include "CtlPendingOrders.h"
#include "VuPendingOrders.h"
//#include "VuKeyValues.h"
#include "IQFeedSymbol.h"
//#include "IQFeedOptions.h"
#include "IQFeedRetrieveHistory.h"
#include "IQFeedSymbolFile.h"
#include "ChartDatedDatum.h"
#include "ChartRealTimeContainer.h"
#include "InstrumentFile.h"

#include <iostream>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog {
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()



//
// CGTScalpDlg dialog
//



CGTScalpDlg::CGTScalpDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGTScalpDlg::IDD, pParent)
  , SomeTextVal(_T(";;"))
  , m_pIQFeedSingleton( NULL )
  , m_pIQFeed( NULL )
  , m_pIB( NULL )
  , m_pBasketTrade( NULL )
  , m_pExecutionProvider( NULL )
  , m_pDataProvider( NULL )
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

}

void CGTScalpDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_GTSESSION1, theApp.m_session1);
  DDX_Control(pDX, IDC_GTSESSION2, theApp.m_session2);
  DDX_Control(pDX, IDC_LBEXECADDR, m_lbExecAddr);
  DDX_Control(pDX, IDC_LBEXECPORT, m_lbExecPort);
  DDX_Control(pDX, IDC_LBLVL1ADDR, m_lbLvl1Addr);
  DDX_Control(pDX, IDC_LBLVL2ADDR, m_lbLvl2Addr);
  DDX_Control(pDX, IDC_LBLVL1PORT, m_lbLvl1Port);
  DDX_Control(pDX, IDC_LBLVL2PORT, m_lbLvl2Port);
  DDX_Control(pDX, IDC_LIVE, m_btnLive);
  DDX_Control(pDX, IDC_DAYSAGO, m_edtDaysAgo);
  DDX_Control(pDX, IDC_ALLOWTRADES, m_cbAllowTrades);
  DDX_Control(pDX, IDC_SYMBOLLIST, m_lbSymbolList);
  DDX_Control(pDX, IDC_IQCOMMANDLIST, m_lbIQCommands);
  DDX_Control(pDX, IDC_ENTRY1, m_edtEntry1);
  DDX_Control(pDX, IDC_DATETIMEPICKER3, m_dtLastDate);
  DDX_Control(pDX, IDC_DATETIMEPICKER2, m_dtLastTime);
  DDX_Control(pDX, IDC_DATETIMEPICKER4, m_dtStartDate);
  DDX_Control(pDX, IDC_DATETIMEPICKER5, m_dtStartTime);
  DDX_Control(pDX, IDC_STATICRB, m_grpEndDay);
  DDX_Control(pDX, IDC_SCANTYPE, m_grpScanType);
  DDX_Control(pDX, IDC_USEDAYSTART, m_cbUseDayStart);
  DDX_Control(pDX, IDC_USEDAYEND, m_cbUseDayEnd);
  DDX_Control(pDX, IDC_ENDDAYSELECT, m_rbSelectByDay);
  DDX_Control(pDX, IDC_ENDBARCOUNT, m_rbSelectByBarCount);
  DDX_Control(pDX, IDC_ENDDAYCOUNT, m_rbSelectByDayCount);
  DDX_Control(pDX, IDC_IBACCT, m_edtIBAcctCode);
  DDX_Control(pDX, IDC_GRPDATASOURCE, m_grpDataSource);
  DDX_Control(pDX, IDC_EDTFUNDS, m_edtFunds);
  DDX_Control(pDX, IDC_GRPEXECUTION, m_grpExecution);
  DDX_Control(pDX, IDC_CBOUTSIDERTH, m_cbOutsideRTH);
  DDX_Control(pDX, IDC_GRPORDERSIDE, m_grpOrderSide);
  DDX_Control(pDX, IDC_GRPTRADETYPE, m_grpTradeType);
  DDX_Control(pDX, IDC_EBLIMITPRICE, m_ebLimitPrice);
  DDX_Control(pDX, IDC_EBSTOPPRICE, m_ebStopPrice);
  DDX_Control(pDX, IDC_EDTPASSWORD, m_edtPassword);
  DDX_Control(pDX, IDC_EXECIB, m_btnIBExecution);
  DDX_Control(pDX, IDC_DSIB, m_btnIBData);
  DDX_Control(pDX, IDC_DSIQFEED, m_btnIQFeedData);
  DDX_Control(pDX, IDC_IQFWINDOWS, m_btnIQFWindows);
  DDX_Control(pDX, IDC_EDTORGSYMBOLNAME, m_edtOriginalSymbolName);
  DDX_Control(pDX, IDC_EDTALTSYMBOLNAME, m_edtAlternateSymbolName);
}

BEGIN_MESSAGE_MAP(CGTScalpDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
  ON_WM_DESTROY( )
	//}}AFX_MSG_MAP
  ON_BN_CLICKED(IDC_BTNLOGIN, &CGTScalpDlg::OnBnClickedLogin)
  ON_BN_CLICKED(IDC_LOGOUT, &CGTScalpDlg::OnBnClickedLogout)
  ON_BN_CLICKED(IDC_BTNSTART, &CGTScalpDlg::OnBnClickedStart)
  ON_BN_CLICKED(IDC_ACCOUNTS, &CGTScalpDlg::OnBnClickedAccounts)
  ON_BN_CLICKED(IDC_IQFEED, &CGTScalpDlg::OnBnClickedIqfeed)
  ON_BN_CLICKED(IDC_BTNPENDORD, &CGTScalpDlg::OnBnClickedBtnpendord)
  ON_BN_CLICKED(IDC_BTN_OPTIONS, &CGTScalpDlg::OnBnClickedBtnOptions)
  ON_BN_CLICKED(IDC_BTNHISTORY, &CGTScalpDlg::OnBnClickedBtnhistory)
  ON_BN_CLICKED(IDC_VUARMS, &CGTScalpDlg::OnBnClickedVuarms)
  ON_BN_CLICKED(IDC_LIVE, &CGTScalpDlg::OnBnClickedLive)
  ON_EN_CHANGE(IDC_DAYSAGO, &CGTScalpDlg::OnEnChangeDaysago)
  ON_BN_CLICKED(IDC_ALLOWTRADES, &CGTScalpDlg::OnBnClickedAllowtrades)
  ON_CBN_SELCHANGE(IDC_SYMBOLLIST, &CGTScalpDlg::OnCbnSelchangeSymbollist)
  ON_MESSAGE( WM_PERIODICREFRESH, &CGTScalpDlg::OnPeriodicRefresh )
  ON_BN_CLICKED(IDC_IQFEEDLOADSYMBOLS, &CGTScalpDlg::OnBnClickedIqfeedloadsymbols)
  ON_BN_CLICKED(IDC_LOADDAILYDATA, &CGTScalpDlg::OnBnClickedLoaddailydata)
  ON_BN_CLICKED(IDC_DNLDDAYSYMBOL, &CGTScalpDlg::OnBnClickedDnlddaysymbol)
  ON_BN_CLICKED(IDC_IQFEEDCMD, &CGTScalpDlg::OnBnClickedIqfeedcmd)
  ON_BN_CLICKED(IDC_ENDDAYSELECT, &CGTScalpDlg::OnBnClickedEnddayselect)
  ON_BN_CLICKED(IDC_ENDBARCOUNT, &CGTScalpDlg::OnBnClickedEndbarcount)
  ON_BN_CLICKED(IDC_ENDDAYCOUNT, &CGTScalpDlg::OnBnClickedEnddaycount)
  ON_BN_CLICKED(IDC_BTNSCAN, &CGTScalpDlg::OnBnClickedBtnscan)
  ON_BN_CLICKED(IDC_RBDARVAS, &CGTScalpDlg::OnBnClickedRbdarvas)
  ON_BN_CLICKED(IDC_RBBOLLINGER, &CGTScalpDlg::OnBnClickedRbbollinger)
  ON_BN_CLICKED(IDC_RBBREAKOUT, &CGTScalpDlg::OnBnClickedRbbreakout)
  ON_BN_CLICKED(IDC_USEDAYSTART, &CGTScalpDlg::OnBnClickedUsedaystart)
  ON_BN_CLICKED(IDC_USEDAYEND, &CGTScalpDlg::OnBnClickedUsedayend)
  ON_BN_CLICKED(IDC_OPENIB, &CGTScalpDlg::OnBnClickedOpenib)
  ON_BN_CLICKED(IDC_RADIO1, &CGTScalpDlg::OnBnClickedRadio1)
  ON_BN_CLICKED(IDC_RADIO2, &CGTScalpDlg::OnBnClickedRadio2)
  ON_BN_CLICKED(IDC_IBWATCH, &CGTScalpDlg::OnBnClickedIbwatch)
  ON_BN_CLICKED(IDC_IBUNWATCH, &CGTScalpDlg::OnBnClickedIbunwatch)
  ON_BN_CLICKED(IDC_IBCLOSE, &CGTScalpDlg::OnBnClickedIbclose)
  ON_BN_CLICKED(IDC_DSIQFEED, &CGTScalpDlg::OnBnClickedDsiqfeed)
  ON_BN_CLICKED(IDC_DSIB, &CGTScalpDlg::OnBnClickedDsib)
  ON_BN_CLICKED(IDC_DSGT1, &CGTScalpDlg::OnBnClickedDsgt1)
  ON_BN_CLICKED(IDC_DSGT2, &CGTScalpDlg::OnBnClickedDsgt2)
  ON_BN_CLICKED(IDC_CHARTSYMBOL, &CGTScalpDlg::OnBnClickedChartsymbol)
  ON_EN_CHANGE(IDC_IBACCT, &CGTScalpDlg::OnEnChangeIbacct)
  ON_BN_CLICKED(IDC_RTCHART, &CGTScalpDlg::OnBnClickedRtchart)
  ON_BN_CLICKED(IDC_BASKET, &CGTScalpDlg::OnBnClickedBasket)
  ON_BN_CLICKED(IDC_BASKETPREPARE, &CGTScalpDlg::OnBnClickedBasketprepare)
  ON_BN_CLICKED(IDC_ADDTESTSYMBOLS, &CGTScalpDlg::OnBnClickedAddtestsymbols)
  ON_BN_CLICKED(IDC_EXECIB, &CGTScalpDlg::OnBnClickedExecib)
  ON_BN_CLICKED(IDC_EXECGENESIS1, &CGTScalpDlg::OnBnClickedExecgenesis1)
  ON_BN_CLICKED(IDC_EXECGENESIS2, &CGTScalpDlg::OnBnClickedExecgenesis2)
  ON_BN_CLICKED(IDC_EXEC4, &CGTScalpDlg::OnBnClickedExec4)
  ON_BN_CLICKED(IDC_EXEC5, &CGTScalpDlg::OnBnClickedExec5)
  ON_BN_CLICKED(IDC_CBOUTSIDERTH, &CGTScalpDlg::OnBnClickedCboutsiderth)
  ON_BN_CLICKED(IDC_RBOSBUY, &CGTScalpDlg::OnBnClickedRbosbuy)
  ON_BN_CLICKED(IDC_RBOSSELL, &CGTScalpDlg::OnBnClickedRbossell)
  ON_BN_CLICKED(IDC_RBTTMKT, &CGTScalpDlg::OnBnClickedRbttmkt)
  ON_BN_CLICKED(IDC_RBTTLMT, &CGTScalpDlg::OnBnClickedRbttlmt)
  ON_BN_CLICKED(IDC_RBTTSTP, &CGTScalpDlg::OnBnClickedRbttstp)
  ON_BN_CLICKED(IDC_RBTTSTPLMIT, &CGTScalpDlg::OnBnClickedRbttstplmit)
  ON_BN_CLICKED(IDC_RBTT01, &CGTScalpDlg::OnBnClickedRbtt01)
  ON_BN_CLICKED(IDC_RBTT02, &CGTScalpDlg::OnBnClickedRbtt02)
  ON_BN_CLICKED(IDC_BTNORDER, &CGTScalpDlg::OnBnClickedBtnorder)
  ON_BN_CLICKED(IDC_IQFWINDOWS, &CGTScalpDlg::OnBnClickedIqfwindows)
  ON_BN_CLICKED(IDC_BTNSAVESYMBOL, &CGTScalpDlg::OnBnClickedBtnsavesymbol)
END_MESSAGE_MAP()


// CGTScalpDlg message handlers

BOOL CGTScalpDlg::OnInitDialog() {
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

//  pTradingLogic = NULL;

  options = NULL;
  pvpo = NULL;
  pvi = NULL;
  psoi = NULL;
  pvuArms = NULL;
  pNews = NULL;

  theApp.m_bLive = true;
  m_btnLive.SetCheck( BST_CHECKED );
  m_cbAllowTrades.SetCheck( BST_UNCHECKED );

  //char buf[ 11 ];
  //m_edtDaysAgo.GetLine( 0, buf, 10 );
  m_edtDaysAgo.SetWindowTextA( "1" );

  // looks like addresses for exec, level1, and level2
  char *GTAddress[] = { 
    "76.8.64.2","76.8.64.3","76.8.64.4","69.64.202.155","69.64.202.156","69.64.202.157","" };
  // preferred ports for level2
  int ExecPort[] = { 15805,16805,16705,16605,16505,16405,16305,16205,16105,16005,15905,15305,15405,15505,15605,15705,17205,0 };

  // preferred ports for level 1 and level 2
  int Level1Port0[] = {16811,26811,36811,46811,56811,17811,27811,37811,47811,57811,0};
  int Level2Port0[] = {16810,26810,36810,46810,56810,17810,27810,37810,47810,57810,0};

  // any combination of the following
  char *Level2Address1[] = { "69.64.202.155","69.64.202.156","69.64.202.157",""};
  // alternate ports
  int Level2Port1[] = { 16324,26324,0};

  unsigned short ix = 0;
  char *pstr = GTAddress[ix];
  while ( 0 != *pstr ) {
    m_lbExecAddr.AddString( pstr );
    m_lbLvl1Addr.AddString( pstr );
    m_lbLvl2Addr.AddString( pstr );
    pstr = GTAddress[++ix];
  }

  ix = 0;
  pstr = Level2Address1[ix];
  while ( 0 != *pstr ) {
    m_lbLvl2Addr.AddString( pstr );
    pstr = Level2Address1[ ++ix ];
  }

  m_lbExecAddr.SetCurSel( 3 );
  m_lbLvl1Addr.SetCurSel( 0 );
  m_lbLvl2Addr.SetCurSel( 0 );

  CString s;
  int *pport = ExecPort;
  while ( 0 != *pport ) {
    s.Format( "%d", *pport );
    m_lbExecPort.AddString( LPCTSTR( s ) );
    pport++;
  }

  pport = Level1Port0;
  while ( 0 != *pport ) {
    s.Format( "%d", *pport );
    m_lbLvl1Port.AddString( LPCTSTR( s ) );
    pport++;
  }

  pport = Level2Port0;
  while ( 0 != *pport ) {
    s.Format( "%d", *pport );
    m_lbLvl2Port.AddString( LPCTSTR( s ) );
    pport++;
  }

  pport = Level2Port1;
  while ( 0 != *pport ) {
    s.Format( "%d", *pport );
    m_lbLvl2Port.AddString( LPCTSTR( s ) );
    pport++;
  }

  m_lbExecPort.SetCurSel( 5 );
  m_lbLvl1Port.SetCurSel( 0 );
  m_lbLvl2Port.SetCurSel( 1 );

  m_eDayCalc = CSymbolSelectionFilter::NoDayCalc;
  m_eScanType = NoScanType;
  m_bUseDayStart = false;
  m_bUseDayEnd = false;

  m_eDataSourceType = NoDS;
  m_eExecutionType = NoExec;
  //pChartIntraDay = NULL;

  m_refresh.SetThreadWindow( theApp.m_pMainWnd );

  m_bOutsideRTH = false;

  m_eOrderSide = OrderSide::Unknown;
  m_eOrderType = OrderType::Unknown;

  return TRUE;  // return TRUE  unless you set the focus to a control
}

void CGTScalpDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CGTScalpDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);


	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CGTScalpDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

afx_msg void CGTScalpDlg::OnCancel() {  // With System 'X' or with Cancel button
  //CDialog::OnCancel();
  //CleanUpStuff();
  CDialog::DestroyWindow();
}

afx_msg void CGTScalpDlg::OnClose() {  // not called knowingly
  CDialog::OnClose();
}

afx_msg void CGTScalpDlg::OnOK() {  // with OK button
  // CWnd::UpdateData  when doing Exchange type activities
  //CDialog::OnOK();  // Don't call the base class CDialog::OnCancel, because it calls EndDialog, which will make the dialog box invisible but will not destroy it.
  //CleanUpStuff();
  CDialog::DestroyWindow();
}

afx_msg LRESULT CGTScalpDlg::OnPeriodicRefresh( WPARAM w, LPARAM l ) {
  m_refresh.HandleRefresh();
  return 1;
}

afx_msg void CGTScalpDlg::OnDestroy( ) {

  m_refresh.ResetThreadWindow();

  for each ( CTradingLogic *p in m_vTradingLogic ) {
    delete p;
  }
  m_vTradingLogic.clear();
  if ( NULL != options ) {  // should this be close/destroy instead?
    delete options;
    options = NULL;
  }
  if ( NULL != pvpo ) {  // should this be close/destroy instead?
    delete pvpo;
    pvpo = NULL;
  }
  if ( NULL != pvi ) {  // should this be close/destroy instead?
    delete pvi;
    pvi = NULL;
  }
  if ( NULL != psoi ) {
    delete psoi;
    psoi = NULL;
  }
  if ( NULL != pvuArms ) {
    delete pvuArms;
    pvuArms = NULL;
  }
  if ( NULL != pNews ) {
    delete pNews;
    pNews = NULL;
  }
  if ( NULL != m_pBasketTrade ) {
    delete m_pBasketTrade;
    m_pBasketTrade = NULL;
  }
  if ( NULL != m_pIQFeedSingleton ) {
    //delete m_pIQFeed;  // don't do here else we'll be doing it twice
    //m_pIQFeed = NULL;
    delete m_pIQFeedSingleton;
    m_pIQFeedSingleton = NULL;
  }
  if ( NULL != m_pIB ) {
    delete m_pIB;
    m_pIB = NULL;
  }
}

void CGTScalpDlg::OnBnClickedLogin()
{
  int result = 0;

  bLoggedIn = true;
  CString s;

  try {
    char buffer1[20];
    char buffer2[20];
    unsigned short nPort;

    m_lbExecAddr.GetText( m_lbExecAddr.GetCurSel(), buffer1 );
    m_lbExecPort.GetText( m_lbExecPort.GetCurSel(), buffer2 );
    nPort = atoi( buffer2 );
    theApp.m_session1.m_setting.SetExecAddress(buffer1,nPort);

    m_lbLvl1Addr.GetText( m_lbLvl1Addr.GetCurSel(), buffer1 );
    m_lbLvl1Port.GetText( m_lbLvl1Port.GetCurSel(), buffer2 );
    nPort = atoi( buffer2 );
    theApp.m_session1.m_setting.SetQuoteAddress(buffer1,nPort);

    m_lbLvl2Addr.GetText( m_lbLvl2Addr.GetCurSel(), buffer1 );
    m_lbLvl2Port.GetText( m_lbLvl2Port.GetCurSel(), buffer2 );
    nPort = atoi( buffer2 );
    theApp.m_session1.m_setting.SetLevel2Address(buffer1,nPort);

    m_edtPassword.GetWindowTextA( buffer1, 20 );
    result = theApp.m_session1.Login( "OURB001", buffer1 );
    std::cout << "Login result (sess1) is " << result << std::endl;
    if ( 1 != result ) bLoggedIn = false;

    m_lbExecAddr.GetText( m_lbExecAddr.GetCurSel(), buffer1 );
    m_lbExecPort.GetText( m_lbExecPort.GetCurSel(), buffer2 );
    nPort = atoi( buffer2 );
    theApp.m_session2.m_setting.SetExecAddress(buffer1,nPort);

    m_lbLvl1Addr.GetText( m_lbLvl1Addr.GetCurSel(), buffer1 );
    m_lbLvl1Port.GetText( m_lbLvl1Port.GetCurSel(), buffer2 );
    nPort = atoi( buffer2 );
    theApp.m_session2.m_setting.SetQuoteAddress(buffer1,nPort);

    m_lbLvl2Addr.GetText( m_lbLvl2Addr.GetCurSel(), buffer1 );
    m_lbLvl2Port.GetText( m_lbLvl2Port.GetCurSel(), buffer2 );
    nPort = atoi( buffer2 );
    theApp.m_session2.m_setting.SetLevel2Address(buffer1,nPort);

    m_edtPassword.GetWindowTextA( buffer1, 20 );
    result = theApp.m_session2.Login( "OURB002", buffer1 );
    std::cout << "Login result (sess2) is " << result << std::endl;
    if ( 1 != result ) bLoggedIn = false;


  }
  catch (...) {
    std::cout << "Login Exception" << std::endl;
    bLoggedIn = false;
  }

  if ( bLoggedIn ) {
  }


  //m_session.m_accounts.
  //m_session.m_account.
  //m_session.m_setting.
  //m_session.m_stocks.
  //m_session.m_cancels.
  //m_session.m_orders.
  //theApp.m_session.PlayHistory("name");
}

void CGTScalpDlg::OnBnClickedLogout()
{
  //if ( 
  //if ( bLoggedIn ) {

    theApp.m_session1.Logout();
    theApp.m_session1.TryClose();

    theApp.m_session2.Logout();
    theApp.m_session2.TryClose();
  //}
}

void CGTScalpDlg::OnBnClickedStart() {
  char symbol[ 30 ];
  m_lbSymbolList.GetWindowTextA( symbol, 30 );
  m_vTradingLogic.push_back( new CTradingLogic( symbol ) );
  //pTradingLogic = new CTradingLogic( "ICE" );
  //pTradingLogic = new CTradingLogic( "ZXZZT" );
}

void CGTScalpDlg::OnBnClickedAccounts() {
  theApp.m_session1.EmitSessionInfo();
  theApp.m_session2.EmitSessionInfo();
}

void CGTScalpDlg::OnBnClickedIqfeed() {

  if ( NULL == m_pIQFeedSingleton ) {
    m_pIQFeedSingleton = new CIQFeedProviderSingleton();
    m_pIQFeed = m_pIQFeedSingleton->GetIQFeedProvider();

    m_btnIQFeedData.EnableWindow( TRUE );
    m_btnIQFWindows.EnableWindow( TRUE );
  }
}

void CGTScalpDlg::OnBnClickedBtnpendord() {
  pvpo = new CVuPendingOrders( theApp.m_pMainWnd );
}


void CGTScalpDlg::OnBnClickedBtnOptions() {
  //CIQFeedOptions *options = new CIQFeedOptions( "ICE" );
  options = new CVuOptions( theApp.m_pMainWnd ); 
  options->ShowWindow(1);
}

void CGTScalpDlg::OnNewsMessage( CIQFNewsMessage *pMsg ) {
  
  CString s;
  s.Format( "%s %s", pMsg->m_sDistributor.c_str(), pMsg->m_sHeadline.c_str() );
  pNews->WriteLine( s );
}

void CGTScalpDlg::OnBnClickedBtnhistory() {
  // TODO: Add your control notification handler code here
//  IQFeedHistoryHT *pht = new IQFeedHistoryHT();
//  pht->Send( "HT,ICE,3;" );
}

void CGTScalpDlg::OnBnClickedVuarms() {
  // TODO: Add your control notification handler code here
  pvuArms = new CVuChartArmsIntraDay( theApp.m_pMainWnd );
  pvuArms->ShowWindow( 1 );
  pvuArms->StartCharts( theApp.m_bLive, theApp.m_nDaysAgo );
}

void CGTScalpDlg::OnBnClickedLive() {
  // TODO: Add your control notification handler code here
  theApp.m_bLive = BST_CHECKED == m_btnLive.GetCheck();
}

void CGTScalpDlg::OnEnChangeDaysago() {
  // TODO:  If this is a RICHEDIT control, the control will not
  // send this notification unless you override the CDialog::OnInitDialog()
  // function and call CRichEditCtrl().SetEventMask()
  // with the ENM_CHANGE flag ORed into the mask.

  // TODO:  Add your control notification handler code here
  //CString s;
  char buf[ 10 ];
  m_edtDaysAgo.GetWindowTextA( buf, 10 );
  theApp.m_nDaysAgo = atoi( buf );
}

void CGTScalpDlg::OnBnClickedAllowtrades() {
  // TODO: Add your control notification handler code here
  theApp.m_bAllowTrades = BST_CHECKED == m_cbAllowTrades.GetCheck();
}

void CGTScalpDlg::OnCbnSelchangeSymbollist() {
  // TODO: Add your control notification handler code here
}
//ICE;@YM#;GOOG;ZXZZT
//IDC_SYMBOLLIST

void CGTScalpDlg::OnBnClickedIqfeedloadsymbols()
{
  // TODO: Add your control notification handler code here
  CIQFeedSymbolFile *p = new CIQFeedSymbolFile();
  p->Load( "mktsymbols.txt" );
  delete p;
}

void CGTScalpDlg::OnBnClickedLoaddailydata() {
  // TODO: Add your control notification handler code here
  char szDays[ 30 ];
  m_edtDaysAgo.GetWindowTextA( szDays, 30 );
  int nDays = atoi( szDays );
  if ( 0 < nDays ) {
    CScripts *scripts = new CScripts();
    scripts->GetIQFeedHistoryForSymbolRange( CScripts::Daily, nDays );
  }
}

void CGTScalpDlg::OnBnClickedDnlddaysymbol() {
  // TODO: Add your control notification handler code here
  char szSymbol[ 30 ];
  m_lbSymbolList.GetWindowTextA( szSymbol, 30 );
  char szDays[ 30 ];
  m_edtDaysAgo.GetWindowTextA( szDays, 30 );
  int nDays = atoi( szDays );
  if ( 0 != nDays && 0 != *szSymbol ) {
    CScripts *scripts = new CScripts();
    scripts->GetIQFeedHistoryForSymbol( szSymbol, CScripts::Daily, nDays );
  }
  else {
    cout << "Days was " << nDays << " for Symbol " << szSymbol << endl;
  }
}

void CGTScalpDlg::OnBnClickedIqfeedcmd() {
  char szCommand[ 40 ];
  m_lbIQCommands.GetWindowTextA( szCommand, 40 );
  if ( 0 != *szCommand ) {
    if ( NULL != m_pIQFeed ) {
      m_pIQFeed->Send( szCommand );
    }
  }
}

void CGTScalpDlg::OnBnClickedEnddayselect() {
  // TODO: Add your control notification handler code here
  //int i = m_dt;
  //bool b = m_rbDaySelect.
  m_grpEndDay.CheckRadioButton( IDC_ENDDAYCOUNT, IDC_ENDDAYSELECT, IDC_ENDDAYSELECT );
  m_dtStartDate.EnableWindow( 1 );
  m_dtStartTime.EnableWindow( 1 );
  m_eDayCalc = CSymbolSelectionFilter::DaySelect;
}

void CGTScalpDlg::OnBnClickedEndbarcount() {
  // TODO: Add your control notification handler code here
  m_grpEndDay.CheckRadioButton( IDC_ENDDAYCOUNT, IDC_ENDDAYSELECT, IDC_ENDBARCOUNT );
  m_dtStartDate.EnableWindow( 0 );
  m_dtStartTime.EnableWindow( 0 );
  m_eDayCalc = CSymbolSelectionFilter::BarCount;
}

void CGTScalpDlg::OnBnClickedEnddaycount() {
  // TODO: Add your control notification handler code here
  m_grpEndDay.CheckRadioButton( IDC_ENDDAYCOUNT, IDC_ENDDAYSELECT, IDC_ENDDAYCOUNT );
  m_dtStartDate.EnableWindow( 0 );
  m_dtStartTime.EnableWindow( 0 );
  m_eDayCalc = CSymbolSelectionFilter::DayCount;
}

void CGTScalpDlg::OnBnClickedBtnscan() {
  // TODO: Add your control notification handler code here
  int count = 0;
  ptime dtStart;
  ptime dtEnd;
  bool bValidValues = ( ( CSymbolSelectionFilter::NoDayCalc != m_eDayCalc ) && ( NoScanType != m_eScanType ) );
  if ( bValidValues ) {
    SYSTEMTIME dtLastDate, dtLastTime, dtStartDate, dtStartTime;
    m_dtLastDate.GetTime( &dtLastDate );
    m_dtLastTime.GetTime( &dtLastTime );
    m_dtStartDate.GetTime( &dtStartDate );
    m_dtStartTime.GetTime( &dtStartTime );
    dtStart = ptime( 
      boost::gregorian::date( dtStartDate.wYear, dtStartDate.wMonth, dtStartDate.wDay ),
      boost::posix_time::time_duration( dtStartTime.wHour, dtStartTime.wMinute, dtStartTime.wSecond, dtStartTime.wMilliseconds ) );
    dtEnd = ptime( 
      boost::gregorian::date( dtLastDate.wYear, dtLastDate.wMonth, dtLastDate.wDay ),
      boost::posix_time::time_duration( dtLastTime.wHour, dtLastTime.wMinute, dtLastTime.wSecond, dtLastTime.wMilliseconds ) );
    if ( ( CSymbolSelectionFilter::BarCount == m_eDayCalc  ) || ( CSymbolSelectionFilter::DayCount == m_eDayCalc ) ) {
      char szDays[ 30 ];
      m_edtDaysAgo.GetWindowTextA( szDays, 30 );
      count = atoi( szDays );
      if ( 0 == count ) bValidValues = false;
    }
  }
  if ( bValidValues ) {
    //CScripts *scripts = new CScripts();
    CSymbolSelectionFilter *pFilter;
    switch ( m_eScanType ) {
      case Darvas:
        pFilter = new CSelectSymbolWithDarvas( m_eDayCalc, count, m_bUseDayStart, dtStart, m_bUseDayEnd, dtEnd );
        break;
      case Bollinger:
        pFilter = new CSelectSymbolWithBollinger( m_eDayCalc, count, m_bUseDayStart, dtStart, m_bUseDayEnd, dtEnd );
        break;
      case Breakout:
        pFilter = new CSelectSymbolWithBreakout( m_eDayCalc, count, m_bUseDayStart, dtStart, m_bUseDayEnd, dtEnd );
        break;
      case Volatility:
        pFilter = new CSelectSymbolWithVolatility( m_eDayCalc, count, m_bUseDayStart, dtStart, m_bUseDayEnd, dtEnd );
        break;
      case TenPercent:
        pFilter = new CSelectSymbolWith10Percent( m_eDayCalc, count, m_bUseDayStart, dtStart, m_bUseDayEnd, dtEnd );
        break;
    }
    if ( pFilter->Validate() ) {
      if ( NULL == m_pBasketTrade ) {
        std::cout << "Basket is not started." << std::endl;
      }
      else {
        pFilter->SetOnAddSymbolHandler( fastdelegate::MakeDelegate( this, &CGTScalpDlg::HandleSymbolForBasketContainer ) );
      }
      pFilter->Start();
      pFilter->WrapUp();
    }
    else {
      MessageBox( "Invalid presets", "Error" );
    }
    
  }
  else {
    MessageBox( "Invalid values found", "Error" );
  }
  
}

void CGTScalpDlg::OnBnClickedRbdarvas() {
  // TODO: Add your control notification handler code here
  m_grpScanType.CheckRadioButton( IDC_RBDARVAS, IDC_RADIO3, IDC_RBDARVAS );
  m_eScanType = Darvas;
}

void CGTScalpDlg::OnBnClickedRbbollinger() {
  // TODO: Add your control notification handler code here
  m_grpScanType.CheckRadioButton( IDC_RBDARVAS, IDC_RADIO3, IDC_RBBOLLINGER );
  m_eScanType = Bollinger;
}

void CGTScalpDlg::OnBnClickedRbbreakout() {
  m_grpScanType.CheckRadioButton( IDC_RBDARVAS, IDC_RADIO3, IDC_RBBREAKOUT );
  m_eScanType = Breakout;
}

void CGTScalpDlg::OnBnClickedRadio1() { // 10% range, positive or negative
  m_grpScanType.CheckRadioButton( IDC_RBDARVAS, IDC_RADIO3, IDC_RADIO1 );
  m_eScanType = TenPercent;
}

void CGTScalpDlg::OnBnClickedRadio2() { // volatility
  m_grpScanType.CheckRadioButton( IDC_RBDARVAS, IDC_RADIO3, IDC_RADIO2 );
  m_eScanType = Volatility;
}

void CGTScalpDlg::OnBnClickedUsedaystart() {
  m_bUseDayStart = ( BST_CHECKED == m_cbUseDayStart.GetCheck() );
  m_dtStartDate.EnableWindow( m_bUseDayStart ? 1 : 0 );
  m_dtStartTime.EnableWindow( m_bUseDayStart ? 1 : 0 );

  bool bBothDays = m_bUseDayStart && m_bUseDayEnd;
  m_rbSelectByDay.EnableWindow( bBothDays ? 0 : 1 );
  m_rbSelectByBarCount.EnableWindow( bBothDays ? 0 : 1 );
  m_rbSelectByDayCount.EnableWindow( bBothDays ? 0 : 1 );
}

void CGTScalpDlg::OnBnClickedUsedayend() {
  // TODO: Add your control notification handler code here
  m_bUseDayEnd = ( BST_CHECKED == m_cbUseDayEnd.GetCheck() );
  m_dtLastDate.EnableWindow( m_bUseDayEnd ? 1 : 0 );
  m_dtLastTime.EnableWindow( m_bUseDayEnd ? 1 : 0 );

  bool bBothDays = m_bUseDayStart && m_bUseDayEnd;
  m_rbSelectByDay.EnableWindow( bBothDays ? 0 : 1 );
  m_rbSelectByBarCount.EnableWindow( bBothDays ? 0 : 1 );
  m_rbSelectByDayCount.EnableWindow( bBothDays ? 0 : 1 );
}

class CTestTrade {
public:
  CTestTrade( void ) {};
  ~CTestTrade( void ) {};
  void HandleTrade( const CTrade &trade ) { 
    //std::cout << trade.m_dblTrade << " " << trade.m_nTradeSize << endl; 
  };
protected:
private:
};

CTestTrade testTrade;

void CGTScalpDlg::OnBnClickedOpenib() {
  // TODO: Add your control notification handler code here
  if ( NULL == m_pIB ) {
    char szAcct[ 50 ];
    m_edtIBAcctCode.GetWindowTextA( szAcct, 50 );
    if ( 0 == *szAcct ) {
      cout << "No IB Account Code" << endl;
    }
//    else {
      string sAcct( szAcct );
      m_pIB = new CIBTWS( sAcct );
      m_pIB->Connect();
      m_btnIBExecution.EnableWindow( TRUE );
      m_btnIBData.EnableWindow( TRUE );
//    }
  }
}

void CGTScalpDlg::OnBnClickedIbwatch() {
  if ( NULL != m_pIB ) {
    char symbol[ 30 ];
    m_lbSymbolList.GetWindowTextA( symbol, 30 );
    m_pIB->AddTradeHandler( symbol, MakeDelegate( &testTrade, &CTestTrade::HandleTrade ) );
  }
}

void CGTScalpDlg::OnBnClickedIbunwatch() {
  if ( NULL != m_pIB ) {
    char symbol[ 30 ];
    m_lbSymbolList.GetWindowTextA( symbol, 30 );
    m_pIB->RemoveTradeHandler( symbol, MakeDelegate( &testTrade, &CTestTrade::HandleTrade ) );
  }
}

void CGTScalpDlg::OnBnClickedIbclose() {
  if ( NULL != m_pIB ) {
    m_pIB->Disconnect();
    delete m_pIB;
    m_pIB = NULL;
    if ( BST_CHECKED == m_btnIBExecution.GetCheck() ) {
      m_pExecutionProvider = NULL;
      m_btnIBExecution.SetCheck( BST_UNCHECKED );
    }
    m_btnIBExecution.EnableWindow( FALSE );
    if ( BST_CHECKED == m_btnIBData.GetCheck() ) {
      m_pDataProvider = NULL;
      m_btnIBData.SetCheck( BST_UNCHECKED );
    }
    m_btnIBData.EnableWindow( FALSE );
  }
}

void CGTScalpDlg::OnBnClickedDsiqfeed() {
  m_grpDataSource.CheckRadioButton( IDC_DSIQFEED, IDC_DS2, IDC_DSIQFEED );
  m_eDataSourceType = DSIQFeed;
  m_pDataProvider = m_pIQFeed;
}

void CGTScalpDlg::OnBnClickedDsib() {
  m_grpDataSource.CheckRadioButton( IDC_DSIQFEED, IDC_DS2, IDC_DSIB );
  m_eDataSourceType = DSIB;
  m_pDataProvider = m_pIB;
}

void CGTScalpDlg::OnBnClickedDsgt1() {
  m_grpDataSource.CheckRadioButton( IDC_DSIQFEED, IDC_DS2, IDC_DSGT1 );
  m_eDataSourceType = DSGenesis1;
}

void CGTScalpDlg::OnBnClickedDsgt2() {
  m_grpDataSource.CheckRadioButton( IDC_DSIQFEED, IDC_DS2, IDC_DSGT2 );
  m_eDataSourceType = DSGenesis2;
}

void CGTScalpDlg::OnBnClickedChartsymbol() {
  char szSymbol[ 30 ];
  m_lbSymbolList.GetWindowTextA( szSymbol, 30 );
  string sSymbol( szSymbol );
  if ( ( 0 != *szSymbol ) && ( NoDS != m_eDataSourceType ) ) {
    if ( DSIB == m_eDataSourceType ) {  // only accept IB for now
      if ( NULL != m_pIB ) { // make sure IB is turned on
        CVuChart *pChartIntraDay;  // need to add to vector so can delete at end of program run
        pChartIntraDay = new CVuChart( sSymbol );
        pChartIntraDay->m_chart.SetBarFactoryWidthSeconds( 6 );
        pChartIntraDay->m_chart.SetWindowWidthSeconds( 90 * 6 ); 
        pChartIntraDay->m_chart.setMajorTickInc( 12 * 6 );
        pChartIntraDay->m_chart.setMinorTickInc( 18 );
        pChartIntraDay->m_chart.SetUpdateChart( true );
        std::string s = "Intraday ";
        s.append( sSymbol );
        pChartIntraDay->m_chart.SetTitle( s );
        pChartIntraDay->ShowWindow( 1 );
        m_pIB->AddTradeHandler( 
          sSymbol, 
          MakeDelegate( &pChartIntraDay->m_chart, &CChartDatedDatum::AddTrade ) 
          );
      }
    }
  }
}

void CGTScalpDlg::OnEnChangeIbacct() {
  // TODO:  If this is a RICHEDIT control, the control will not
  // send this notification unless you override the CDialog::OnInitDialog()
  // function and call CRichEditCtrl().SetEventMask()
  // with the ENM_CHANGE flag ORed into the mask.

  // TODO:  Add your control notification handler code here
}

void CGTScalpDlg::OnBnClickedRtchart() {
  char szSymbol[ 30 ];
  m_lbSymbolList.GetWindowTextA( szSymbol, 30 );
  string sSymbol( szSymbol );
  if ( ( 0 != *szSymbol ) && ( NoDS != m_eDataSourceType ) ) {
    if ( DSIB == m_eDataSourceType ) {  // only accept IB for now
      if ( NULL != m_pIB ) { // make sure IB is turned on
        CChartRealTimeContainer *pChart;  // need to add to vector so can delete at end of program run
        pChart = new CChartRealTimeContainer( sSymbol, m_pIB );
        pChart->ShowWindow( SW_SHOWNORMAL );
      }
    }
  }
}

// Todo:  move the switch statements in to the applicable groups and have the information stored and checked 
//    there in order to facilitate multiple reuse.

void CGTScalpDlg::OnBnClickedBasket() {
  bool bOK = true;
  CProviderInterface *pData = NULL;
  CProviderInterface *pExec = NULL;
  if ( NoDS == m_eDataSourceType ) {
    std::cout << "No Data Source selected" << std::endl;
    bOK = false;
  }
  else {
    switch ( m_eDataSourceType ) {
      case DSIQFeed:
        if ( NULL == m_pIQFeed ) { 
          bOK = false;
          std::cout << "IQFeed not started" << std::endl;
        }
        else {
          pData = m_pIQFeed;
          std::cout << "Data Source = IQFeed" << std::endl;
        }
        break;
      case DSIB:
        if ( NULL == m_pIB ) {
          bOK = false;
          std::cout << "IB not started" << std::endl;
        }
        else { 
          pData = m_pIB;
          std::cout << "Data Source = IB" << std::endl;
        }
        break;
      case DSGenesis1:
        break;
      case DSGenesis2:
        break;
    }
  }
  if ( NoExec == m_eExecutionType ) {
    std::cout << "No Execution Destination selected" << std::endl;
    bOK = false;
  }
  else {
    switch ( m_eExecutionType ) {
      case ExecIB:
        if ( NULL == m_pIB ) {
          bOK = false;
          std::cout << "IB not started" << std::endl;
        }
        else {
          pExec = m_pIB;
          std::cout << "Execution Destination = IB" << std::endl;
        }
        break;
      case ExecGenesis1:
        break;
      case ExecGenesis2:
        break;
    }
  }
  if ( bOK ) {
    if ( NULL != m_pBasketTrade ) {
      std::cout << "Basket already started" << std::endl;
    }
    else {
      m_pBasketTrade = new CBasketTradeContainer( pData, pExec);
    }
  }
}

void CGTScalpDlg::HandleSymbolForBasketContainer(const std::string &sSymbolName, const std::string &sPath, const std::string &sStrategy ) {
  m_pBasketTrade->AddSymbol( sSymbolName, sPath, sStrategy );
}

void CGTScalpDlg::OnBnClickedBasketprepare() {
  CString sFunds;
  m_edtFunds.GetWindowTextA( sFunds );
  if ( 0 < sFunds.GetLength() ) {
    int nFunds = atoi( (LPCTSTR) sFunds );
    double dblFunds = (double) nFunds;

    if ( m_bUseDayEnd ) {
      ptime dtEnd;
      SYSTEMTIME dtLastDate, dtLastTime;
      m_dtLastDate.GetTime( &dtLastDate );
      m_dtLastTime.GetTime( &dtLastTime );
      dtEnd = ptime( 
        boost::gregorian::date( dtLastDate.wYear, dtLastDate.wMonth, dtLastDate.wDay )
        //, boost::posix_time::time_duration( dtLastTime.wHour, dtLastTime.wMinute, dtLastTime.wSecond, dtLastTime.wMilliseconds ) 
        );
      m_pBasketTrade->Prepare( dtEnd, dblFunds );
    }
    else {
      std::cout << "no date specified" << std::endl;
    }

  }
}

void CGTScalpDlg::OnBnClickedAddtestsymbols() {
  if ( false ) {
    HandleSymbolForBasketContainer( "DELL", "/bar/86400/D/E/DELL", "Test" );
    HandleSymbolForBasketContainer( "AAPL", "/bar/86400/A/A/AAPL", "Test" );
    HandleSymbolForBasketContainer( "INTC", "/bar/86400/I/N/INTC", "Test" );
    HandleSymbolForBasketContainer( "MSFT", "/bar/86400/M/S/MSFT", "Test" );
    HandleSymbolForBasketContainer( "YHOO", "/bar/86400/Y/H/YHOO", "Test" );
    HandleSymbolForBasketContainer( "GOOG", "/bar/86400/G/O/GOOG", "Test" );
    HandleSymbolForBasketContainer( "ICE", "/bar/86400/I/C/ICE", "Test" );
    HandleSymbolForBasketContainer( "IBM", "/bar/86400/I/B/IBM", "Test" );
  }
  else {
    HandleSymbolForBasketContainer( "@YM#", "/bar/86400/@/Y/@YM#", "Test" );
    HandleSymbolForBasketContainer( "EBN8", "/bar/86400/E/B/EBN8", "Test" );
  }
}

void CGTScalpDlg::OnBnClickedExecib() {
  m_grpExecution.CheckRadioButton( IDC_EXECIB, IDC_EXEC5, IDC_EXECIB );
  m_eExecutionType = ExecIB;
  m_pExecutionProvider = m_pIB;
}

void CGTScalpDlg::OnBnClickedExecgenesis1() {
  m_grpExecution.CheckRadioButton( IDC_EXECIB, IDC_EXEC5, IDC_EXECGENESIS1 );
  m_eExecutionType = ExecGenesis1;
}

void CGTScalpDlg::OnBnClickedExecgenesis2() {
  m_grpExecution.CheckRadioButton( IDC_EXECIB, IDC_EXEC5, IDC_EXECGENESIS2 );
  m_eExecutionType = ExecGenesis2;
}

void CGTScalpDlg::OnBnClickedExec4() {
}

void CGTScalpDlg::OnBnClickedExec5() {
}

void CGTScalpDlg::OnBnClickedCboutsiderth() {
  m_bOutsideRTH = BST_CHECKED == m_cbOutsideRTH.GetCheck();
}

// group Order Side
void CGTScalpDlg::OnBnClickedRbosbuy() { // radio button order side buy
  m_grpOrderSide.CheckRadioButton( IDC_RBOSBUY, IDC_RBOSSELL, IDC_RBOSBUY );
  m_eOrderSide = OrderSide::Buy;
}

void CGTScalpDlg::OnBnClickedRbossell() { // radio button order side sell 
  m_grpOrderSide.CheckRadioButton( IDC_RBOSBUY, IDC_RBOSSELL, IDC_RBOSSELL );
  m_eOrderSide = OrderSide::Sell;
}

// group Trade Type
void CGTScalpDlg::OnBnClickedRbttmkt() {
  m_grpTradeType.CheckRadioButton( IDC_RBTTMKT, IDC_RBTT02, IDC_RBTTMKT );
  m_eOrderType = OrderType::Market;
}

void CGTScalpDlg::OnBnClickedRbttlmt() {
  m_grpTradeType.CheckRadioButton( IDC_RBTTMKT, IDC_RBTT02, IDC_RBTTLMT );
  m_eOrderType = OrderType::Limit;
}

void CGTScalpDlg::OnBnClickedRbttstp() {
  m_grpTradeType.CheckRadioButton( IDC_RBTTMKT, IDC_RBTT02, IDC_RBTTSTP );
  m_eOrderType = OrderType::Stop;
}

void CGTScalpDlg::OnBnClickedRbttstplmit() {
  m_grpTradeType.CheckRadioButton( IDC_RBTTMKT, IDC_RBTT02, IDC_RBTTSTPLMIT );
  m_eOrderType = OrderType::StopLimit;
}

void CGTScalpDlg::OnBnClickedRbtt01() {
  m_grpTradeType.CheckRadioButton( IDC_RBTTMKT, IDC_RBTT02, IDC_RBTT01 );
  m_eOrderType = OrderType::Unknown;
}

void CGTScalpDlg::OnBnClickedRbtt02() {
  m_grpTradeType.CheckRadioButton( IDC_RBTTMKT, IDC_RBTT02, IDC_RBTT02 );
  m_eOrderType = OrderType::Unknown;
}

void CGTScalpDlg::OnBnClickedBtnorder() {
  bool bOkToOrder = true;
  char szSymbol[ 30 ];
  m_lbSymbolList.GetWindowTextA( szSymbol, 30 );
  string sOriginalSymbolName( szSymbol );
  string sSymbolName;
  if ( NULL == m_pExecutionProvider ) bOkToOrder = false;
  else {
    if ( 0 != sOriginalSymbolName.size() ) {
      m_pExecutionProvider->GetAlternateInstrumentName( sOriginalSymbolName, &sSymbolName );
    }
  }
  if ( 0 == sSymbolName.size() ) bOkToOrder = false;
  CInstrument *pInstrument = NULL;
  if ( 0 == sOriginalSymbolName.size() ) bOkToOrder = false;
  else {
    CInstrumentFile file;
    file.OpenIQFSymbols();
    try {
      pInstrument = file.CreateInstrumentFromIQFeed( sOriginalSymbolName, sSymbolName );
    }
    catch (...) {
      std::cout << "CGTScalpDlg::OnBnClickedBtnorder problems" << std::endl;
      bOkToOrder = false;
    }
    file.CloseIQFSymbols();
  }

  if ( OrderSide::Unknown == m_eOrderSide ) bOkToOrder = false;

  char szDays[ 30 ];
  m_edtDaysAgo.GetWindowTextA( szDays, 30 );
  unsigned long nSize = atoi( szDays );
  if ( 0 == nSize ) bOkToOrder = false;

  if ( OrderType::Unknown == m_eOrderType ) bOkToOrder = false;

  char szLimitPrice[ 30 ];
  char szStopPrice[ 30 ];
  m_ebLimitPrice.GetWindowTextA( szLimitPrice, 30 );
  m_ebStopPrice.GetWindowTextA( szStopPrice, 30 );

  double dblLimit, dblStop;
  COrder *pOrder = NULL;
  try {
    switch ( m_eOrderType ) {
    case OrderType::Market:
      if ( bOkToOrder ) pOrder = new COrder( pInstrument, m_eOrderType, m_eOrderSide, nSize );
      break;
    case OrderType::Limit:
      if ( 0 == *szLimitPrice ) bOkToOrder = false;
      else {
        dblLimit = atof( szLimitPrice );
        if ( 0 == dblLimit ) bOkToOrder = false;
        if ( bOkToOrder ) pOrder = new COrder( pInstrument, m_eOrderType, m_eOrderSide, nSize, dblLimit );
      }
      break;
    case OrderType::StopLimit:
      if ( 0 == *szLimitPrice ) bOkToOrder = false;
      else {
        dblLimit = atof( szLimitPrice );
        if ( 0 == dblLimit ) bOkToOrder = false;
      }
      if ( 0 == *szStopPrice ) bOkToOrder = false;
      else {
        dblStop = atof( szStopPrice );
        if ( 0 == dblStop ) bOkToOrder = false;
      }
      if ( bOkToOrder ) 
        pOrder = new COrder( pInstrument, m_eOrderType, m_eOrderSide, nSize, dblLimit, dblStop );
      break;
    case OrderType::Stop:
      if ( 0 == *szStopPrice ) bOkToOrder = false;
      else {
        dblStop = atof( szStopPrice );
        if ( 0 == dblStop ) bOkToOrder = false;
      }
      if ( bOkToOrder ) 
        pOrder = new COrder( pInstrument, m_eOrderType, m_eOrderSide, dblStop, nSize );
      break;
    }
  }
  catch (... ) {
    std::cout << "Had exception while creating order" << std::endl;
    bOkToOrder = false;
  }
  if ( bOkToOrder ) {
    assert( NULL != pOrder );
    pOrder->SetOutsideRTH( m_bOutsideRTH );
    m_pExecutionProvider->PlaceOrder( pOrder );
  }
  else {
    std::cout << "Order was not submitted" << std::endl;
  }
}

void CGTScalpDlg::OnBnClickedIqfwindows() {

  if ( NULL != m_pIQFeed ) {
    pvi = new CVuIndicies( ::AfxGetMainWnd() );
    pvi->ShowWindow(1);

    psoi = new CVuSymbolsOfInterest( ::AfxGetMainWnd() );
    psoi->ShowWindow(1);

    pNews = new CConsoleMessages( ::AfxGetMainWnd() );
    pNews ->ShowWindow( SW_SHOWNORMAL );
    m_pIQFeed->NewsMessage.Add( MakeDelegate( this, &CGTScalpDlg::OnNewsMessage ) );
  }
}

void CGTScalpDlg::OnBnClickedBtnsavesymbol() {
  if ( NULL == m_pExecutionProvider ) {
    std::cout << "No execution provider set" << std::endl;
  }
  else {
    char szSymbol[ 30 ];
    m_edtOriginalSymbolName.GetWindowTextA( szSymbol, 30 );
    string sOriginalInstrumentName( szSymbol );
    m_edtAlternateSymbolName.GetWindowTextA( szSymbol, 30 );
    string sAlternateSymbolName( szSymbol );
    if ( ( 0 == sOriginalInstrumentName.size() ) || ( 0 == sAlternateSymbolName.size() ) ) {
      std::cout << "One of two symbols is zero length" << std::endl;
    }
    else {
      m_pExecutionProvider->SetAlternateInstrumentName( sOriginalInstrumentName, sAlternateSymbolName );
      std::cout << sOriginalInstrumentName << " set to " << sAlternateSymbolName << std::endl;
    }
  }
}
