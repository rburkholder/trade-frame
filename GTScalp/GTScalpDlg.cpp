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
#include "Scripts.h"

#ifdef _DEBUG
#define new DEBUG_NEW
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


// CGTScalpDlg dialog




CGTScalpDlg::CGTScalpDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGTScalpDlg::IDD, pParent)
  , SomeTextVal(_T(";;"))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

  options = NULL;
  pvpo = NULL;
  pvi = NULL;
  psoi = NULL;
  pvuArms = NULL;
  pNews = NULL;
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
  ON_BN_CLICKED(IDC_BTNHFS, &CGTScalpDlg::OnBnClickedBtnhfs)
  ON_BN_CLICKED(IDC_IQFEEDLOADSYMBOLS, &CGTScalpDlg::OnBnClickedIqfeedloadsymbols)
  ON_BN_CLICKED(IDC_LOADDAILYDATA, &CGTScalpDlg::OnBnClickedLoaddailydata)
  ON_BN_CLICKED(IDC_TESTHDF5, &CGTScalpDlg::OnBnClickedTesthdf5)
  ON_BN_CLICKED(IDC_DNLDDAYSYMBOL, &CGTScalpDlg::OnBnClickedDnlddaysymbol)
  ON_BN_CLICKED(IDC_IQFEEDCMD, &CGTScalpDlg::OnBnClickedIqfeedcmd)
  ON_BN_CLICKED(IDC_ITERATE, &CGTScalpDlg::OnBnClickedIterate)
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

  theApp.m_bLive = true;
  m_btnLive.SetCheck( BST_CHECKED );
  m_cbAllowTrades.SetCheck( BST_UNCHECKED );

  //char buf[ 11 ];
  //m_edtDaysAgo.GetLine( 0, buf, 10 );
  m_edtDaysAgo.SetWindowTextA( "1" );

  // looks like addresses for exec, level1, and level2
  CString GTAddress[] = { "76.8.64.2","76.8.64.3","76.8.64.4","69.64.202.157","" };
  // preferred ports for level2
  int ExecPort[] = { 15805,16805,16705,16605,16505,16405,16305,16205,16105,16005,15905,15305,15405,15505,15605,15705,17205,0 };

  // preferred ports for level 1 and level 2
  int Level1Port0[] = {16811,26811,36811,46811,56811,17811,27811,37811,47811,57811,0};
  int Level2Port0[] = {16810,26810,36810,46810,56810,17810,27810,37810,47810,57810,0};

  // any combination of the following
  CString Level2Address1[] = { "69.64.202.155","69.64.202.156","69.64.202.157",""};
  // alternate ports
  int Level2Port1[] = { 16324,26324,0};

  CString *pstr = GTAddress;
  while ( (*pstr).Compare( "" ) ) {
    //while ( "" != LPCTSTR( pstr ) ) {
    m_lbExecAddr.AddString( LPCTSTR( *pstr ) );
    m_lbExecAddr.SetCurSel( 0 );
    m_lbLvl1Addr.AddString( LPCTSTR( *pstr ) );
    m_lbLvl1Addr.SetCurSel( 0 );
    m_lbLvl2Addr.AddString( LPCTSTR( *pstr ) );
    m_lbLvl2Addr.SetCurSel( 0 );
    pstr++;
  }

  CString s;
  int *pport = ExecPort;
  while ( 0 != *pport ) {
    s.Format( "%d", *pport );
    m_lbExecPort.AddString( LPCTSTR( s ) );
    m_lbExecPort.SetCurSel( 0 );
    pport++;
  }

  pport = Level1Port0;
  while ( 0 != *pport ) {
    s.Format( "%d", *pport );
    m_lbLvl1Port.AddString( LPCTSTR( s ) );
    m_lbLvl1Port.SetCurSel( 0 );
    pport++;
  }

  pport = Level2Port0;
  while ( 0 != *pport ) {
    s.Format( "%d", *pport );
    m_lbLvl2Port.AddString( LPCTSTR( s ) );
    m_lbLvl2Port.SetCurSel( 0 );
    pport++;
  }

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
  theApp.m_pRefresh ->HandleRefresh();
  return 1;
}

afx_msg void CGTScalpDlg::OnDestroy( ) {
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
  if ( NULL != theApp.m_pIQFeed ) {
    delete theApp.m_pIQFeed;
    theApp.m_pIQFeed = NULL;
  }
}

void CGTScalpDlg::OnBnClickedLogin()
{
  int result = 0;

  bLoggedIn = false;
  CString s;

  try {
    theApp.m_session1.m_setting.SetExecAddress("76.8.64.4",15805);
    theApp.m_session1.m_setting.SetQuoteAddress("69.64.202.157",16811);
    theApp.m_session1.m_setting.SetLevel2Address("69.64.202.157",16810);
    result = theApp.m_session1.Login( "OURB001", "pembmnc" );
    s.Format("Login result (ses1) is %d", result );
    theApp.pConsoleMessages->WriteLine(s);
    if ( 1 == result ) bLoggedIn = true;

    theApp.m_session2.m_setting.SetExecAddress("76.8.64.4",15805);
    theApp.m_session2.m_setting.SetQuoteAddress("69.64.202.157",16811);
    theApp.m_session2.m_setting.SetLevel2Address("69.64.202.157",16810);
    result = theApp.m_session2.Login( "OURB002", "pembmnc" );
    s.Format("Login result (ses2) is %d", result );
    theApp.pConsoleMessages->WriteLine(s);
    if ( 1 == result ) bLoggedIn = true;


  }
  catch (...) {
    theApp.pConsoleMessages->WriteLine("Login Exception");
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

  if ( NULL == theApp.m_pIQFeed ) {
    theApp.m_pIQFeed = new CIQFeed( this );

    pvi = new CVuIndicies( theApp.m_pMainWnd );
    pvi->ShowWindow(1);

    psoi = new CVuSymbolsOfInterest( theApp.m_pMainWnd );
    psoi->ShowWindow(1);

    pNews = new CConsoleMessages( theApp.m_pMainWnd );
    pNews ->ShowWindow( SW_SHOWNORMAL );
    theApp.m_pIQFeed->NewsMessage.Add( MakeDelegate( this, &CGTScalpDlg::OnNewsMessage ) );
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

void CGTScalpDlg::OnBnClickedBtnhfs() {
  // TODO: Add your control notification handler code here
  pHdf5 = new CHDF5;
  pHdf5->Test();
  delete pHdf5;
}

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

void CGTScalpDlg::OnBnClickedTesthdf5() {
  // TODO: Add your control notification handler code here
  CScripts *scripts = new CScripts();
  scripts->TestDataSet();
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
  // TODO: Add your control notification handler code here
  char szCommand[ 40 ];
  m_lbIQCommands.GetWindowTextA( szCommand, 40 );
  if ( 0 != *szCommand ) {
    if ( NULL != theApp.m_pIQFeed ) {
      theApp.m_pIQFeed->Send( szCommand );
    }
  }
}

void CGTScalpDlg::OnBnClickedIterate() {
  // TODO: Add your control notification handler code here
  CScripts *scripts = new CScripts();
  scripts->IterateGroups();
}
