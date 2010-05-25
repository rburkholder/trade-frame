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

//#include <codeproject/thread.h>  // class inbound messages

#include <boost/asio.hpp>  // class outbound processing
#include <boost/thread.hpp>  // separate thread for asio run processing
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>

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
// http://think-async.com/Asio/Recipes

// use asio post messages to send commands 
// can we base stuff on Lock-Free Queues? http://www.ddj.com/hpc-high-performance-computing/208801974
// input streams are cr/lf delimited
// LOG received buffer length to see if we are actually using big buffers on reception
// maybe do dynamic message pointer updates later
// two threads:
//   * external thread for routines that send message to this class
//   * asio thread for processing outbound and inbound
// collect statistics to see if network side input buffers are being broken up
//   if they are, then remain with character by character delivery
//   if they are not, then can use begin/end iterators to pass information
//   or come up with a different scheme of managing iterators over multiple buffers for use by Spirit
// need code to catch when the socket is closed for whatever reason
// provide an interator capability scan buffers with out the re-copy process, useful for the news parsing libraries

// ownerT:  CRTP class
// charT:  type of character processed 

template <typename ownerT, typename charT = unsigned char>
class CNetwork {
public:

  enum enumMessageErrorTypes: size_t {
    OK = 0,
    ERROR_WRITE,
    ERROR_SOCKET,
    ERROR_CONNECT
  };

#define NETWORK_INPUT_BUF_SIZE 2048

  typedef unsigned short port_t;
  typedef std::string ipaddress_t;

  struct structConnection {
    ipaddress_t sAddress;
    port_t nPort;
    structConnection( const ipaddress_t& sAddress_, port_t nPort_ )
      : sAddress( sAddress_ ), nPort( nPort_ ) {};
    structConnection( void ) : sAddress( "127.0.0.1" ), nPort( 0 ) {};
  };

  // factor a couple of these out as traits for here and for IQFeedMessages.
  typedef charT bufferelement_t;
  typedef boost::array<bufferelement_t, NETWORK_INPUT_BUF_SIZE> inputbuffer_t; // bulk input buffer via asio
  typedef CBufferRepository<inputbuffer_t> inputrepository_t;
  typedef std::vector<bufferelement_t> linebuffer_t;  // used for composing lines of data for processing
  typedef CBufferRepository<linebuffer_t> linerepository_t;

//  CNetwork(CAppModule* pModule, const structMessages&);
  CNetwork( void );
  CNetwork( const structConnection& connection );
  CNetwork( const ipaddress_t& sAddress, port_t nPort );
  ~CNetwork( void );

  void SetPort( port_t port ) { m_Connection.nPort = port; };
  void SetAddress( const ipaddress_t& ipaddress ) { m_Connection.sAddress = ipaddress; };
  void Connect( void );
  void Connect( const structConnection& connection );
  void Disconnect( void );
  void Send( const std::string&, bool bNotifyOnDone = false ); // string being sent out to network
  void GiveBackBuffer( linebuffer_t* p ) { m_reposLineBuffers.CheckInL( p ); };  // parsed buffer being given back to accept more parsed network traffic

protected:

  // CRTP based callbacks:
  void OnNetworkConnected(void) {};
  void OnNetworkDisconnected(void) {};
  void OnNetworkError( size_t ) {;};
  void OnNetworkLineBuffer( linebuffer_t* ) {};  // new line available for processing
  void OnNetworkSendDone(void) {};

private:

  enum enumNetworkState {
    NS_INITIALIZING,
    NS_DISCONNECTED,
    NS_CONNECTING,
    NS_CONNECTED,
    NS_DISCONNECTING, 
    NS_CLOSING,
    NS_CLOSED
  } m_stateNetwork;

  structConnection m_Connection;

  boost::thread m_asioThread;

  boost::asio::io_service m_io;
  boost::asio::io_service::work* m_pwork;
  boost::asio::ip::tcp::socket* m_psocket;

