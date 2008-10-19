// GTScalp.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "GTScalp.h"
#include "GTScalpDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
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

	BOOL b = CWinApp::InitInstance();
  ASSERT( b );

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

  //m_pIQFeed = NULL;

  m_bLive = true;

  //CGTScalpDlg *p4 = new CGTScalpDlg();

  CGTScalpDlg dlg;
  m_pMainWnd = &dlg;
  //m_pMainWnd = p4;
  //dlg.MoveWindow(

  //CGTSessionX::Initialize(GTAPI_VERSION);

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

  //delete m_pRefresh;

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

int CGTScalpApp::ExitInstance() {

  //CGTSessionX::Uninitialize();
	
	return CWinApp::ExitInstance();
}

BOOL CGTScalpApp::OnIdle(LONG lCount) {
  if ( 1 == CWinApp::OnIdle( lCount ) ) { // reserve first loop through for system
    return TRUE;
  }
  // anything else is fair game
  return FALSE;  // and then indicate don't do anything until another message has been processed
  // return TRUE; // keep accepting OnIdle messages
}


