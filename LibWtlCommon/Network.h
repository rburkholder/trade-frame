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
#include <boost/thread.hpp>  // separate thread for asio run processing
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>

#include <string>
#include <vector>
#include <cassert>

#include <typeinfo.h>
#include <sstream>

#include <LibCommon/ReusableBuffers.h>

// custom on
// http://msdn.microsoft.com/en-us/library/e5ewb1h3.aspx
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
// custom off

using boost::asio::ip::tcp;

// http://www.codeproject.com/KB/wtl/WTLIntellisense.aspx

// use asio post messages to send commands 
// use CThread PostMessages to return responses
// use circular buffer of 20 messages
// post message when circular buffer is empty
// based upon Lock-Free Queues? http://www.ddj.com/hpc-high-performance-computing/208801974
// input streams are cr/lf delimited
// LOG received buffer length to see if we are actually using big buffers on reception
// constructor uses static init structure, rather than dynamic update one message types
// maybe do dynamic message pointer updates later
// three threads:
//   * external thread for routines that send message to this class
//   * asio thread for processing outbound and inbound
//   * windows thread for processing windows messages
// when running with socket async io, is a timer needed to keep asio running?
// collect statistics to see if network side input buffers are being broken up
//   if they are, then remain with character by character delivery
//   if they are not, then can use begin/end iterators to pass information
//   or come up with a different scheme of managing iterators over multiple buffers for use by Spirit
// need code to catch when the socket is closed for whatever reason

template <class ownerT, class charT = unsigned char>
class CNetwork: public CGuiThreadImpl<CNetwork<ownerT> > {
public:

  // message ids for messages delivered to this class from external caller
  enum enumMessageTypes { 
    WM_NETWORK_CONNECT = WM_USER + 1,  // open the port
    WM_NETWORK_DISCONNECT,  // close the port
    WM_NETWORK_SEND,  // send string over network port
    WM_NETWORK_PROCESSED // message has been processed, return buffer to queue for re-use
  };

  enum enumMessageErrorTypes {
    OK = 0,
    ERROR_WRITE,
    ERROR_SOCKET,
    ERROR_CONNECT
  };

#define NETWORK_INPUT_BUF_SIZE 2048

  // pre-initialized message ids for messages delivered to and accepted by external caller
  struct structMessages {  // information needed by port processing thread
    ownerT* owner;        // owner object to which message is sent (PostMessage needs to be implemented)
    UINT msgInitialized;   // message id when class thread has been  initialized  (wparam=null, lparam=null)
    UINT msgClosed;   // message id when class thread has ended   (wparam=null, lparam=null)
    UINT msgConnected;    // message id when socket has been connected  (wparam=null, lparam=null)
    UINT msgDisconnected;  // message id when socket has been disconnected/closed  (wparam=null, lparam=null)
    UINT msgProcess;  // message id when sending line of text to process  (wparam=linebuffer_t*, lparam=null)
    UINT msgSendDone; // returns buffer used by the send message  (wparam=linebuffer_t*, lparam=null)
    UINT msgError;    // message id when error is encountered  (wparam=enumMessageErrorTypes, lparam=null)
    structMessages(void): owner( 0 ), msgInitialized( 0 ), msgClosed( 0 ), msgConnected( 0 ), msgDisconnected( 0 ),
      msgProcess( 0 ), msgSendDone( 0 ), msgError( 0 ) {};
    structMessages(ownerT* owner_, UINT msgInitialized_, UINT msgClosed_, UINT msgConnected_, UINT msgDisconnected_,
      UINT msgProcess_, UINT msgSendDone_, UINT msgError_ )
      : owner( owner_ ), msgInitialized( msgInitialized_ ), msgClosed( msgClosed_ ), msgConnected( msgConnected_ ),
        msgDisconnected( msgDisconnected_ ), msgProcess( msgProcess_ ), msgSendDone( msgSendDone_ ), msgError( msgError_ ) {};
  };

  struct structConnection {
    std::string sAddress;
    unsigned short nPort;
    structConnection( const std::string& sAddress_, unsigned short nPort_ )
      : sAddress( sAddress_ ), nPort( nPort_ ) {};
  };

  // factor a couple of these out as traits for here and for IQFeedMessages.
  typedef charT bufferelement_t;
  typedef boost::array<bufferelement_t, NETWORK_INPUT_BUF_SIZE> inputbuffer_t; // bulk input buffer via asio
  typedef CBufferRepository<inputbuffer_t> inputrepository_t;
  typedef std::vector<bufferelement_t> linebuffer_t;  // used for composing lines of data for processing
  typedef CBufferRepository<linebuffer_t> linerepository_t;

