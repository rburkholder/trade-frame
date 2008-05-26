#pragma once

#include "IQFeedThreadCrossing.h"
#include "IQFeedMessages.h"
#include "IQFeedSymbol.h"
#include "Delegate.h"

#include <queue>
#include <map>
#include <string>
using namespace std;

class CIQFeed :public CWnd {
  DECLARE_DYNAMIC(CIQFeed)
public:
  CIQFeed( CWnd *pParent = NULL );
  ~CIQFeed(void);

  void Connect( void );
  void Disconnect( void );

  Delegate<CIQFNewsMessage *> NewsMessage;
  Delegate<CIQFTimeMessage *> TimeMessage;

  CIQFeedThreadCrossing *CheckOutLookupPort( void );
  void CheckInLookupPort( CIQFeedThreadCrossing * );

  void Send( char *szCommand );

  void StartQuoteTradeWatch( CIQFeedSymbol *pSymbol );
  void StopQuoteTradeWatch( CIQFeedSymbol *pSymbol );

protected:

  virtual void HandleQMessage( CIQFUpdateMessage *pMsg ) {};
  virtual void HandlePMessage( CIQFSummaryMessage *pMsg ) {};
  virtual void HandleFMessage( CIQFFundamentalMessage *pMsg ) {};
  virtual void HandleNMessage( CIQFNewsMessage *pMsg ) {};
  virtual void HandleTMessage( CIQFTimeMessage *pMsg ) {};
  virtual void HandleSMessage( CIQFSystemMessage *pMsg ) {};

private:
  CIQFeedThreadCrossing IQConnect;  // port and buffers for main streaming connection
  void OnPreCrossThreadResponse( unsigned  short nStr, const char *str, LPVOID object );
  afx_msg LRESULT OnCrossThreadArrival( WPARAM, LPARAM );
  void OnNewResponsePort5009( const char *str );
  void PortNullProcessing( const char *str );

  queue<CIQFeedThreadCrossing *> m_qLookupPortAvailable;

  DECLARE_MESSAGE_MAP()
};

