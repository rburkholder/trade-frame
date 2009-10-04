/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
 *                                                                      *
 * This file is provided as is WITHOUT ANY WARRANTY                     *
 *  without even the implied warranty of                                *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                *
 *                                                                      *
 * This software may not be used nor distributed without proper license *
 * agreement.                                                           *
 *                                                                      *
 * See the file LICENSE.txt for redistribution information.             *
 ************************************************************************/

#pragma once

// Todo:  Need to check that all buffers are empty.

#include "FastDelegate.h"
using namespace fastdelegate;

#include <string>

#include <codeproject/thread.h>

#define DATA_BUFSIZE 5120

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

  std::string m_sLastCommand;

private:
  // from CreateThread
  //HANDLE m_hReceiveThread;
  //DWORD m_ReceiveThreadId;
//  CWinThread *m_pThread;
//  static UINT ReceiveThread( LPVOID lpParameter );
  volatile bool m_bThreadActive;  // set and reset only by RecieveThread

  OnNewResponseHandler OnNewResponse;
  
//  SOCKET m_socket;
  bool m_bSocketOpen;
  bool m_bWaitingForCharacters;  // debugging aid to confirm nothing active when put in waiting qwu
  
  DWORD m_Flags;
  DWORD m_cntBytesSent;

};
