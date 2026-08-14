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

#include <string>
#include <vector>
#include <cassert>
#include <iostream>

#include <boost/asio/post.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/executor_work_guard.hpp>

#include <boost/array.hpp>
#include <boost/thread.hpp>  // separate thread for asio run processing
#include <boost/bind/bind.hpp>
#include <boost/interprocess/detail/atomic.hpp>

#include <OUCommon/Debug.h>

#include "ReusableBuffers.h"

// example timeout code
// http://www.boost.org/doc/libs/1_43_0/doc/html/boost_asio/example/timeouts/connect_timeout.cpp
// http://www.boost.org/doc/libs/1_54_0/doc/html/boost_asio/example/cpp03/timeouts/async_tcp_client.cpp

using boost::asio::ip::tcp;

// 2023/10/01
// https://www.boost.org/doc/libs/1_83_0/doc/html/boost_asio/overview/implementation.html#boost_asio.overview.implementation.linux_kernel_5_10
  //If BOOST_ASIO_HAS_IO_URING is defined, uses io_uring for file-related asynchonous operations.
  //Uses epoll for demultiplexing other event sources.
  //Optionally uses io_uring for all asynchronous operations if, in addition to BOOST_ASIO_HAS_IO_URING, BOOST_ASIO_DISABLE_EPOLL is defined to disable epoll.


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

namespace ou {

template <typename ownerT, typename charT = unsigned char>
class Network {
public:

  enum enumMessageErrorTypes: size_t {
    OK = 0,
    ERROR_WRITE,
    ERROR_SOCKET,
    ERROR_CONNECT
  };

#define NETWORK_INPUT_BUF_SIZE 2048

  using port_t = unsigned short;
  using ipaddress_t =  std::string;

  struct structConnection {
    ipaddress_t sAddress;
    port_t nPort;
    structConnection( const ipaddress_t& sAddress_, port_t nPort_ )
      : sAddress( sAddress_ ), nPort( nPort_ ) {};
    structConnection( void ) : sAddress( "127.0.0.1" ), nPort( 0 ) {};
  };

  // factor a couple of these out as traits for here and for IQFeedMessages.
  using bufferelement_t = charT;
  using inputbuffer_t = boost::array<bufferelement_t, NETWORK_INPUT_BUF_SIZE>; // bulk input buffer via asio
  using inputrepository_t = BufferRepository<inputbuffer_t>;
  using linebuffer_t = std::vector<bufferelement_t>;  // used for composing lines of data for processing
  using linerepository_t = BufferRepository<linebuffer_t>;

  Network();
  Network( const structConnection& connection );
  Network( const ipaddress_t& sAddress, port_t nPort );
  virtual ~Network( void );

  void SetPort( port_t port ) { m_Connection.nPort = port; };
  void SetAddress( const ipaddress_t& ipaddress ) { m_Connection.sAddress = ipaddress; };
  void Connect();
  void Connect( const structConnection& connection );
  void Disconnect();
  void Send( const std::string&, bool bNotifyOnDone = false ); // string being sent out to network
  void GiveBackBuffer( linebuffer_t* p ) { m_reposLineBuffers.CheckInL( p ); };  // parsed buffer being given back to accept more parsed network traffic

protected:

  // CRTP based dummy callbacks
  void OnNetworkConnected() {};
  void OnNetworkDisconnected() {};
  void OnNetworkError( size_t ) {;};
  void OnNetworkLineBuffer( linebuffer_t* ) {};  // new line available for processing
  void OnNetworkSendDone() {};

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

  boost::asio::io_context m_io_context;

  using work_guard_t = boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;
  using pWorkGuard_t = std::unique_ptr<work_guard_t>;
  pWorkGuard_t m_pWorkGuard;

  boost::asio::ip::tcp::socket* m_psocket;

  boost::asio::steady_timer m_timer;

  // variables used to sync for thread ending
  volatile boost::uint32_t m_cntActiveSends;  // number of active sends
  volatile boost::uint32_t m_lReadProgress; // >0 when read buffer processing is active

