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

#include <codeproject/thread.h>  // class inbound messages
#include <boost/asio.hpp>  // class outbound processing
#include <boost/array.hpp>
#include <string>


// use asio post messages to send commands 
// use CThread PostMessages to return responses
// use circular buffer of 20 messages
// post message when circular buffer is empty
// based upon Lock-Free Queues, http://www.ddj.com/hpc-high-performance-computing/208801974
// input streams are cr/lf delimited
// LOG received buffer length to see if we are actually using big buffers on reception
// constructor uses static init structure, rather than dynamic update one message types
// maybe do dynamic message pointer updates later

class CNetwork: public CGuiThreadImpl<CNetwork> {
public:

  enum enumMessageTypes {  // messages accepted by thread
    WM_NETWORK_OPEN = WM_USER + 1,  // open the port
    WM_NETWORK_SEND,  // send string over network port
    WM_NETWORK_CLOSE,  // close the port
    WM_NETWORK_PROCESSED // message has been processed, update buffer queue with processed buffer
  };

  struct structMessages {  // information needed by port processing thread
    HWND hWnd;        // handle of window to which message is sent
    UINT msgProcess;  // message id when sending line to process
    UINT msgInitialized;   // message id when socket system has been opened and initialized
    UINT msgClosed;   // message id when socket system has been closed
    UINT msgError;    // message id when error is encountered
    structMessages(void): hWnd( 0 ), msgProcess( 0 ), msgInitialized( 0 ), msgClosed( 0 ), msgError( 0 ) {};
    structMessages(HWND hWnd_, UINT msgProcess_, UINT msgInitialized_, UINT msgClosed_, UINT msgError_ )
      : hWnd( hWnd_ ), msgProcess( msgProcess_ ), msgInitialized( msgInitialized_ ), msgClosed( msgClosed_ ), msgError( msgError_ ) {};
  };

  struct structConnection {
    std::string sAddress;
    unsigned short nPort;
  };

  struct

  CNetwork(CAppModule* pModule, const structMessages&);
  ~CNetwork(void);

protected:
  BEGIN_MSG_MAP_EX(CNetwork)
//    MSG_WM_TIMER(OnTimer)
    MESSAGE_HANDLER(WM_NETWORK_OPEN, OnOpen)
    MESSAGE_HANDLER(WM_NETWORK_SEND, OnSend)
    MESSAGE_HANDLER(WM_NETWORK_PROCESSED, OnProcessed)
    MESSAGE_HANDLER(WM_NETWORK_CLOSE, OnClose)
  END_MSG_MAP()

  BOOL InitializeThread( void );
  void CleanupThread( DWORD );

  LRESULT OnOpen( UINT, WPARAM, LPARAM, BOOL &bHandled);
  LRESULT OnSend( UINT, WPARAM, LPARAM, BOOL &bHandled);
  LRESULT OnProcessed( UINT, WPARAM, LPARAM, BOOL &bHandled);
  LRESULT OnClose( UINT, WPARAM, LPARAM, BOOL &bHandled);

  boost::asio::io_service m_io;
  boost::asio::ip::tcp::socket* m_psocket;


private:
#define NETWORK_INBOUND_BUF_SIZE 5120

  structMessages m_Messages;

  boost::array<char, NETWORK_INBOUND_BUF_SIZE> m_buf;

};
