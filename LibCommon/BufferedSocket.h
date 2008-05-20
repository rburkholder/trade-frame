#pragma once

// Todo:  Need to check that all buffers are empty.

#include "FastDelegate.h"
using namespace fastdelegate;

#define DATA_BUFSIZE 2048

class CBufferedSocket {
public:
  CBufferedSocket(void);
  ~CBufferedSocket(void);

  typedef FastDelegate3<unsigned short, const char *, LPVOID> OnNewResponseHandler; // size (including null), array
  void SetOnNewResponse( OnNewResponseHandler function ) {
    OnNewResponse = function;
  }

  void SetObject( LPVOID object ) { m_object = object; };
  LPVOID GetObject( void ) { return m_object; };
  void Open( const char *pAddress, unsigned short Port );
  void Send( const char *pCommand );
  void Close();

protected:
  LPVOID m_object;  // an object used by the caller

private:
  HANDLE m_hReceiveThread;
  DWORD m_ReceiveThreadId;

  OnNewResponseHandler OnNewResponse;
  static DWORD WINAPI ReceiveThread( LPVOID lpParameter );

  SOCKET m_socket;
  
  DWORD m_Flags;
  DWORD m_cntBytesSent;

  bool m_bSocketOpen;

};
