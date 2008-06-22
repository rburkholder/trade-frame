#include "StdAfx.h"

#include <stdexcept>

#include "GTStockX.h"

#include "GndtProvider.h"

CGndtProvider::CGndtProvider(void)
: CProviderInterface(), GTSession(), m_bLoginValuesSet( false )
{
  m_sName = "GNDT";
  m_nID = EProviderGNDT;
}

CGndtProvider::~CGndtProvider(void) { 
}

void CGndtProvider::SetLoginValues( const std::string &sLoginName, const std::string &sPassword,
    const std::string &sExecAddr, unsigned short nExecPort,
    const std::string &sLvl1Addr, unsigned short nLvl1Port,
    const std::string &sLvl2Addr, unsigned short nLvl2Port ) {

  m_sLoginName = sLoginName;
  m_sPassword = sPassword;
  m_sExecAddr = sExecAddr;
  m_nExecPort = nExecPort;
  m_sLvl1Addr = sLvl1Addr;
  m_nLvl1Port = nLvl1Port;
  m_sLvl2Addr = sLvl2Addr;
  m_nLvl2Port = nLvl2Port;

  m_bLoginValuesSet = true;
}

void CGndtProvider::Connect() {
  if ( !m_bConnected ) {
    if ( !m_bLoginValuesSet ) throw std::invalid_argument( "GNDT Login Values Not Set" );
  
    m_bConnected = true;

    CProviderInterface::Connect();
    GTSession::m_setting.SetExecAddress( m_sExecAddr.c_str(), m_nExecPort );
    GTSession::m_setting.SetQuoteAddress( m_sLvl1Addr.c_str(), m_nLvl1Port );
    GTSession::m_setting.SetLevel2Address( m_sLvl2Addr.c_str(), m_nLvl2Port );
    int ret = GTSession::Login( m_sLoginName.c_str(), m_sPassword.c_str() );

    // need to add '   OnConnected( 0 );' to event that has login
 
  }
}

void CGndtProvider::Disconnect() {
  if ( m_bConnected ) {
    m_bConnected = false;
    GTSession::Logout();
    GTSession::TryClose();
    BOOL bCanClose = GTSession::CanClose();
    CProviderInterface::Disconnect();
    // need to add 'OnDisconnected( 0 );' to event that has disconnection
  }
}

GTStock* CGndtProvider::OnCreateStock(LPCSTR pszStock) {
  return dynamic_cast<GTStock*>( new CGndtSymbol( pszStock, this ) );
}

CSymbol *CGndtProvider::NewCSymbol( const std::string &sSymbolName ) {
  return dynamic_cast<CSymbol*>( GTSession::CreateStock( sSymbolName.c_str() ) );
}

void CGndtProvider::StartQuoteWatch(CSymbol *pSymbol) {
  dynamic_cast<CGndtSymbol*>( pSymbol )->StartQuoteWatch();
}

void CGndtProvider::StopQuoteWatch(CSymbol *pSymbol) {
  dynamic_cast<CGndtSymbol*>( pSymbol )->StopQuoteWatch();
}

void CGndtProvider::StartTradeWatch(CSymbol *pSymbol) {
  dynamic_cast<CGndtSymbol*>( pSymbol )->StartTradeWatch();
}

void CGndtProvider::StopTradeWatch(CSymbol *pSymbol) {
  dynamic_cast<CGndtSymbol*>( pSymbol )->StopTradeWatch();
}

void CGndtProvider::StartDepthWatch(CSymbol *pSymbol) {
  dynamic_cast<CGndtSymbol*>( pSymbol )->StartDepthWatch();
}

void CGndtProvider::StopDepthWatch(CSymbol *pSymbol) {
  dynamic_cast<CGndtSymbol*>( pSymbol )->StopDepthWatch();
}

BEGIN_MESSAGE_MAP(CGndtProvider, GTSession)
	//{{AFX_MSG_MAP(CGTSessionX)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CGndtProvider::OnExecConnected() {

  OnConnected( 0 );
  std::cout << "Exec Connected" << std::endl;

  return GTSession::OnExecConnected();
}

int CGndtProvider::OnExecDisconnected(){

  OnDisconnected( 0 );
  cout << "Exec Disconnected" << endl;

  return GTSession::OnExecDisconnected();
}

