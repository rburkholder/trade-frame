// ConsoleMessages.cpp : implementation file
//

#include "stdafx.h"
//#include "GTScalp.h" // provides dialog box constants
#include "ConsoleMessages.h"

#include "assert.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CConsoleMessages dialog

IMPLEMENT_DYNAMIC(CConsoleMessages, CGUIFrameBase)

CConsoleMessages::CConsoleMessages(CWnd* pParent /*=NULL*/)
	: CGUIFrameBase( pParent ), bDialogReady( false )
{

  CGUIFrameBase::SetTitleBarText( _T( "Console Messages" ) );
  BOOL b = CGUIFrameBase::Create();
  assert( b );
  CGUIFrameBase::SetPosition( 300, 600, 1100, 1100 );

  CRect rect1, rect2;
  //GetWindowRect(&rect1);
  //rect1.MoveToXY(10,10);
  //MoveWindow(&rect1);

  CGUIFrameBase::GetClientRect( &rect1 );
  rect2.SetRect( 5, 5, rect1.right - 5, rect1.bottom - 5 );
  m_ceMessages.Create( WS_CHILD | WS_VISIBLE | ES_LEFT | ES_MULTILINE | ES_READONLY | WS_VSCROLL, rect2, this, 2 );
  m_ceMessages.SetLimitText( 10000000 );

  //ceMessages.MoveWindow( &rect2 );

  //m_ceMessages.EnableScrollBar( SB_VERT, ESB_ENABLE_BOTH );
  //m_ceMessages.EnableWindow( );

  /*
  VERIFY( m_font.CreateFont( 
   14,                        // nHeight
   0,                         // nWidth
   0,                         // nEscapement
   0,                         // nOrientation
   FW_NORMAL,                 // nWeight
   FALSE,                     // bItalic
   FALSE,                     // bUnderline
   0,                         // cStrikeOut
   ANSI_CHARSET,              // nCharSet
   OUT_DEFAULT_PRECIS,        // nOutPrecision
   CLIP_DEFAULT_PRECIS,       // nClipPrecision
   DEFAULT_QUALITY,           // nQuality
   DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
   "Arial"));                 // lpszFacename
   */
  VERIFY( m_font.CreatePointFont( 80, "Arial" ) );
  m_ceMessages.SetFont( &m_font );

  bDialogReady = true;
}

CConsoleMessages::~CConsoleMessages() {
  m_font.DeleteObject();
  //DestroyWindow();
  // need to destroy CDialog and CMessageList?
}

afx_msg void CConsoleMessages::OnSize( UINT i, int x, int y ) {
  if ( bDialogReady ) {
    CGUIFrameBase::OnSize( i, x, y );
    CRect rect1, rect2;
    GetClientRect( &rect1 );
    rect2.SetRect( 5, 5, rect1.right - 5, rect1.bottom - 5 );
    m_ceMessages.MoveWindow( &rect2 );
  }
}

/*
void CConsoleMessages::DoDataExchange(CDataExchange* pDX) {
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_MESSAGES, ceMessages);
  //DDX_Text(pDX, IDC_MESSAGES, sMessageList);
}
*/

// need to make this more elegant... need some sort of append capability

void CConsoleMessages::Write(const CString &sMessage) {
  int l = m_ceMessages.GetWindowTextLengthA();
  m_ceMessages.SetSel( l, l );
  m_ceMessages.ReplaceSel( (LPCTSTR) sMessage );
  //sMessageList.Insert(0, sMessage);
  //UpdateData(FALSE);
}

void CConsoleMessages::Write(const char *sMessage) {
  int l = m_ceMessages.GetWindowTextLengthA();
  m_ceMessages.SetSel( l, l );
  m_ceMessages.ReplaceSel( sMessage );
  //sMessageList.Insert(0, sMessage);
  //UpdateData(FALSE);
}

void CConsoleMessages::Write(const std::string &sMessage) {
  int l = m_ceMessages.GetWindowTextLengthA();
  m_ceMessages.SetSel( l, l );
  m_ceMessages.ReplaceSel( sMessage.c_str() );
  //sMessageList.Insert(0, sMessage.c_str() );
  //UpdateData(FALSE);
}

void CConsoleMessages::WriteLine(const CString &sMessage) {
  int l = m_ceMessages.GetWindowTextLengthA();
  m_ceMessages.SetSel( l, l );
  m_ceMessages.ReplaceSel( (LPCTSTR) sMessage );
  WriteLine();
  //sMessageList.Insert(0, _T( "\r\n" ) );
  //sMessageList.Insert(0, sMessage);
  //UpdateData(FALSE);
}

void CConsoleMessages::WriteLine( const char *sMessage ) {
  int l = m_ceMessages.GetWindowTextLengthA();
  m_ceMessages.SetSel( l, l );
  m_ceMessages.ReplaceSel( sMessage );
  WriteLine();
  //sMessageList.Insert(0, _T( "\r\n" ) );
  //sMessageList.Insert(0, sMessage);
  //UpdateData(FALSE);
}

void CConsoleMessages::WriteLine() {
  int l = m_ceMessages.GetWindowTextLengthA();
  m_ceMessages.SetSel( l, l );
  m_ceMessages.ReplaceSel( _T( "\r\n" ) );
  //sMessageList.Insert(0, _T( "\r\n" ) );
  //UpdateData(FALSE);
}

BEGIN_MESSAGE_MAP(CConsoleMessages, CGUIFrameBase)
  ON_WM_SIZE()
END_MESSAGE_MAP()

// CConsoleMessages message handlers
