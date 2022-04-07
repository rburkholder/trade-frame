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

  using inherited_t = typename ou::Network<Dispatcher<T> >;
  using linebuffer_t = typename inherited_t::linebuffer_t;

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

  // called by Network via CRTP
  void OnNetworkConnected();
  void OnNetworkDisconnected();
  void OnNetworkError( size_t e );
  void OnNetworkSendDone();
  void OnNetworkLineBuffer( linebuffer_t* );  // new line available for processing

  void Initialized();

  void OnMBOAdd( const ou::tf::iqfeed::l2::msg::OrderArrival::decoded& ) {}
  void OnMBOSummary( const ou::tf::iqfeed::l2::msg::OrderArrival::decoded& ) {}
  void OnMBOUpdate( const ou::tf::iqfeed::l2::msg::OrderArrival::decoded& ) {}
  void OnMBODelete( const ou::tf::iqfeed::l2::msg::OrderDelete::decoded& ) {}

private:
  bool m_bInitialized;
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
  if ( &Dispatcher<T>::OnNetworkConnected != &T::OnNetworkConnected ) {
    static_cast<T*>( this )->OnNetworkConnected();
  }
}

template <typename T>
void Dispatcher<T>::OnNetworkDisconnected() {
  if ( &Dispatcher<T>::OnNetworkDisconnected != &T::OnNetworkDisconnected ) {
    static_cast<T*>( this )->OnNetworkDisconnected();
  }
  std::cout << "Dispatcher<T>::OnNetworkDisconnected()" << std::endl;
  m_bInitialized = false;
}

template <typename T>
void Dispatcher<T>::OnNetworkError( size_t e ) {
}

template <typename T>
void Dispatcher<T>::OnNetworkSendDone() {
}

template <typename T>
void Dispatcher<T>::Initialized() {
  if ( &Dispatcher<T>::Initialized != &T::Initialized ) {
    static_cast<T*>( this )->Initialized();
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
void Dispatcher<T>::OnNetworkLineBuffer( linebuffer_t* pBuffer ) {

  typename linebuffer_t::iterator iter = (*pBuffer).begin();
  typename linebuffer_t::iterator end = (*pBuffer).end();

  BOOST_ASSERT( iter != end );

  std::string str( iter, end );
  //if ( '5' == str[0] ) {
  //  std::cout << "MarketDepth: '" << str << "'" << std::endl;
  //}

  switch ( *iter ) {
    case '3': // Order Add
      {
        namespace OrderArrival = ou::tf::iqfeed::l2::msg::OrderArrival;
        OrderArrival::decoded msg;
        if ( OrderArrival::Decode( msg, iter, end) ) {
          if ( &Dispatcher<T>::OnMBOAdd != &T::OnMBOAdd ) {
            static_cast<T*>( this )->OnMBOAdd( msg );
          }
        }
        else {
          std::cout << "MarketDepth Order Add error" << std::endl;
        }
      }
      break;
    case '4': // Order/Level2 Update
      {
        namespace OrderArrival = ou::tf::iqfeed::l2::msg::OrderArrival;
        OrderArrival::decoded msg;
        if ( OrderArrival::Decode( msg, iter, end) ) {
          if ( &Dispatcher<T>::OnMBOUpdate != &T::OnMBOUpdate ) {
            static_cast<T*>( this )->OnMBOUpdate( msg );
          }
        }
        else {
          std::cout << "MarketDepth Order Update error" << std::endl;
        }
      }
      break;
    case '5': // Order Delete
      {
        namespace OrderDelete = ou::tf::iqfeed::l2::msg::OrderDelete;
        OrderDelete::decoded msg;
        if ( OrderDelete::Decode( msg, iter, end) ) {
          if ( &Dispatcher<T>::OnMBODelete != &T::OnMBODelete ) {
            static_cast<T*>( this )->OnMBODelete( msg );
          }
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
      {
        namespace OrderArrival = ou::tf::iqfeed::l2::msg::OrderArrival;
        OrderArrival::decoded msg;
        if ( OrderArrival::Decode( msg, iter, end) ) {
          if ( &Dispatcher<T>::OnMBOSummary != &T::OnMBOSummary ) {
            static_cast<T*>( this )->OnMBOSummary( msg );
          }
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
          Initialized();
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

  inherited_t::GiveBackBuffer( pBuffer );

}

} // namespace l2
} // namesapce iqfeed
} // namespace tf
} // namespace ou
