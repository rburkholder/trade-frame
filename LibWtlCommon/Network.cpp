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

#include "StdAfx.h"
#include "Network.h"

#include <boost/bind.hpp>

using boost::asio::ip::tcp;

CNetwork::CNetwork(CAppModule* pModule, const structMessages& messages)
: CGuiThreadImpl<CNetwork>( pModule ), m_Messages( messages ),
  m_timer( m_io, boost::posix_time::seconds( 1 ) ), m_bKeepTimerActive( true ),
  m_psocket( new tcp::socket( m_io ) ), 
  m_bSocketOpen( false ), //, m_bSocketOpened( false ),
  m_cntBytesTransferred_input( 0 ), m_cntAsyncReads( 0 ),
  m_cntSends( 0 ), m_cntBytesTransferred_send( 0 )
{
  m_line = m_reposLineBuffers.CheckOut();
  m_line->clear();
}

CNetwork::~CNetwork(void) {
  if ( 0 != m_line->size() ) {
    OutputDebugString( "CNetwork::~CNetwork: m_line is non-zero in size.\n" );
  }
  delete m_line;

  // check that we've closed and deleted the thread
  // check that we've closed and deleted the socket
}

BOOL CNetwork::InitializeThread( void ) {

  m_asioThread = boost::thread( &CNetwork::AsioThread, this );

  BOOL b = PostMessage( m_Messages.msgInitialized );

  return TRUE;
}

void CNetwork::AsioThread( void ) {

  m_io.run();  // handles async timer and async socket 
#ifdef _DEBUG
  OutputDebugString( "ASIO thread exited.\n" );
#endif
}

void CNetwork::CleanupThread( DWORD dw ) {

  // http://www.boost.org/doc/libs/1_40_0/doc/html/boost_asio/reference/deadline_timer.html

  m_bKeepTimerActive = false;
  m_timer.cancel();

  if ( m_bSocketOpen ) {
    m_psocket->close();
    m_bSocketOpen = false;
  }

  m_asioThread.join();  // wait for i/o thread to terminate

  BOOL b = PostMessage( m_Messages.msgClosed );
}

LRESULT CNetwork::OnConnect( UINT, WPARAM wParam, LPARAM, BOOL &bHandled ) {

  const structConnection& connection  = reinterpret_cast<const structConnection&>( wParam ); 

  // http://www.boost.org/doc/libs/1_40_0/doc/html/boost_asio/reference/basic_stream_socket/async_connect.html
  tcp::endpoint endpoint( boost::asio::ip::address::from_string( connection.sAddress ), connection.nPort );
  m_psocket->async_connect( 
    endpoint, 
    boost::bind<void>( &CNetwork::ConnectHandler, this, boost::asio::placeholders::error ) 
    );

  bHandled = true;
  return 1;
}

void CNetwork::ConnectHandler( const boost::system::error_code& error ) {
  if ( error ) {
    BOOL b = PostMessage( m_Messages.msgError, ERROR_CONNECT );
  }
  else {
    m_bSocketOpen = true;

    AsyncRead();

    BOOL b = PostMessage( m_Messages.msgConnected );
  }
}

void CNetwork::AsyncRead( void ) {

  inputbuffer_t* pbuffer = m_reposInputBuffers.CheckOut();
//  if ( NETWORK_INPUT_BUF_SIZE > pbuffer->capacity() ) {
//    pbuffer->reserve( NETWORK_INPUT_BUF_SIZE );
//  }
  m_psocket->async_read_some( boost::asio::buffer( *pbuffer ), 
    boost::bind( 
      &CNetwork::ReadHandler, this,
      boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, pbuffer ) );

}

void CNetwork::ReadHandler( const boost::system::error_code& error, std::size_t bytes_transferred, inputbuffer_t* pbuffer ) {

  ++m_cntAsyncReads;
  if ( 0 == bytes_transferred ) {
    // probably infers that connection has been closed
    OutputDebugString( "CNetwork::ReadHandler:  connection probably closed.\n" );
  }
  else {
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
        PostMessage( m_Messages.msgProcess, reinterpret_cast<WPARAM>( m_line ) );
        ++m_cntSends;
        // and allocate another buffer
        m_line = m_reposLineBuffers.CheckOut();
        m_line->clear();
      }
      else {
        if ( 0x0d == ch ) {
          // ignore the character
        }
        else {
          m_line->push_back( ch );
        }
      }

      --bytes_transferred;
    } // end while

  }
}

void CNetwork::TimerHandler( const boost::system::error_code& error ) {
  if ( m_bKeepTimerActive ) {
    m_timer.expires_at( m_timer.expires_at() + boost::posix_time::seconds( 1 ) );
    m_timer.async_wait( boost::bind( &CNetwork::TimerHandler, this, boost::asio::placeholders::error ) );
  }
}

LRESULT CNetwork::OnSend( UINT, WPARAM wParam, LPARAM, BOOL &bHandled ) {

  linebuffer_t* pbuffer = reinterpret_cast<linebuffer_t*>( wParam );

  if ( m_bSocketOpen ) {
    boost::asio::async_write( *m_psocket, boost::asio::buffer( *pbuffer ), 
      boost::bind( &CNetwork::WriteHandler, this, 
      boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, pbuffer )
      );
    ++m_cntSends;
  }
  else {
    BOOL b = PostMessage( m_Messages.msgError, ERROR_SOCKET );
  }

  bHandled = true;
  return 1;
}

void CNetwork::WriteHandler( const boost::system::error_code& error, std::size_t bytes_transferred, linebuffer_t* pbuffer ) {
  if ( error ) {
    BOOL b = PostMessage( m_Messages.msgError, ERROR_WRITE ); 
  }
  assert( bytes_transferred == pbuffer->size() );
  m_cntBytesTransferred_send += bytes_transferred;
  PostMessage( m_Messages.msgSendDone, reinterpret_cast<WPARAM>( pbuffer ) );
}

LRESULT CNetwork::OnDisconnect( UINT, WPARAM, LPARAM, BOOL &bHandled ) {

  //m_psocket->cancel();  //  boost::asio::error::operation_aborted, boost::asio::error::operation_not_supported on xp
  m_psocket->close();
  m_bSocketOpen = false;

  BOOL b = PostMessage( m_Messages.msgDisconnected );

  bHandled = true;
  return 1;
}

LRESULT CNetwork::OnProcessed( UINT, WPARAM wparam, LPARAM, BOOL &bHandled ) {

  linebuffer_t* pbuffer = reinterpret_cast<linebuffer_t*>( wparam );
  m_reposLineBuffers.CheckIn( pbuffer );

  bHandled = true;
  return 1;
}

BOOL CNetwork::PostMessage( UINT msg, WPARAM wParam, LPARAM lParam ) {
  BOOL b = false;
  if ( 0 != msg ) {
    if ( 0 != m_Messages.hWnd ) {
      b = ::PostMessage( m_Messages.hWnd, msg, wParam, lParam );
    }
  }
  return b;
}