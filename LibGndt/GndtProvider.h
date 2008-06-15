#pragma once

#include <iostream>
#include <string>

#include "K:\Data\Projects\Genesis\v8\GenSrc\API\INC\GTSession.h" 

#include "providerinterface.h"

#include "GndtSymbol.h"


class CGndtProvider :
  public CProviderInterface, public GTSession {
public:
  CGndtProvider(void);
  virtual ~CGndtProvider(void);

  void SetLoginValues( 
    const std::string &sLoginName, const std::string &sPassword,
    const std::string &sExecAddr, unsigned short nExecPort,
    const std::string &sLvl1Addr, unsigned short nLvl1Port,
    const std::string &sLvl2Addr, unsigned short nLvl2Port );
  virtual void Connect( void );
  virtual void Disconnect( void );


protected:
  DECLARE_MESSAGE_MAP()

  virtual GTStock *OnCreateStock( LPCSTR pszStock);
  virtual CSymbol *NewCSymbol( const std::string &sSymbolName );

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

  virtual int InitSession( void );
  virtual int ExitSession( void );

	virtual int OnGotLevel2Connected();
	virtual int OnGotLevel2Disconnected();

	virtual int OnGotQuoteConnected();
	virtual int OnGotQuoteDisconnected();

  void EmitSessionInfo();

  bool m_bLoginValuesSet;
  std::string m_sLoginName;
  std::string m_sPassword;
  std::string m_sExecAddr;
  unsigned short m_nExecPort;
  std::string m_sLvl1Addr;
  unsigned short m_nLvl1Port;
  std::string m_sLvl2Addr;
  unsigned short m_nLvl2Port;

  virtual void StartQuoteWatch( CSymbol *pSymbol );
  virtual void StopQuoteWatch( CSymbol *pSymbol );
  virtual void StartTradeWatch( CSymbol *pSymbol );
  virtual void StopTradeWatch( CSymbol *pSymbol );
  virtual void StartDepthWatch( CSymbol *pSymbol );
  virtual void StopDepthWatch( CSymbol *pSymbol );

private:
};
