// GTScalp.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "GTScalp.h"
#include "GTScalpDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CGTScalpApp

BEGIN_MESSAGE_MAP(CGTScalpApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CGTScalpApp construction

CGTScalpApp::CGTScalpApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CGTScalpApp object

CGTScalpApp theApp;


// CGTScalpApp initialization

BOOL CGTScalpApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("OneUnified\\TradeFrame"));

  //_CrtSetBreakAlloc(2968);
  //_CrtSetBreakAlloc(988);

  m_pIQFeed = NULL;

  m_bLive = true;

  CGTSessionX::Initialize(GTAPI_VERSION);

  CGTScalpDlg dlg;
  m_pMainWnd = &dlg;
  //dlg.MoveWindow(

  pConsoleMessages = new CConsoleCoutMessages(m_pMainWnd);
  if ( NULL != pConsoleMessages ) {
    pConsoleMessages->ShowWindow( SW_SHOWNORMAL );
  }

  m_pRefresh = new CGeneratePeriodicRefresh( m_pMainWnd );

	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

  //CGTSessionX::Uninitialize();

  delete m_pRefresh;

  delete pConsoleMessages;
  pConsoleMessages = NULL;

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

int CGTScalpApp::ExitInstance() {

  CGTSessionX::Uninitialize();
	
	return CWinApp::ExitInstance();
}