  inputrepository_t m_reposInputBuffers;  // content received from the network
  linerepository_t m_reposLineBuffers;  // parsed lines sent to the callers
  linerepository_t m_reposSendBuffers; // buffers used to send data to network

  linebuffer_t* m_pline;  // current parsing results

  size_t m_cntAsyncReads;
  size_t m_cntBytesTransferred_input;
  size_t m_cntLinesProcessed;

  size_t m_cntSends;
  size_t m_cntBytesTransferred_send;

  void OnConnectDone( const boost::system::error_code& error );
  void OnNetDisconnecting( void);
  void OnSendDoneCommon( const boost::system::error_code& error, std::size_t bytes_transferred, linebuffer_t* );
  void OnSendDone( const boost::system::error_code& error, std::size_t bytes_transferred, linebuffer_t* );
  void OnSendDoneNoNotify( const boost::system::error_code& error, std::size_t bytes_transferred, linebuffer_t* );
  void OnReadDone( const boost::system::error_code& error, const std::size_t bytes_transferred, inputbuffer_t* );
  void AsyncRead( void );

  void AsioThread( void );

  void CommonConstruction( void );
  void OnTimeOut( void );

};

//
// Network Construction
//

template <typename ownerT, typename charT>
Network<ownerT,charT>::Network()
:
  m_stateNetwork( NS_INITIALIZING ),
  m_psocket( NULL ),
  m_cntBytesTransferred_input( 0 ), m_cntAsyncReads( 0 ),
  m_cntSends( 0 ), m_cntBytesTransferred_send( 0 ),
  m_cntLinesProcessed( 0 ),
  m_cntActiveSends( 0 ), m_lReadProgress( 0 ),
  m_timer( m_io_context )
{
  CommonConstruction();
}

//
// Network Construction
//

template <typename ownerT, typename charT>
Network<ownerT,charT>::Network( const structConnection& connection )
:
  m_Connection( connection ),
  m_stateNetwork( NS_INITIALIZING ),
  m_psocket( NULL ),
  m_cntBytesTransferred_input( 0 ), m_cntAsyncReads( 0 ),
  m_cntSends( 0 ), m_cntBytesTransferred_send( 0 ),
  m_cntLinesProcessed( 0 ),
  m_cntActiveSends( 0 ), m_lReadProgress( 0 ),
  m_timer( m_io_context )

{
  CommonConstruction();
}

//
// Network Construction
//

template <typename ownerT, typename charT>
Network<ownerT,charT>::Network( const ipaddress_t& sAddress, port_t nPort )
:
  m_Connection( sAddress, nPort ),
  m_stateNetwork( NS_INITIALIZING ),
  m_psocket( NULL ),
  m_cntBytesTransferred_input( 0 ), m_cntAsyncReads( 0 ),
  m_cntSends( 0 ), m_cntBytesTransferred_send( 0 ),
  m_cntLinesProcessed( 0 ),
  m_cntActiveSends( 0 ), m_lReadProgress( 0 ),
  m_timer( m_io_context )
{
  CommonConstruction();
}

//
// CommonConstruction
//

template <typename ownerT, typename charT>
void Network<ownerT,charT>::CommonConstruction() {
  m_pline = m_reposLineBuffers.CheckOutL();  // have a receiving line ready
  m_pline->clear();
  m_pWorkGuard = std::make_unique<work_guard_t>( boost::asio::make_work_guard( m_io_context ) );
  m_asioThread = boost::thread( boost::bind( &Network::AsioThread, this ) );
  m_stateNetwork = NS_DISCONNECTED;
}

//
// Destructor
//

template <typename ownerT, typename charT>
Network<ownerT,charT>::~Network() {

  m_timer.cancel();

  if ( NS_CONNECTED == m_stateNetwork ) {
    Disconnect();
  }
  else {
    assert( ( NS_DISCONNECTED == m_stateNetwork ) || ( NS_DISCONNECTING == m_stateNetwork ) );
  }

  m_pWorkGuard.reset();  // stop the asio service (let it run out of work, which at this point should be none)
//  m_io.stop();  // kinda redundant

  m_asioThread.join();  // wait for i/o thread to cleanup and terminate

  m_stateNetwork = NS_CLOSING;

//  delete m_asioThread;
//  m_asioThread = NULL;

//  if ( m_asioThread. ) {  // need to find check for done and cleared
//    OutputDebugString( "Network::~Network: m_asioThread is not NULL.\n" );
//  }

#if defined _DEBUG
  if ( 0 != m_pline->size() ) {
//    OutputDebugString( "Network::~Network: m_line is non-zero in size.\n" );
  }
#endif

  // check in our held line so it gets cleaned up
  m_reposLineBuffers.CheckInL( m_pline );
  m_pline = NULL;

#if defined _DEBUG
  DEBUGOUT( typeid( this ).name()
    << " " << m_cntBytesTransferred_input << " bytes in on "
    << m_cntAsyncReads << " reads with " << m_cntLinesProcessed << " lines out, "
    << m_cntBytesTransferred_send << " bytes out on "
    << m_cntSends << " sends."
    << std::endl
  )
#endif

  m_stateNetwork = NS_CLOSED;

}

//
// AsioThread
//

template <typename ownerT, typename charT>
void Network<ownerT,charT>::AsioThread() {

  m_io_context.run();  // handles async socket

#ifdef _DEBUG
  DEBUGOUT( "ASIO Thread Exit:  " << typeid( this ).name() << std::endl )
#endif
}

//
// Connect
//

template <typename ownerT, typename charT>
void Network<ownerT,charT>::Connect() {

#ifdef _DEBUG
  if ( NULL != m_psocket ) {
    DEBUGOUT( "Network::OnConnect:  m_psocket not null.\n" );
  }
#endif

  assert( NS_DISCONNECTED == m_stateNetwork );

  m_stateNetwork = NS_CONNECTING;

  assert( 0 != m_Connection.nPort );

  // http://www.boost.org/doc/libs/1_40_0/doc/html/boost_asio/reference/basic_stream_socket/async_connect.html
  m_psocket = new tcp::socket( m_io_context );
  tcp::endpoint endpoint( boost::asio::ip::make_address( m_Connection.sAddress ), m_Connection.nPort );
  m_psocket->async_connect(
    endpoint,
    boost::bind<void>( &Network::OnConnectDone, this, boost::asio::placeholders::error )
    );
  m_timer.expires_after( boost::asio::chrono::seconds( 2 ) );
  m_timer.async_wait( boost::bind<void>( &Network::OnTimeOut, this ) );
}

template <typename ownerT, typename charT>
void Network<ownerT,charT>::OnTimeOut( void ) {
  if ( NS_CONNECTING == m_stateNetwork ) {
    Network::Disconnect();
  }
}

//
// Connect
//

template <typename ownerT, typename charT>
void Network<ownerT,charT>::Connect( const structConnection& connection ) {
  m_Connection = connection;
  Connect();
}

//
// OnConnectDone
//

template <typename ownerT, typename charT>
void Network<ownerT,charT>::OnConnectDone( const boost::system::error_code& error ) {

  if ( error ) {
    if ( &Network<ownerT, charT>::OnNetworkError != &ownerT::OnNetworkError ) {
      static_cast<ownerT*>( this )->OnNetworkError( ERROR_CONNECT );
    }
  }
  else {

    m_stateNetwork = NS_CONNECTED;

    if ( &Network<ownerT, charT>::OnNetworkConnected != &ownerT::OnNetworkConnected ) {
      static_cast<ownerT*>( this )->OnNetworkConnected();
    }

    AsyncRead();
  }
}

//
// Disconnect
//

template <typename ownerT, typename charT>
void Network<ownerT,charT>::Disconnect() {

  if ( NS_DISCONNECTED != m_stateNetwork ) {
    //m_psocket->cancel();  //  boost::asio::error::operation_aborted, boost::asio::error::operation_not_supported on xp
    switch ( m_stateNetwork ) {
    case NS_CONNECTED:
      m_stateNetwork = NS_DISCONNECTING;
      m_psocket->shutdown( boost::asio::ip::tcp::socket::shutdown_both );
      OnNetDisconnecting();
      break;
    case NS_CONNECTING:
      m_stateNetwork = NS_DISCONNECTING;
      //m_psocket->close();
      OnNetDisconnecting();
      break;
    default:
      assert( NS_DISCONNECTING == m_stateNetwork );
    }
  }
}

//
// OnDisconnnecting
// an async call
//

template <typename ownerT, typename charT>
void Network<ownerT,charT>::OnNetDisconnecting() {
  if ( ( 0 == m_cntActiveSends ) // there are no active sends
    && ( 0 == m_lReadProgress )  // no reads in progress
//    && ( !m_reposLineBuffers.Outstanding() )  // all clients buffers have been returned. [ can't as destroy doesn't clean up]
  ) {
    // signal disconnect complete
    m_psocket->close();
    delete m_psocket;
    m_psocket = NULL;

    m_stateNetwork = NS_DISCONNECTED;
    if ( &Network<ownerT, charT>::OnNetworkDisconnected != &ownerT::OnNetworkDisconnected ) {
      static_cast<ownerT*>( this )->OnNetworkDisconnected();
    }
  }
  else {
    // wait for operations to complete, by posting a message to the io processing queue
    boost::asio::post( m_io_context, boost::bind( &Network::OnNetDisconnecting, this ) );
  }
}

//
// AsyncRead
//

template <typename ownerT, typename charT>
void Network<ownerT,charT>::AsyncRead() {

  if ( NS_DISCONNECTING == m_stateNetwork ) {
    //bool i = true;  // break point, waiting for network stuff to clear out
  }
  else {
    assert( NS_CONNECTED == m_stateNetwork );
  }

  //InterlockedIncrement( &m_lReadProgress );
  boost::interprocess::ipcdetail::atomic_inc32( &m_lReadProgress );

  inputbuffer_t* pbuffer = m_reposInputBuffers.CheckOutL();
//  if ( NETWORK_INPUT_BUF_SIZE > pbuffer->capacity() ) {
//    pbuffer->reserve( NETWORK_INPUT_BUF_SIZE );
//  }
  m_psocket->async_read_some( boost::asio::buffer( *pbuffer ),
    boost::bind(
      &Network::OnReadDone, this,
      boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, pbuffer ) );
}

//
// OnReadDone
//

template <typename ownerT, typename charT>
void Network<ownerT,charT>::OnReadDone( const boost::system::error_code& error, const std::size_t bytes_transferred_, inputbuffer_t* pbuffer ) {

  size_t bytes_transferred( bytes_transferred_ ); // need to keep original number for debugging

  if ( 0 == bytes_transferred ) {
    // probably infers that connection has been closed
//    OutputDebugString( "Network::ReadHandler: connection probably closed.\n" );
    if ( 0 != m_pline->size() ) {
      m_pline->clear();
    }
  }
  else {
    assert( ( NS_CONNECTED == m_stateNetwork ) || ( NS_DISCONNECTING == m_stateNetwork) );

    ++m_cntAsyncReads;
    m_cntBytesTransferred_input += bytes_transferred;

    AsyncRead();  // set up for another read while processing existing buffer

    // process the buffer:
    // need current linebuffer, need stats on how often it waits for subsequent bulk data
    bufferelement_t ch;
    typename inputbuffer_t::const_iterator input = pbuffer->begin();
    while ( 0 != bytes_transferred ) {
      ch = *input;
      ++input;
      if ( 0 == ch ) {
//        OutputDebugString( "Network::ReadHandler: have a 0x00 character.\n" );
      }
      if ( 0x0a == ch ) {
        // send the buffer off
        try {
          if ( &Network<ownerT, charT>::OnNetworkLineBuffer != &ownerT::OnNetworkLineBuffer ) {
            static_cast<ownerT*>( this )->OnNetworkLineBuffer( m_pline );
          }
        }
        catch( const std::logic_error& e ) {
          std::cerr << "Network<>::OnReadDone logic error: " << e.what() << std::endl;
        }
        catch( const std::runtime_error& e ) {
          std::cerr << "Network<>::OnReadDone runtime error: " << e.what() << std::endl;
        }
        catch( const std::exception& e ) {
          std::cerr << "Network<>::OnReadDone exception: " << e.what() << std::endl;
        }
        catch(...) {
          std::cerr << "Network<>::OnReadDone default exception handler" << std::endl;
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

  //InterlockedDecrement( &m_lReadProgress );
  boost::interprocess::ipcdetail::atomic_dec32( &m_lReadProgress );
}

//
// Send
//

template <typename ownerT, typename charT>
void Network<ownerT,charT>::Send( const std::string& send, bool bNotifyOnDone ) {

  if ( NS_CONNECTED == m_stateNetwork ) {
    //InterlockedIncrement( &m_cntActiveSends );
    boost::interprocess::ipcdetail::atomic_inc32( &m_cntActiveSends );

    typename linerepository_t::pBuffer_t pbuffer = m_reposSendBuffers.CheckOutL();
    pbuffer->clear();
    for ( char ch: send ) {
      (*pbuffer).push_back( ch );
    }
    if ( bNotifyOnDone ) {
      boost::asio::async_write( *m_psocket, boost::asio::buffer( *pbuffer ),
        boost::bind( &Network::OnSendDone, this,
        boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, pbuffer )
        );
    }
    else {
      boost::asio::async_write( *m_psocket, boost::asio::buffer( *pbuffer ),
        boost::bind( &Network::OnSendDoneNoNotify, this,
        boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, pbuffer )
        );
    }
    ++m_cntSends;
  }
  else {
    if ( &Network<ownerT, charT>::OnNetworkError != &ownerT::OnNetworkError ) {
      static_cast<ownerT*>( this )->OnNetworkError( ERROR_SOCKET );
    }
  }
}

//
// OnSendDoneCommon
//

template <typename ownerT, typename charT>
void Network<ownerT,charT>::OnSendDoneCommon(
        const boost::system::error_code& error,
        std::size_t bytes_transferred,
        linebuffer_t* pbuffer
        ) {

  assert( ( NS_CONNECTED == m_stateNetwork ) || ( NS_DISCONNECTING == m_stateNetwork) );

  m_reposSendBuffers.CheckInL( pbuffer );
  //assert( bytes_transferred == pbuffer->size() );
  if ( bytes_transferred != pbuffer->size() ) {
    std::cerr << "network::OnSendDoneCommon bt=" << bytes_transferred << ", size=" << pbuffer->size() << std::endl;
    //std::cout << "network.h::OnSendDoneCommon lb=" << pbuffer->
  }
  m_cntBytesTransferred_send += bytes_transferred;

//  InterlockedDecrement( &m_cntActiveSends );
  boost::interprocess::ipcdetail::atomic_dec32( &m_cntActiveSends );

  if ( 0 != error.value() ) {
    if ( &Network<ownerT, charT>::OnNetworkError != &ownerT::OnNetworkError ) {
      static_cast<ownerT*>( this )->OnNetworkError( ERROR_WRITE );
    }
  }
}

//
// OnSendDone
//

template <typename ownerT, typename charT>
void Network<ownerT,charT>::OnSendDone(
        const boost::system::error_code& error,
        std::size_t bytes_transferred,
        linebuffer_t* pbuffer
        ) {
  OnSendDoneCommon( error, bytes_transferred, pbuffer );
  if ( 0 != error.value() ) {
    if ( &Network<ownerT, charT>::OnNetworkSendDone != &ownerT::OnNetworkSendDone ) {
      static_cast<ownerT*>( this )->OnNetworkSendDone();
    }
  }
}

//
// OnSendDoneNoNotify
//

template <typename ownerT, typename charT>
void Network<ownerT,charT>::OnSendDoneNoNotify(
        const boost::system::error_code& error,
        std::size_t bytes_transferred,
        linebuffer_t* pbuffer) {
  OnSendDoneCommon( error, bytes_transferred, pbuffer );
}

} // ou