  inputrepository_t m_reposInputBuffers;  // content received from the network
  linerepository_t m_reposLineBuffers;  // parsed lines sent to the callers
  linerepository_t m_reposSendBuffers; // buffers used to send data to network

  linebuffer_t* m_pline;  // current parsing results

  unsigned int m_cntAsyncReads;
  unsigned int m_cntBytesTransferred_input;
  unsigned int m_cntLinesProcessed;

  unsigned int m_cntSends;
  unsigned int m_cntBytesTransferred_send;

  void OnConnectDone( const boost::system::error_code& error );
  void OnSendDoneCommon( const boost::system::error_code& error, std::size_t bytes_transferred, linebuffer_t* );
  void OnSendDone( const boost::system::error_code& error, std::size_t bytes_transferred, linebuffer_t* );
  void OnSendDoneNoNotify( const boost::system::error_code& error, std::size_t bytes_transferred, linebuffer_t* );
  void OnReadDone( const boost::system::error_code& error, std::size_t bytes_transferred, inputbuffer_t* );
  void AsyncRead( void );

  void AsioThread( void );

  void CommonConstruction( void );

};

template <typename ownerT, typename charT>
CNetwork<ownerT,charT>::CNetwork( void )
: 
  m_stateNetwork( NS_INITIALIZING ),
  m_psocket( NULL ), 
  m_cntBytesTransferred_input( 0 ), m_cntAsyncReads( 0 ),
  m_cntSends( 0 ), m_cntBytesTransferred_send( 0 ),
  m_cntLinesProcessed( 0 )
{
  CommonConstruction();
}

template <typename ownerT, typename charT>
CNetwork<ownerT,charT>::CNetwork( const structConnection& connection )
: 
  m_Connection( connection ),
  m_stateNetwork( NS_INITIALIZING ),
  m_psocket( NULL ), 
  m_cntBytesTransferred_input( 0 ), m_cntAsyncReads( 0 ),
  m_cntSends( 0 ), m_cntBytesTransferred_send( 0 ),
  m_cntLinesProcessed( 0 )
{
  CommonConstruction();
}

template <typename ownerT, typename charT>
CNetwork<ownerT,charT>::CNetwork( const ipaddress_t& sAddress, port_t nPort ) 
:
  m_Connection( sAddress, nPort ),
  m_stateNetwork( NS_INITIALIZING ),
  m_psocket( NULL ), 
  m_cntBytesTransferred_input( 0 ), m_cntAsyncReads( 0 ),
  m_cntSends( 0 ), m_cntBytesTransferred_send( 0 ),
  m_cntLinesProcessed( 0 )
{
  CommonConstruction();
}

template <typename ownerT, typename charT>
void CNetwork<ownerT,charT>::CommonConstruction( void ) {
  m_pline = m_reposLineBuffers.CheckOutL();  // have a receiving line ready
  m_pline->clear();
  m_pwork = new boost::asio::io_service::work(m_io);  // keep the asio service running 
  m_asioThread = boost::thread( boost::bind( &CNetwork::AsioThread, this ) );
  m_stateNetwork = NS_DISCONNECTED;
}

template <typename ownerT, typename charT>
CNetwork<ownerT,charT>::~CNetwork(void) {

  if ( NS_CONNECTED == m_stateNetwork ) {
    Disconnect();
  }
  else {
    assert( NS_DISCONNECTED == m_stateNetwork );
  }

  m_stateNetwork = NS_CLOSING;

#if defined _DEBUG
  if ( 0 != m_pline->size() ) {
    OutputDebugString( "CNetwork::~CNetwork: m_line is non-zero in size.\n" );
  }
#endif

  delete m_pwork;  // stop the asio service (let it run out of work, which at this point should be none)
//  m_io.stop();  // kinda redundant

  m_asioThread.join();  // wait for i/o thread to terminate

//  delete m_asioThread;
//  m_asioThread = NULL;

//  if ( m_asioThread. ) {  // need to find check for done and cleared
//    OutputDebugString( "CNetwork::~CNetwork: m_asioThread is not NULL.\n" );
//  }

  // check in our held line so it gets cleaned up
  m_reposLineBuffers.CheckInL( m_pline );
  m_pline = NULL;

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

  m_stateNetwork = NS_CLOSED;

}