  CNetwork(CAppModule* pModule, const structMessages&);
  ~CNetwork(void);

protected:
  BEGIN_MSG_MAP_EX(CNetwork)
//    MSG_WM_TIMER(OnTimer)
    MESSAGE_HANDLER(WM_NETWORK_SEND, OnSend)  // command to be transmitted to network  (wparam=linebuffer_t*, lparam=null)
    MESSAGE_HANDLER(WM_NETWORK_PROCESSED, OnProcessed)  // line buffer is returned to repository  (wparam=linebuffer_t*, lparam=null)
    MESSAGE_HANDLER(WM_NETWORK_CONNECT, OnConnect)  //(wparam=const structConnection&, lparam=null)
    MESSAGE_HANDLER(WM_NETWORK_DISCONNECT, OnDisconnect)  //(wparam=null, lparam=null)
  END_MSG_MAP()

  LRESULT OnConnect( UINT, WPARAM, LPARAM, BOOL &bHandled);
  LRESULT OnSend( UINT, WPARAM, LPARAM, BOOL &bHandled);
  LRESULT OnProcessed( UINT, WPARAM, LPARAM, BOOL &bHandled);
  LRESULT OnDisconnect( UINT, WPARAM, LPARAM, BOOL &bHandled);

  BOOL InitializeThread( void );
  void CleanupThread( DWORD );

  BOOL PostMessageToOwner( UINT, WPARAM = NULL, LPARAM = NULL );

private:

  enum enumNetworkState {
    NS_INITIALIZING,
    NS_DISCONNECTED,
    NS_CONNECTING,
    NS_CONNECTED,
    NS_DISCONNECTING
  } m_stateNetwork;

  structMessages m_Messages;

  bool m_bKeepTimerActive;

  boost::thread m_asioThread;

  boost::asio::io_service m_io;
  boost::asio::ip::tcp::socket* m_psocket;

  boost::asio::deadline_timer m_timer;  // replace this busy work thing with: http://think-async.com/Asio/Recipes

  inputrepository_t m_reposInputBuffers;
  linerepository_t m_reposLineBuffers;

  linebuffer_t* m_pline;

  unsigned int m_cntAsyncReads;
  unsigned int m_cntBytesTransferred_input;
  unsigned int m_cntLinesProcessed;

  unsigned int m_cntSends;
  unsigned int m_cntBytesTransferred_send;

  void ConnectHandler( const boost::system::error_code& error );
  void TimerHandler( const boost::system::error_code& error );
  void WriteHandler( const boost::system::error_code& error, std::size_t bytes_transferred, linebuffer_t*, WPARAM wParam );
  void ReadHandler( const boost::system::error_code& error, std::size_t bytes_transferred, inputbuffer_t* );
  void AsyncRead( void );

  void AsioThread( void );

};

template <class ownerT, class charT>
CNetwork<ownerT,charT>::CNetwork(CAppModule* pModule, const structMessages& messages)
: CGuiThreadImpl<CNetwork>( pModule ), m_Messages( messages ),
  m_stateNetwork( NS_INITIALIZING ),
  m_timer( m_io, boost::posix_time::seconds( 1 ) ), m_bKeepTimerActive( true ),
  m_psocket( NULL ), //m_bSocketOpen( false ), //, m_bSocketOpened( false ),
  m_cntBytesTransferred_input( 0 ), m_cntAsyncReads( 0 ),
  m_cntSends( 0 ), m_cntBytesTransferred_send( 0 ),
  m_cntLinesProcessed( 0 )
{
  m_pline = m_reposLineBuffers.CheckOut();
  m_pline->clear();
}

template <class ownerT, class charT>
CNetwork<ownerT,charT>::~CNetwork(void) {

  PostQuitMessage();

  if ( 0 != m_pline->size() ) {
    OutputDebugString( "CNetwork::~CNetwork: m_line is non-zero in size.\n" );
  }
  m_reposLineBuffers.CheckIn( m_pline );
  m_pline = NULL;

//  if ( m_asioThread. ) {  // need to find check for done and cleared
//    OutputDebugString( "CNetwork::~CNetwork: m_asioThread is not NULL.\n" );
//  }

#if defined _DEBUG
  std::stringstream ss;
  ss << typeid( this ).name()
    << " bytes in " << m_cntBytesTransferred_input 
    << " on " << m_cntAsyncReads << " reads with " << m_cntLinesProcessed << " lines out, "
    << " bytes out " << m_cntBytesTransferred_send
    << " on " << m_cntSends << " sends." 
    << std::endl;
  OutputDebugString( ss.str().c_str() );
  ss.str() = "";
#endif
}