int CGndtProvider::OnExecMsgErrMsg(const GTErrMsg &errmsg) {
  CString s;
  s.Format("ErrMsg:  %d, %d, %s, %s, %s", 
    errmsg.dwOrderSeqNo, errmsg.nErrCode, errmsg.szStock, errmsg.szText,
    CGTSessionX::GetErrorMessage(errmsg.nErrCode));
  cout << s << endl;
  return GTSession::OnExecMsgErrMsg(errmsg);

}

int CGndtProvider::OnExecMsgChat(const GTChat &chat){
  CString s;
  s.Format("Chat:  %d, %s, %s, %s", 
    chat.nLevel, chat.szText, chat.szUserFm, chat.szUserTo);
  //theApp.pConsoleMessages->WriteLine(s);
  cout << s << endl;
  return GTSession::OnExecMsgChat(chat);
}

int CGndtProvider::OnExecMsgLogin(BOOL bLogin) {
  //theApp.pConsoleMessages->WriteLine("Log In");
  cout << "Log In" << endl;
  return GTSession::OnExecMsgLogin(bLogin);
}

int CGndtProvider::OnExecMsgLoggedin() {
  //theApp.pConsoleMessages->WriteLine("Logged In");
  CString s;
  s.Format( "Exec Logged in, %s ", m_account.szAccountID );
  //theApp.pConsoleMessages->WriteLine( s );
  cout << s << endl;

  return GTSession::OnExecMsgLoggedin();
}

void CGndtProvider::EmitSessionInfo() {

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

int CGndtProvider::OnExecMsgOpenPosition( const GTOpenPosition &open ) {
  CString s;
  s.Format( "Exec Open Position: Acct:%s Stk:%s Side:%c Position:%d@%0.2f", 
    open.szAccountID, open.szStock, open.chOpenSide, open.nOpenShares, open.dblOpenPrice  );
  cout << s << endl;
  return GTSession::OnExecMsgOpenPosition( open );
}

int CGndtProvider::OnExecMsgState( const GTServerState &state ) {
  CString s;
  s.Format( "MsgState:  %d %d %d %s", state.nConnect, state.nReportSvrID, state.nSvrID, state.szServer );
  cout << s << endl;
  return GTSession::OnExecMsgState( state );
}

int CGndtProvider::OnExecMsgAccount( const GTAccount &account ) {
  CString s;
  s.Format( "MsgAccount: %s %s %s %s %s", account.szAccountCode, account.szAccountID, account.szAccountName, account.szGroupID, account.szReconcileID );
  cout << s << endl;
  return GTSession::OnExecMsgAccount( account );
}

int CGndtProvider::OnExecMsgUser( const GTUser &user ) {
  CString s;
  s.Format( "MsgUser: %s, %s, %d, %d, %d", user.szUserID, user.szUserName, user.nLogExecID, user.bAdmin, user.bNoSOES );
  cout << s << endl;
  return GTSession::OnExecMsgUser( user );
  
}

int CGndtProvider::OnExecMsgPopup( const GTPopup &popup ) {
  CString s;
  s.Format( "MsgPopup: %d %s", popup.nLength, popup.pMsg );
  return GTSession::OnExecMsgPopup( popup );
}


int CGndtProvider::OnGotLevel2Connected() {
  cout << "Level 2 Connected" << endl;
  return GTSession::OnGotLevel2Connected();
}

int CGndtProvider::OnGotLevel2Disconnected() {
  cout << "Level 2 Disconnected" << endl;
  return GTSession::OnGotLevel2Disconnected();
}

int CGndtProvider::OnGotQuoteConnected() {
  cout << "Quote Connected" << endl;
  return GTSession::OnGotQuoteConnected();
}

int CGndtProvider::OnGotQuoteDisconnected() {
  cout << "Quote Disconnected" << endl;
  return GTSession::OnGotQuoteDisconnected();
}

int CGndtProvider::InitSession() {
  std::cout << "GNDT InitSession" << std::endl;
  return GTSession::InitSession();
}

int CGndtProvider::ExitSession() {
  std::cout << "GNDT ExitSession" << std::endl;
  return GTSession::ExitSession();
}

