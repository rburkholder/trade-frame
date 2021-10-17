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

#include <OUCommon/Network.h>

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed
namespace l2 { // level 2 data

template <typename T>
class Dispatcher: public ou::Network<Dispatcher<T> > {
  friend ou::Network<Dispatcher<T> >;
public:

  using inherited_t = typename ou::Network<Dispatcher<T> >;
  using linebuffer_t = typename inherited_t::linebuffer_t;

  Dispatcher();
  virtual ~Dispatcher();

protected:

  // called by Network via CRTP
  void OnNetworkConnected(void);
  void OnNetworkDisconnected(void);
  void OnNetworkError( size_t e );
  void OnNetworkSendDone(void);
  void OnNetworkLineBuffer( linebuffer_t* );  // new line available for processing

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
void Dispatcher<T>::OnNetworkConnected(void) {
}

template <typename T>
void Dispatcher<T>::OnNetworkDisconnected(void) {
}

template <typename T>
void Dispatcher<T>::OnNetworkError( size_t e ) {
}

template <typename T>
void Dispatcher<T>::OnNetworkSendDone(void) {
}

template <typename T>
void Dispatcher<T>::OnNetworkLineBuffer( linebuffer_t* pBuffer ) {

  typename linebuffer_t::iterator iter = (*pBuffer).begin();
  typename linebuffer_t::iterator end = (*pBuffer).end();

  BOOST_ASSERT( iter != end );

  std::string str( iter, end );
  std::cout << "IQFeed::9200: '" << str << "'" << std::endl;

  switch ( *iter ) {
    case '3': // Order Add
      break;
    case '4': // Order/Level2 Update
      break;
    case '5': // Order Delete
      break;
    case '6': // Order/Level2 Summary
      break;
    case '7': // Price Level Summary
      break;
    case '8': // Price Level Update
      break;
    case '9': // Price Level Delete
      break;
    case 'S':
      if ( !m_bInitialized ) {
        m_bInitialized = true;
        // make a message keyword parser? - from the spirit contribution repository
        // TODO: for field comparisons, use spirit or the trie method
        ou::Network<Dispatcher<T> >::Send( "S,TIMESTAMPSOFF\n" );  // TODO: maybe send on S,KEYOK, check that there are no listeners to the event
        ou::Network<Dispatcher<T> >::Send( "S,SET PROTOCOL,6.2\n" );
      }
      break;
    default:
      //throw "Unknown message type in IQFeed"; // unknown message type
      std::string str( iter, end );
      std::cout << "Unknown 9200 message type: '" << str << "'" << std::endl;
      break;
  }

  inherited_t::GiveBackBuffer( pBuffer );

}


} // namespace l2
} // namesapce iqfeed
} // namespace tf
} // namespace ou