template <class ownerT, class charT>
BOOL CNetwork<ownerT,charT>::InitializeThread( void ) {

  BOOL b = PostMessageToOwner( m_Messages.msgInitialized );

  return TRUE;
}

template <class ownerT, class charT>
void CNetwork<ownerT,charT>::AsioThread( void ) {

  m_io.run();  // handles async timer and async socket 
#ifdef _DEBUG
  std::stringstream ss;
  ss << "ASIO Thread Exit:  " << typeid( this ).name() << std::endl;
  OutputDebugString( ss.str().c_str() );
  ss.str() = "";
#endif
}

template <class ownerT, class charT>
void CNetwork<ownerT,charT>::CleanupThread( DWORD dw ) {

  if ( NS_CONNECTED == m_stateNetwork ) {
    BOOL bHandled;
    OnDisconnect( NULL, NULL, NULL, bHandled );
  }

  m_asioThread.join();  // wait for i/o thread to terminate

//  delete m_asioThread;
//  m_asioThread = NULL;

  BOOL b = PostMessageToOwner( m_Messages.msgClosed );
}

template <class ownerT, class charT>
LRESULT CNetwork<ownerT,charT>::OnConnect( UINT, WPARAM wParam, LPARAM, BOOL &bHandled ) {

  const structConnection* connection  = reinterpret_cast<structConnection*>( wParam ); 

  // keep the asio busy with something
  // http://www.boost.org/doc/libs/1_40_0/doc/html/boost_asio/reference/deadline_timer.html
  m_timer.async_wait( boost::bind( &CNetwork::TimerHandler, this, boost::asio::placeholders::error ) );

  m_asioThread = boost::thread( boost::bind( &CNetwork::AsioThread, this ) );

  if ( NULL != m_psocket ) {
    OutputDebugString( "CNetwork::OnConnect:  m_psocket not null.\n" );
  }
  m_psocket = new tcp::socket( m_io );

  m_stateNetwork = NS_CONNECTING;

  // http://www.boost.org/doc/libs/1_40_0/doc/html/boost_asio/reference/basic_stream_socket/async_connect.html
  tcp::endpoint endpoint( boost::asio::ip::address::from_string( connection->sAddress ), connection->nPort );
  m_psocket->async_connect( 
    endpoint, 
    boost::bind<void>( &CNetwork::ConnectHandler, this, boost::asio::placeholders::error ) 
    );

  bHandled = true;
  return 1;
}

template <class ownerT, class charT>
LRESULT CNetwork<ownerT,charT>::OnDisconnect( UINT, WPARAM, LPARAM, BOOL &bHandled ) {

  //m_psocket->cancel();  //  boost::asio::error::operation_aborted, boost::asio::error::operation_not_supported on xp
  if ( NS_CONNECTED == m_stateNetwork ) {
    m_psocket->close();
  }

  delete m_psocket;
  m_psocket = NULL;

  m_bKeepTimerActive = false;
  m_timer.cancel();

  m_stateNetwork = NS_DISCONNECTED;

  BOOL b = PostMessageToOwner( m_Messages.msgDisconnected );

  bHandled = true;
  return 1;
}

template <class ownerT, class charT>
void CNetwork<ownerT,charT>::ConnectHandler( const boost::system::error_code& error ) {
  if ( error ) {
    BOOL b = PostMessageToOwner( m_Messages.msgError, ERROR_CONNECT );
  }
  else {
//    m_bSocketOpen = true;

    m_stateNetwork = NS_CONNECTED;

    AsyncRead();

    BOOL b = PostMessageToOwner( m_Messages.msgConnected );
  }
}

template <class ownerT, class charT>
void CNetwork<ownerT,charT>::AsyncRead( void ) {

  assert( NS_CONNECTED == m_stateNetwork );

  inputbuffer_t* pbuffer = m_reposInputBuffers.CheckOut();
//  if ( NETWORK_INPUT_BUF_SIZE > pbuffer->capacity() ) {
//    pbuffer->reserve( NETWORK_INPUT_BUF_SIZE );
//  }
  m_psocket->async_read_some( boost::asio::buffer( *pbuffer ), 
    boost::bind( 
      &CNetwork::ReadHandler, this,
      boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, pbuffer ) );

}

