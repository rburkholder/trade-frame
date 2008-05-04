#ifndef mysocket_def
#define mysocket_def

#include <afxsock.h>

class EClientSocket;

class MySocket : public CAsyncSocket
{
public:
   MySocket( EClientSocket *pClient);
   void OnConnect( int i);
   void OnReceive( int i);
   void OnSend( int i);
   void OnClose( int i);
private:
   EClientSocket *m_pClient;
};

#endif
