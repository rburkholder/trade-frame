#pragma once
#include "afxwin.h"
//#include "Resource.h"

#include "GUIFrameBase.h"

#include <string>
using namespace std;

// CConsoleMessages dialog

class CConsoleMessages : public CGUIFrameBase {
	DECLARE_DYNAMIC(CConsoleMessages)
public:
	CConsoleMessages(CWnd* pParent = NULL);   // standard constructor
	virtual ~CConsoleMessages();

  void Write( const CString &sMessage );
  void Write( const char *sMessage );
  void Write( const string &s );
  void WriteLine( const CString &sMessage );
  void WriteLine( const char *sMessage );
  void WriteLine( void );

// Dialog Data
	//enum { IDD = IDD_CONSOLEMESSAGES };

protected:
	//virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

  CEdit m_ceMessages;
  CFont m_font;

private:

  bool bDialogReady;

  afx_msg void OnSize( UINT, int, int );


};