template <class ownerT, class charT>
void CNetwork<ownerT,charT>::ReadHandler( const boost::system::error_code& error, std::size_t bytes_transferred, inputbuffer_t* pbuffer ) {

  if ( 0 == bytes_transferred ) {
    // probably infers that connection has been closed
    OutputDebugString( "CNetwork::ReadHandler: connection probably closed.\n" );
  }
  else {
    assert( NS_CONNECTED == m_stateNetwork );

    ++m_cntAsyncReads;
    m_cntBytesTransferred_input += bytes_transferred;

    AsyncRead();  // set up for another read while processing existing buffer

    // process the buffer here

    // need current linebuffer, need stats on how often it waits for subsequent bulk data
    bufferelement_t ch;
    inputbuffer_t::const_iterator input = pbuffer->begin();
    while ( 0 != bytes_transferred ) {
      ch = *input;
      ++input;
      if ( 0 == ch ) {
        OutputDebugString( "CNetwork::ReadHandler: have a 0x00 character.\n" );
      }
      if ( 0x0a == ch ) {
        // send the buffer off 
        PostMessageToOwner( m_Messages.msgProcess, reinterpret_cast<WPARAM>( m_pline ) );
        ++m_cntLinesProcessed;
        // and allocate another buffer
        m_pline = m_reposLineBuffers.CheckOut();
        m_pline->clear();
      }
      else {
        if ( 0x0d == ch ) {
          // ignore the character
        }
        else {
          m_pline->push_back( ch );
        }
      }

      --bytes_transferred;
    } // end while

  }
  m_reposInputBuffers.CheckIn( pbuffer );
}

template <class ownerT, class charT>
void CNetwork<ownerT,charT>::TimerHandler( const boost::system::error_code& error ) {

  //assert( NS_CONNECTED == m_stateNetwork );

  if ( m_bKeepTimerActive ) {
    m_timer.expires_at( m_timer.expires_at() + boost::posix_time::seconds( 1 ) );
    m_timer.async_wait( boost::bind( &CNetwork::TimerHandler, this, boost::asio::placeholders::error ) );
  }
}

template <class ownerT, class charT>
LRESULT CNetwork<ownerT,charT>::OnSend( UINT, WPARAM wParam, LPARAM lParam, BOOL &bHandled ) {

  linebuffer_t* pbuffer = reinterpret_cast<linebuffer_t*>( lParam );

  if ( NS_CONNECTED == m_stateNetwork ) {
    boost::asio::async_write( *m_psocket, boost::asio::buffer( *pbuffer ), 
      boost::bind( &CNetwork::WriteHandler, this,
      boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, pbuffer, wParam )
      );
    ++m_cntSends;
  }
  else {
    BOOL b = PostMessageToOwner( m_Messages.msgError, ERROR_SOCKET );
  }

  bHandled = true;
  return 1;
}

template <class ownerT, class charT>
void CNetwork<ownerT,charT>::WriteHandler( 
        const boost::system::error_code& error, 
        std::size_t bytes_transferred, 
        linebuffer_t* pbuffer,
        WPARAM wParam) {

  assert( NS_CONNECTED == m_stateNetwork );

  if ( error ) {
    BOOL b = PostMessageToOwner( m_Messages.msgError, ERROR_WRITE ); 
  }
  assert( bytes_transferred == pbuffer->size() );
  m_cntBytesTransferred_send += bytes_transferred;
  PostMessageToOwner( m_Messages.msgSendDone, wParam, reinterpret_cast<LPARAM>( pbuffer ) );
}

template <class ownerT, class charT>
LRESULT CNetwork<ownerT,charT>::OnProcessed( UINT, WPARAM wparam, LPARAM, BOOL &bHandled ) {

  linebuffer_t* pbuffer = reinterpret_cast<linebuffer_t*>( wparam );
  m_reposLineBuffers.CheckIn( pbuffer );

  bHandled = true;
  return 1;
}

template <class ownerT, class charT>
BOOL CNetwork<ownerT,charT>::PostMessageToOwner( UINT msg, WPARAM wParam, LPARAM lParam ) {
  //BOOL b = false;
  if ( 0 != msg ) {
    if ( NULL != m_Messages.owner ) {
      m_Messages.owner->PostMessage( msg, wParam, lParam );
    }
  }
  return TRUE;
}