template <typename ownerT, typename charT>
void CNetwork<ownerT,charT>::AsioThread( void ) {

  m_io.run();  // handles async socket 

#ifdef _DEBUG
  std::stringstream ss;
  ss << "ASIO Thread Exit:  " << typeid( this ).name() << std::endl;
  OutputDebugString( ss.str().c_str() );
  ss.str() = "";
#endif
}

template <typename ownerT, typename charT>
void CNetwork<ownerT,charT>::Connect( void ) {

#ifdef _DEBUG
  if ( NULL != m_psocket ) {
    OutputDebugString( "CNetwork::OnConnect:  m_psocket not null.\n" );
  }
#endif

  assert( NS_DISCONNECTED == m_stateNetwork );

  m_stateNetwork = NS_CONNECTING;

  assert( 0 != m_Connection.nPort );

  // http://www.boost.org/doc/libs/1_40_0/doc/html/boost_asio/reference/basic_stream_socket/async_connect.html
  m_psocket = new tcp::socket( m_io );
  tcp::endpoint endpoint( boost::asio::ip::address::from_string( m_Connection.sAddress ), m_Connection.nPort );
  m_psocket->async_connect( 
    endpoint, 
    boost::bind<void>( &CNetwork::OnConnectDone, this, boost::asio::placeholders::error ) 
    );
}

template <typename ownerT, typename charT>
void CNetwork<ownerT,charT>::Connect( const structConnection& connection ) {
  m_Connection = connection;
  Connect();
}

template <typename ownerT, typename charT>
void CNetwork<ownerT,charT>::Disconnect( void ) {

  if ( NS_DISCONNECTED != m_stateNetwork ) {
    //m_psocket->cancel();  //  boost::asio::error::operation_aborted, boost::asio::error::operation_not_supported on xp
    if ( NS_CONNECTED == m_stateNetwork ) {
      m_stateNetwork = NS_DISCONNECTING;
      m_psocket->close();
      delete m_psocket;
      m_psocket = NULL;
    }
    else {
      assert( NS_DISCONNECTED == m_stateNetwork );
    }
  }

  m_stateNetwork = NS_DISCONNECTED;

  if ( &CNetwork<ownerT, charT>::OnNetworkDisconnected != &ownerT::OnNetworkDisconnected ) {
    static_cast<ownerT*>( this )->OnNetworkDisconnected();
  }

}

template <typename ownerT, typename charT>
void CNetwork<ownerT,charT>::AsyncRead( void ) {

  assert( NS_CONNECTED == m_stateNetwork );

  inputbuffer_t* pbuffer = m_reposInputBuffers.CheckOutL();
//  if ( NETWORK_INPUT_BUF_SIZE > pbuffer->capacity() ) {
//    pbuffer->reserve( NETWORK_INPUT_BUF_SIZE );
//  }
  m_psocket->async_read_some( boost::asio::buffer( *pbuffer ), 
    boost::bind( 
      &CNetwork::OnReadDone, this,
      boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, pbuffer ) );
}

template <typename ownerT, typename charT>
void CNetwork<ownerT,charT>::OnConnectDone( const boost::system::error_code& error ) {

  if ( error ) {
    if ( &CNetwork<ownerT, charT>::OnNetworkError != &ownerT::OnNetworkError ) {
      static_cast<ownerT*>( this )->OnNetworkError( ERROR_CONNECT );
    }
  }
  else {

    m_stateNetwork = NS_CONNECTED;

    if ( &CNetwork<ownerT, charT>::OnNetworkConnected != &ownerT::OnNetworkConnected ) {
      static_cast<ownerT*>( this )->OnNetworkConnected();
    }

    AsyncRead();
  }
}

