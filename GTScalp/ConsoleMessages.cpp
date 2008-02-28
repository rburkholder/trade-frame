// ConsoleMessages.cpp : implementation file
//

#include "stdafx.h"
//#include "GTScalp.h" // provides dialog box constants
#include "ConsoleMessages.h"

// CConsoleMessages dialog

IMPLEMENT_DYNAMIC(CConsoleMessages, CDialog)

CConsoleMessages::CConsoleMessages(CWnd* pParent /*=NULL*/)
	: CDialog(CConsoleMessages::IDD, pParent)
  , sMessageList(_T(""))
{

  bDialogReady = false;

  Create(IDD_CONSOLEMESSAGES);
  CRect rect1, rect2;
  GetWindowRect(&rect1);
  rect1.MoveToXY(10,10);
  MoveWindow(&rect1);

  GetClientRect( &rect1 );
  rect2.SetRect( 5, 5, rect1.right - 5, rect1.bottom - 5 );
  ceMessages.MoveWindow( &rect2 );

  bDialogReady = true;
}

afx_msg void CConsoleMessages::OnSize( UINT i, int x, int y ) {
  if ( bDialogReady ) {
    CRect rect1, rect2;
    GetClientRect( &rect1 );
    rect2.SetRect( 5, 5, rect1.right - 5, rect1.bottom - 5 );
    ceMessages.MoveWindow( &rect2 );
  }
}

CConsoleMessages::~CConsoleMessages() {
  DestroyWindow();
  // need to destroy CDialog and CMessageList?
}

void CConsoleMessages::DoDataExchange(CDataExchange* pDX) {
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_MESSAGES, ceMessages);
  DDX_Text(pDX, IDC_MESSAGES, sMessageList);
}

// need to make this more elegant... need some sort of append capability

void CConsoleMessages::Write(const CString &sMessage) {
  sMessageList.Insert(0, sMessage);
  UpdateData(FALSE);
}

void CConsoleMessages::Write(const char *sMessage) {
  sMessageList.Insert(0, sMessage);
  UpdateData(FALSE);
}

void CConsoleMessages::Write(const string &sMessage) {
  sMessageList.Insert(0, sMessage.c_str() );
  UpdateData(FALSE);
}

void CConsoleMessages::WriteLine(const CString &sMessage) {
  sMessageList.Insert(0, _T( "\r\n" ) );
  sMessageList.Insert(0, sMessage);
  UpdateData(FALSE);
}

void CConsoleMessages::WriteLine( const char *sMessage ) {
  sMessageList.Insert(0, _T( "\r\n" ) );
  sMessageList.Insert(0, sMessage);
  UpdateData(FALSE);
}

void CConsoleMessages::WriteLine() {
  sMessageList.Insert(0, _T( "\r\n" ) );
  UpdateData(FALSE);
}

BEGIN_MESSAGE_MAP(CConsoleMessages, CDialog)
  ON_WM_SIZE()

END_MESSAGE_MAP()


// CConsoleMessages message handlers
