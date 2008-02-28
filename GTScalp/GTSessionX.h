
#pragma once

#include "K:\Data\Projects\Genesis\v8\GenSrc\API\INC\GTSession.h"

class CGTSessionX : public GTSession {
public:
  CGTSessionX(void);
  ~CGTSessionX(void);

  virtual GTStock *OnCreateStock( LPCSTR pszStock);

	virtual int OnExecConnected();
	virtual int OnExecDisconnected();
	virtual int OnExecMsgErrMsg(const GTErrMsg &errmsg);
  virtual int OnExecMsgLogin(BOOL bLogin);
	virtual int OnExecMsgLoggedin();
	virtual int OnExecMsgChat(const GTChat &chat);
  virtual int OnExecMsgOpenPosition( const GTOpenPosition &open );
  virtual int OnExecMsgState( const GTServerState &state );
  virtual int OnExecMsgAccount( const GTAccount &account );
  virtual int OnExecMsgUser( const GTUser &user );
  virtual int OnExecMsgPopup( const GTPopup &popup );

	virtual int OnGotLevel2Connected();
	virtual int OnGotLevel2Disconnected();

	virtual int OnGotQuoteConnected();
	virtual int OnGotQuoteDisconnected();

  void EmitSessionInfo();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGTSessionX)
	//}}AFX_VIRTUAL

protected:

  //nOpenShares


	//{{AFX_MSG(CGTSessionX)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
