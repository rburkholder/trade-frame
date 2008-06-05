#include "StdAfx.h"
//#include "GTScalp.h"
#include "GTSessionX.h"
#include "GTStockX.h"

#include <iostream>
using namespace std;

CGTSessionX::CGTSessionX(void): GTSession() {  
}

CGTSessionX::~CGTSessionX(void){
}

BEGIN_MESSAGE_MAP(CGTSessionX, GTSession)
	//{{AFX_MSG_MAP(CGTSessionX)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CGTSessionX::OnExecConnected()
{
	//m_pDlg->m_login.EnableWindow(FALSE);
	//m_pDlg->m_start.EnableWindow(TRUE);

	//m_pDlg->m_list.InsertString(0, "Exec Connected");

  cout << "Exec Connected" << endl;

	return GTSession::OnExecConnected();
}

int CGTSessionX::OnExecDisconnected(){
  cout << "Exec Disconnected" << endl;
  return GTSession::OnExecDisconnected();
}

int CGTSessionX::OnExecMsgErrMsg(const GTErrMsg &errmsg) {
  CString s;
  s.Format("ErrMsg:  %d, %d, %s, %s, %s", 
    errmsg.dwOrderSeqNo, errmsg.nErrCode, errmsg.szStock, errmsg.szText,
    CGTSessionX::GetErrorMessage(errmsg.nErrCode));
  cout << s << endl;
  return GTSession::OnExecMsgErrMsg(errmsg);

}

int CGTSessionX::OnExecMsgChat(const GTChat &chat){
  CString s;
  s.Format("Chat:  %d, %s, %s, %s", 
    chat.nLevel, chat.szText, chat.szUserFm, chat.szUserTo);
  //theApp.pConsoleMessages->WriteLine(s);
  cout << s << endl;
  return GTSession::OnExecMsgChat(chat);
}

int CGTSessionX::OnExecMsgLogin(BOOL bLogin) {
  //theApp.pConsoleMessages->WriteLine("Log In");
  cout << "Log In" << endl;
  return GTSession::OnExecMsgLogin(bLogin);
}

int CGTSessionX::OnExecMsgLoggedin() {
  //theApp.pConsoleMessages->WriteLine("Logged In");
  CString s;
  s.Format( "Exec Logged in, %s ", m_account.szAccountID );
  //theApp.pConsoleMessages->WriteLine( s );
  cout << s << endl;

  return GTSession::OnExecMsgLoggedin();
}

void CGTSessionX::EmitSessionInfo() {

  CString s;

  s.Format( "AccountID: %s", m_account.szAccountID); 
  cout << s << endl;
  s.Format( "Equity:    %0.2f", m_account.dblCurrentEquity); 
  cout << s << endl;
  s.Format( "Current:   %0.2f", m_account.dblCurrentAmount ); 
  cout << s << endl;
  s.Format( "Long:      %0.2f", m_account.dblCurrentLong); 
  cout << s << endl;
  s.Format( "Short:     %0.2f", m_account.dblCurrentShort ); 
  cout << s << endl;
  s.Format( "PL:        %0.2f", m_account.dblPLRealized ); 
  cout << s << endl;

  GTOpenPosition open;
  void *pos;

  pos=GetFirstOpenPosition( m_account.szAccountID, open );
	while(pos != NULL){
    s.Format( "Logged In Open Position: Acct:%s Stk:%s Side:%c Position:%d@%0.2f", 
      open.szAccountID, open.szStock, open.chOpenSide, open.nOpenShares, open.dblOpenPrice  );
    cout << s << endl;
		pos = GetNextOpenPosition(m_account.szAccountID, pos, open);
	}
}

int CGTSessionX::OnExecMsgOpenPosition( const GTOpenPosition &open ) {
  CString s;
  s.Format( "Exec Open Position: Acct:%s Stk:%s Side:%c Position:%d@%0.2f", 
    open.szAccountID, open.szStock, open.chOpenSide, open.nOpenShares, open.dblOpenPrice  );
  cout << s << endl;
  return GTSession::OnExecMsgOpenPosition( open );
}

int CGTSessionX::OnExecMsgState( const GTServerState &state ) {
  CString s;
  s.Format( "MsgState:  %d %d %d %s", state.nConnect, state.nReportSvrID, state.nSvrID, state.szServer );
  cout << s << endl;
  return GTSession::OnExecMsgState( state );
}

int CGTSessionX::OnExecMsgAccount( const GTAccount &account ) {
  CString s;
  s.Format( "MsgAccount: %s %s %s %s %s", account.szAccountCode, account.szAccountID, account.szAccountName, account.szGroupID, account.szReconcileID );
  cout << s << endl;
  return GTSession::OnExecMsgAccount( account );
}

int CGTSessionX::OnExecMsgUser( const GTUser &user ) {
  CString s;
  s.Format( "MsgUser: %s, %s, %d, %d, %d", user.szUserID, user.szUserName, user.nLogExecID, user.bAdmin, user.bNoSOES );
  cout << s << endl;
  return GTSession::OnExecMsgUser( user );
  
}

int CGTSessionX::OnExecMsgPopup( const GTPopup &popup ) {
  CString s;
  s.Format( "MsgPopup: %d %s", popup.nLength, popup.pMsg );
  return GTSession::OnExecMsgPopup( popup );
}


int CGTSessionX::OnGotLevel2Connected() {
  cout << "Level 2 Connected" << endl;
  return GTSession::OnGotLevel2Connected();
}

int CGTSessionX::OnGotLevel2Disconnected() {
  cout << "Level 2 Disconnected" << endl;
  return GTSession::OnGotLevel2Disconnected();
}

int CGTSessionX::OnGotQuoteConnected() {
  cout << "Quote Connected" << endl;
  return GTSession::OnGotQuoteConnected();
}

int CGTSessionX::OnGotQuoteDisconnected() {
  cout << "Quote Disconnected" << endl;
  return GTSession::OnGotQuoteDisconnected();
}

GTStock* CGTSessionX::OnCreateStock(LPCSTR pszStock) {
  CGTStockX *pStock = new CGTStockX( *this, pszStock );
  if (NULL == pStock) {
    return NULL;
  }
  return pStock;
}