template <typename ownerT, typename charT>
void CNetwork<ownerT,charT>::OnReadDone( const boost::system::error_code& error, std::size_t bytes_transferred, inputbuffer_t* pbuffer ) {

  if ( 0 == bytes_transferred ) {
    // probably infers that connection has been closed
    OutputDebugString( "CNetwork::ReadHandler: connection probably closed.\n" );
  }
  else {
    assert( NS_CONNECTED == m_stateNetwork );

    ++m_cntAsyncReads;
    m_cntBytesTransferred_input += bytes_transferred;

    AsyncRead();  // set up for another read while processing existing buffer

    // process the buffer:
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
        if ( &CNetwork<ownerT, charT>::OnNetworkLineBuffer != &ownerT::OnNetworkLineBuffer ) {
          static_cast<ownerT*>( this )->OnNetworkLineBuffer( m_pline );
        }
        ++m_cntLinesProcessed;
        // and allocate another buffer
        m_pline = m_reposLineBuffers.CheckOutL();
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
  m_reposInputBuffers.CheckInL( pbuffer );
}

template <typename ownerT, typename charT>
void CNetwork<ownerT,charT>::Send( const std::string& send, bool bNotifyOnDone ) {

  if ( NS_CONNECTED == m_stateNetwork ) {
    linerepository_t::buffer_t pbuffer = m_reposSendBuffers.CheckOutL();
    pbuffer->clear();
    BOOST_FOREACH( char ch, send ) {
      (*pbuffer).push_back( ch );
    }
    if ( bNotifyOnDone ) {
      boost::asio::async_write( *m_psocket, boost::asio::buffer( *pbuffer ), 
        boost::bind( &CNetwork::OnSendDone, this,
        boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, pbuffer )
        );
    }
    else {
      boost::asio::async_write( *m_psocket, boost::asio::buffer( *pbuffer ), 
        boost::bind( &CNetwork::OnSendDoneNoNotify, this,
        boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, pbuffer )
        );
    }
    ++m_cntSends;
  }
  else {
    if ( &CNetwork<ownerT, charT>::OnNetworkError != &ownerT::OnNetworkError ) {
      static_cast<ownerT*>( this )->OnNetworkError( ERROR_SOCKET );
    }
  }
}

template <typename ownerT, typename charT>
void CNetwork<ownerT,charT>::OnSendDoneCommon( 
        const boost::system::error_code& error, 
        std::size_t bytes_transferred, 
        linebuffer_t* pbuffer
        ) {

  assert( NS_CONNECTED == m_stateNetwork );

  m_reposSendBuffers.CheckInL( pbuffer );
  assert( bytes_transferred == pbuffer->size() );
  m_cntBytesTransferred_send += bytes_transferred;

  if ( 0 != error ) {
    if ( &CNetwork<ownerT, charT>::OnNetworkError != &ownerT::OnNetworkError ) {
      static_cast<ownerT*>( this )->OnNetworkError( ERROR_WRITE );
    }
  }
}

template <typename ownerT, typename charT>
void CNetwork<ownerT,charT>::OnSendDone( 
        const boost::system::error_code& error, 
        std::size_t bytes_transferred, 
        linebuffer_t* pbuffer
        ) {
  OnSendDoneCommon( error, bytes_transferred, pbuffer );
  if ( 0 != error ) {
    if ( &CNetwork<ownerT, charT>::OnNetworkSendDone != &ownerT::OnNetworkSendDone ) {
      static_cast<ownerT*>( this )->OnNetworkSendDone();
    }
  }
}

template <typename ownerT, typename charT>
void CNetwork<ownerT,charT>::OnSendDoneNoNotify( 
        const boost::system::error_code& error, 
        std::size_t bytes_transferred, 
        linebuffer_t* pbuffer) {
  OnSendDoneCommon( error, bytes_transferred, pbuffer );
}

