#pragma once

#include "..\LibCommon\BufferedSocket.h"
#include "..\LibCommon\ReusableCharBuffers.h"

#include <queue>
using namespace std;

#include "FastDelegate.h"
using namespace fastdelegate;

class CIQFeedThreadCrossing {
public:
  CIQFeedThreadCrossing(void);
  virtual ~CIQFeedThreadCrossing(void);

  void OpenSocket( const char *pAddress, unsigned short port ); 
  void SendToSocket( const char *pCommand );
  void CloseSocket();

  typedef FastDelegate1<const char*> OnNewResponseHandler;  // post thread-crossing function to client
  void SetOnPostThreadCrossingResponse( OnNewResponseHandler function ) {
    OnNewResponse = function;
  }
  void SetOnPreThreadCrossingResponse( CBufferedSocket::OnNewResponseHandler function ) { // pre thread-crossing function from CBufferedSocket
    m_socket.SetOnNewResponse( function );
  }

  void QueueResponse( unsigned short, const char * );
  void ProcessResponse( void );

  void Activate( void );
  void Deactivate( void );

protected:
  CBufferedSocket m_socket;
  CReusableCharBuffers m_EmptyBuffers;
  queue<CCharBuffer *> m_responses;
  bool m_bActive;
  unsigned long m_nActivations;
private:
  CRITICAL_SECTION csProtectQueues;
  OnNewResponseHandler OnNewResponse;
};

