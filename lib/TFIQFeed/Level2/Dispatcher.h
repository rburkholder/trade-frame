/************************************************************************
 * Copyright(c) 2021, One Unified. All rights reserved.                 *
 * email: info@oneunified.net                                           *
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

/*
 * File:    Dispatcher.h
 * Author:  raymond@burkholder.net
 * Project: TFIQFeed/Level2
 * Created on October 17, 2021 10:56
 */

#pragma once

#include <string>

#include <OUCommon/Network.h>

#include <TFIQFeed/Level2/MsgOrderArrival.h>
#include <TFIQFeed/Level2/MsgOrderDelete.h>

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed
namespace l2 { // level 2 data

template <typename T>
class Dispatcher
: public ou::Network<Dispatcher<T> > {
  friend ou::Network<Dispatcher<T> >;
public:

  Dispatcher();
  virtual ~Dispatcher();

  void Connect();
  void Disconnect();

  // http://www.iqfeed.net/dev/api/docs/docsBeta/MarketDepth.cfm
  void StartMarketByOrder( const std::string& );
  void StopMarketByOrder( const std::string& );
  void StartPriceLevel( const std::string& );
  void StopPriceLevel( const std::string& );

protected:

  // translated from network layer to eliminate name clash
  void OnL2Connected() {}
  void OnL2Disconnected() {}
  void OnL2Error( size_t e ) {}
  void OnL2SendDone() {}

  void OnL2Initialized();

  void OnMBOAdd( const ou::tf::iqfeed::l2::msg::OrderArrival::decoded& ) {}
  void OnMBOSummary( const ou::tf::iqfeed::l2::msg::OrderArrival::decoded& ) {}
  void OnMBOUpdate( const ou::tf::iqfeed::l2::msg::OrderArrival::decoded& ) {}
  void OnMBODelete( const ou::tf::iqfeed::l2::msg::OrderDelete::decoded& ) {}

private:

  using l2_inherited_t = typename ou::Network<Dispatcher<T> >;
  using l2_linebuffer_t = typename l2_inherited_t::linebuffer_t;

  bool m_bInitialized;

  ou::tf::iqfeed::l2::msg::OrderArrival::parser_decoded<typename l2_linebuffer_t::iterator> m_parserArrival;
  ou::tf::iqfeed::l2::msg::OrderDelete::parser_decoded<typename l2_linebuffer_t::iterator> m_parserDelete;

  // called by Network via CRTP
  void OnNetworkConnected();
  void OnNetworkDisconnected();
  void OnNetworkError( size_t e );
  void OnNetworkSendDone();
  void OnNetworkLineBuffer( l2_linebuffer_t* );  // new line available for processing

};

template <typename T>
Dispatcher<T>::Dispatcher( void )
: ou::Network<Dispatcher<T> >( "127.0.0.1", 9200 ),
  m_bInitialized( false )
{}

template <typename T>
Dispatcher<T>::~Dispatcher(void) {
}

template <typename T>
void Dispatcher<T>::Connect() {
  ou::Network<Dispatcher<T> >::Connect();
}

template <typename T>
void Dispatcher<T>::Disconnect() {
  ou::Network<Dispatcher<T> >::Disconnect();
}

template <typename T>
void Dispatcher<T>::OnNetworkConnected() {
  if ( &Dispatcher<T>::OnL2Connected != &T::OnL2Connected ) {
    static_cast<T*>( this )->OnL2Connected();
  }
}

template <typename T>
void Dispatcher<T>::OnNetworkDisconnected() {
  if ( &Dispatcher<T>::OnL2Disconnected != &T::OnL2Disconnected ) {
    static_cast<T*>( this )->OnL2Disconnected();
  }
  std::cout << "Dispatcher<T>::OnNetworkDisconnected()" << std::endl;
  m_bInitialized = false;
}

template <typename T>
void Dispatcher<T>::OnNetworkError( size_t e ) {
  if ( &Dispatcher<T>::OnL2Error != &T::OnL2Error ) {
    static_cast<T*>( this )->OnL2Error( e );
  }
}

template <typename T>
void Dispatcher<T>::OnNetworkSendDone() {
  if ( &Dispatcher<T>::OnL2SendDone != &T::OnL2SendDone ) {
    static_cast<T*>( this )->OnL2SendDone();
  }
}

template <typename T>
void Dispatcher<T>::OnL2Initialized() {
  if ( &Dispatcher<T>::OnL2Initialized != &T::OnL2Initialized ) {
    static_cast<T*>( this )->OnL2Initialized();
  }
}

template <typename T>
void Dispatcher<T>::StartMarketByOrder( const std::string& sName ) {
  ou::Network<Dispatcher<T> >::Send( "WOR," + sName + "\n" );
}

template <typename T>
void Dispatcher<T>::StopMarketByOrder( const std::string& sName ) {
  ou::Network<Dispatcher<T> >::Send( "ROR," + sName + "\n" );
}

template <typename T>
void Dispatcher<T>::StartPriceLevel( const std::string& sName ) {
  // TODO: add MaxPriceLevels
  ou::Network<Dispatcher<T> >::Send( "WPL," + sName + "\n" );
}

template <typename T>
void Dispatcher<T>::StopPriceLevel( const std::string& sName ) {
  ou::Network<Dispatcher<T> >::Send( "RPL," + sName + "\n" );
}

template <typename T>
void Dispatcher<T>::OnNetworkLineBuffer( l2_linebuffer_t* pBuffer ) {

  typename l2_linebuffer_t::iterator iter = (*pBuffer).begin();
  typename l2_linebuffer_t::iterator end = (*pBuffer).end();

  BOOST_ASSERT( iter != end );

  //std::string str( iter, end );
  //if ( '5' == str[0] ) {
  //  std::cout << "MarketDepth: '" << str << "'" << std::endl;
  //}

  switch ( *iter ) {
    case '3': // Order Add
      if ( &Dispatcher<T>::OnMBOAdd != &T::OnMBOAdd ) {
        namespace OrderArrival = ou::tf::iqfeed::l2::msg::OrderArrival;
        OrderArrival::decoded msg;
        if ( OrderArrival::Decode( m_parserArrival, msg, iter, end) ) {
          static_cast<T*>( this )->OnMBOAdd( msg );
        }
        else {
          std::cout << "MarketDepth Order Add error" << std::endl;
        }
      }
      break;
    case '4': // Order/Level2 Update
      if ( &Dispatcher<T>::OnMBOUpdate != &T::OnMBOUpdate ) {
        namespace OrderArrival = ou::tf::iqfeed::l2::msg::OrderArrival;
        OrderArrival::decoded msg;
        if ( OrderArrival::Decode( m_parserArrival, msg, iter, end) ) {
          static_cast<T*>( this )->OnMBOUpdate( msg );
        }
        else {
          std::cout << "MarketDepth Order Update error" << std::endl;
        }
      }
      break;
    case '5': // Order Delete
      if ( &Dispatcher<T>::OnMBODelete != &T::OnMBODelete ) {
        namespace OrderDelete = ou::tf::iqfeed::l2::msg::OrderDelete;
        OrderDelete::decoded msg;
        if ( OrderDelete::Decode( m_parserDelete, msg, iter, end) ) {
          static_cast<T*>( this )->OnMBODelete( msg );
        }
        else {
          std::string str( iter, end );
          std::cout
            << "MarketDepth Order Delete error: '"
            << str
            << "'"
            << "|" << ( end - iter )
            << std::endl;
        }
      }
      break;
    case '6': // Order/Level2 Summary
      if ( &Dispatcher<T>::OnMBOSummary != &T::OnMBOSummary ) {
        namespace OrderArrival = ou::tf::iqfeed::l2::msg::OrderArrival;
        OrderArrival::decoded msg;
        if ( OrderArrival::Decode( m_parserArrival, msg, iter, end) ) {
          static_cast<T*>( this )->OnMBOSummary( msg );
        }
        else {
          std::cout << "MarketDepth Order Summary error" << std::endl;
        }
      }
      break;
    case '7': // Price Level Summary
      std::cout << "MarketDepth Price Level Summary not implemented" << std::endl;
      break;
    case '8': // Price Level Update
      std::cout << "MarketDepth Price Level Update not implemented" << std::endl;
      break;
    case '9': // Price Level Delete
      std::cout << "MarketDepth Price Level Delete not implemented" << std::endl;
      break;
    case 'T':
      // ignore timestamp message
    case 'S':
      if ( !m_bInitialized ) {
        m_bInitialized = true;
        // make a message keyword parser? - from the spirit contribution repository
        // TODO: for field comparisons, use spirit or the trie method
        ou::Network<Dispatcher<T> >::Send( "S,TIMESTAMPSOFF\n" );  // TODO: maybe send on S,KEYOK, check that there are no listeners to the event
        ou::Network<Dispatcher<T> >::Send( "S,SET PROTOCOL,6.2\n" );
      }
      else {
        std::string str( iter, end );
        if ( "S,CURRENT PROTOCOL,6.2," == str ) {
          OnL2Initialized();
        }
      }
      break;
    case 'n':
      {
        std::string str( iter, end );
        std::cout << "MarketDepth Unknown symbol: '" << str << "'" << std::endl;
      }
      break;
    case 'q':
      {
        std::string str( iter, end );
        std::cout << "MarketDepth no depth available: '" << str << "'" << std::endl;
      }
      break;
    default:
      {
        //throw "Unknown message type in port 9200"; // unknown message type
        std::string str( iter, end );
        std::cout << "MarketDepth unknown message type: '" << str << "'" << std::endl;
      }
      break;
  }

  l2_inherited_t::GiveBackBuffer( pBuffer );

}

} // namespace l2
} // namesapce iqfeed
} // namespace tf
} // namespace ou
