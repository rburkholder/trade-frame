/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
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

#pragma once

#include <string>
#include <sstream>

#include <OUCommon/Debug.h>
#include <OUCommon/Network.h>
#include <OUCommon/ReusableBuffers.h>

#include "Messages.h"
#include "SymbolLookup.h"

// In the future, for auxilliary routines making use of IQFeed,
//   think about incorporating the following concept:
//     m_pPort = m_pIQFeedProvider->CheckOutLookupPort();
//     m_pIQFeedProvider->CheckInLookupPort( m_pPort );


namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed

template <typename T>
class IQFeed:
  public ou::Network<IQFeed<T> >
{
  friend ou::Network<IQFeed<T> >;
  friend T;
public:

  using inherited_t = typename ou::Network<IQFeed<T> >;
  using linebuffer_t = typename inherited_t::linebuffer_t;

  IQFeed();
  virtual ~IQFeed();

  // used for returning message buffer
  // linebuffer_t needs to be kept with msg as there are dynamic accesses from it
  void inline DynamicFeedUpdateDone( linebuffer_t* p, IQFDynamicFeedUpdateMessage* msg ) {
    this->GiveBackBuffer( p );
    m_reposDynamicFeedUpdateMessages.CheckInL( msg );
  }
  void inline DynamicFeedSummaryDone( linebuffer_t* p, IQFDynamicFeedSummaryMessage* msg ) {
    this->GiveBackBuffer( p );
    m_reposDynamicFeedSummaryMessages.CheckInL( msg );
  }
  void inline UpdateDone( linebuffer_t* p, IQFUpdateMessage* msg ) {
    this->GiveBackBuffer( p );
    m_reposUpdateMessages.CheckInL( msg );
  }
  void inline SummaryDone( linebuffer_t* p, IQFSummaryMessage* msg ) {
    this->GiveBackBuffer( p );
    m_reposSummaryMessages.CheckInL( msg );
  }
  void inline NewsDone( linebuffer_t* p, IQFNewsMessage* msg ) {
    this->GiveBackBuffer( p );
    m_reposNewsMessages.CheckInL( msg );
  }
  void inline FundamentalDone( linebuffer_t* p, IQFFundamentalMessage* msg ) {
    this->GiveBackBuffer( p );
    m_reposFundamentalMessages.CheckInL( msg );
  }
  void inline TimeDone( linebuffer_t* p, IQFTimeMessage* msg ) {
    this->GiveBackBuffer( p );
    m_reposTimeMessages.CheckInL( msg );
  }
  void inline SystemDone( linebuffer_t* p, IQFSystemMessage* msg ) {
    this->GiveBackBuffer( p );
    m_reposSystemMessages.CheckInL( msg );
  }
  void inline ErrorDone( linebuffer_t* p, IQFErrorMessage* msg ) {
    this->GiveBackBuffer( p );
    m_reposErrorMessages.CheckInL( msg );
  }

  void SetNewsOn();
  void SetNewsOff();

  using setNames_t = SymbolLookup::setNames_t;
  using fSymbol_t = SymbolLookup::fSymbol_t;
  using fDone_t = SymbolLookup::fDone_t;
  void SymbolList(
    const setNames_t& setExchangeFilter, const setNames_t& setSecurityTypeFilter,
    fSymbol_t&& fSymbol, fDone_t&& fDone
  ) {
    m_pSymbolLookup->SymbolList(
      setExchangeFilter, setSecurityTypeFilter,
      std::move( fSymbol ), std::move( fDone )
      );
  }

protected:

  enum enumNewsState {
    NEWSISON,
    NEWSISOFF
  } m_stateNews;

  // called by Network via CRTP
  void OnNetworkConnected() {

    if ( ( 0 == m_mapListedMarket.size() )
      && ( 0 == m_mapSecurityType.size() )
      && ( 0 == m_mapTradeCondition.size() )
    ) {
      // TODO: offer up connected after lookup tables retrieved?
      m_pSymbolLookup = std::make_unique<SymbolLookup>(
        m_mapListedMarket,
        m_mapSecurityType,
        m_mapTradeCondition,
        [this](){
          std::cout
            << "IQF Lookup Tables: "
            << "ListedMarkets=" << m_mapListedMarket.size()
            << ", SecurityTypes=" << m_mapSecurityType.size()
            << ", TradeConditions=" << m_mapTradeCondition.size()
            << std::endl;
          // TODO: disconnect when iqfeed is closed.
          //   leave open for use of SymbolList lookups
          //m_pSymbolLookup->Disconnect(); // will need to delay this to out-of-thread
          //m_pSymbolLookup.reset();

          if ( &IQFeed<T>::OnIQFeedConnected != &T::OnIQFeedConnected ) {
            static_cast<T*>( this )->OnIQFeedConnected();
          }
        }
      );
      m_pSymbolLookup->Connect();
    }

    //if ( &IQFeed<T>::OnIQFeedConnected != &T::OnIQFeedConnected ) {
    //  static_cast<T*>( this )->OnIQFeedConnected();
    //}
  };

  void OnNetworkDisconnected() {
    if ( &IQFeed<T>::OnIQFeedDisConnected != &T::OnIQFeedDisConnected ) {
      static_cast<T*>( this )->OnIQFeedDisConnected();
    }
  };

  void OnNetworkError( size_t e ) {
    if ( &IQFeed<T>::OnIQFeedError != &T::OnIQFeedError ) {
      static_cast<T*>( this )->OnIQFeedError(e);
    }
  };

  void OnNetworkSendDone() {
    if ( &IQFeed<T>::OnIQFeedSendDone != &T::OnIQFeedSendDone ) {
      static_cast<T*>( this )->OnIQFeedSendDone();
    }
  };

  void OnNetworkLineBuffer( linebuffer_t* );  // new line available for processing

  ESecurityType LookupSecurityType( key_t nSecurityType ) const {
    SymbolLookup::mapSecurityType_t::const_iterator iter = m_mapSecurityType.find( nSecurityType );
    assert( m_mapSecurityType.end() != iter );
    return iter->second.eSecurityType;
  }

  std::string LookupListedMarket( key_t nListedMarket ) const {
    SymbolLookup::mapListedMarket_t::const_iterator iter = m_mapListedMarket.find( nListedMarket );
    assert( m_mapListedMarket.end() != iter );
    return iter->second.sShortName;
  }

  // CRTP based dummy callbacks
  void OnIQFeedError( size_t ) {};
  void OnIQFeedConnected() {};
  void OnIQFeedDisConnected() {};
  void OnIQFeedSendDone() {};
  void OnIQFeedFundamentalMessage( linebuffer_t* pBuffer, IQFFundamentalMessage* msg) {};
  void OnIQFeedDynamicFeedSummaryMessage( linebuffer_t* pBuffer, IQFDynamicFeedSummaryMessage* msg) {};
  void OnIQFeedDynamicFeedUpdateMessage( linebuffer_t* pBuffer, IQFDynamicFeedUpdateMessage* msg) {};
  void OnIQFeedSummaryMessage( linebuffer_t* pBuffer, IQFSummaryMessage* msg) {};
  void OnIQFeedUpdateMessage( linebuffer_t* pBuffer, IQFUpdateMessage* msg) {};
  void OnIQFeedNewsMessage( linebuffer_t* pBuffer, IQFNewsMessage* msg) {};
  void OnIQFeedTimeMessage( linebuffer_t* pBuffer, IQFTimeMessage* msg) {};
  void OnIQFeedSystemMessage( linebuffer_t* pBuffer, IQFSystemMessage* msg) {};
  void OnIQFeedErrorMessage( linebuffer_t* pBuffer, IQFErrorMessage* msg) {};

private:

  typename ou::BufferRepository<IQFDynamicFeedUpdateMessage> m_reposDynamicFeedUpdateMessages;
  typename ou::BufferRepository<IQFDynamicFeedSummaryMessage> m_reposDynamicFeedSummaryMessages;
  typename ou::BufferRepository<IQFUpdateMessage> m_reposUpdateMessages;
  typename ou::BufferRepository<IQFSummaryMessage> m_reposSummaryMessages;
  typename ou::BufferRepository<IQFNewsMessage> m_reposNewsMessages;
  typename ou::BufferRepository<IQFFundamentalMessage> m_reposFundamentalMessages;
  typename ou::BufferRepository<IQFTimeMessage> m_reposTimeMessages;
  typename ou::BufferRepository<IQFSystemMessage> m_reposSystemMessages;
  typename ou::BufferRepository<IQFErrorMessage> m_reposErrorMessages;

  enum Version { v49, v61, v62 };
  Version m_version;

  std::unique_ptr<SymbolLookup> m_pSymbolLookup;

  SymbolLookup::mapListedMarket_t m_mapListedMarket;
  SymbolLookup::mapSecurityType_t m_mapSecurityType;
  SymbolLookup::mapTradeCondition_t m_mapTradeCondition;

};

template <typename T>
IQFeed<T>::IQFeed()
: ou::Network<IQFeed<T> >( "127.0.0.1", 5009 )
, m_stateNews( NEWSISOFF )
, m_version( v49 )
{}

template <typename T>
IQFeed<T>::~IQFeed() {}

template <typename T>
void IQFeed<T>::SetNewsOn() {
  if ( NEWSISOFF == m_stateNews ) {
    m_stateNews = NEWSISON;
    std::stringstream ss;
    ss << "S,NEWSON" << std::endl;
    ou::Network<IQFeed<T> >::Send( ss.str() );
  }
}

template <typename T>
void IQFeed<T>::SetNewsOff() {
  if ( NEWSISON == m_stateNews ) {
    m_stateNews = NEWSISOFF;
    std::stringstream ss;
    ss << "S,NEWSOFF" << std::endl;
    ou::Network<IQFeed<T> >::Send( ss.str() );
  }
}

template <typename T>
void IQFeed<T>::OnNetworkLineBuffer( linebuffer_t* pBuffer ) {

  typename linebuffer_t::iterator iter = (*pBuffer).begin();
  typename linebuffer_t::iterator end = (*pBuffer).end();

  assert( iter != end );

  //std::string str( iter, end );
  //std::cout << str << std::endl;

  switch ( *iter ) {
    case 'Q':
      {
        switch ( m_version ) {
          case v49: {
            IQFUpdateMessage* msg = m_reposUpdateMessages.CheckOutL();
            msg->Assign( iter, end );
            if ( &IQFeed<T>::OnIQFeedUpdateMessage != &T::OnIQFeedUpdateMessage ) {
              static_cast<T*>( this )->OnIQFeedUpdateMessage( pBuffer, msg);
            }
            else {
              UpdateDone( pBuffer, msg );
            }
            }
            break;
          case v61:
          case v62: {
            IQFDynamicFeedUpdateMessage* msg = m_reposDynamicFeedUpdateMessages.CheckOutL();
            msg->Assign( iter, end );
            if ( &IQFeed<T>::OnIQFeedDynamicFeedUpdateMessage != &T::OnIQFeedDynamicFeedUpdateMessage ) {
              static_cast<T*>( this )->OnIQFeedDynamicFeedUpdateMessage( pBuffer, msg);
            }
            else {
              DynamicFeedUpdateDone( pBuffer, msg );
            }
            }
            break;
        }
      }
      break;
    case 'P':
      {
        switch ( m_version ) {
          case v49: {
            IQFSummaryMessage* msg = m_reposSummaryMessages.CheckOutL();
            msg->Assign( iter, end );
            if ( &IQFeed<T>::OnIQFeedSummaryMessage != &T::OnIQFeedSummaryMessage ) {
              static_cast<T*>( this )->OnIQFeedSummaryMessage( pBuffer, msg);
            }
            else {
              SummaryDone( pBuffer, msg );
            }
            }
            break;
          case v61:
          case v62: {
            IQFDynamicFeedSummaryMessage* msg = m_reposDynamicFeedSummaryMessages.CheckOutL();
            msg->Assign( iter, end );
            if ( &IQFeed<T>::OnIQFeedDynamicFeedSummaryMessage != &T::OnIQFeedDynamicFeedSummaryMessage ) {
              static_cast<T*>( this )->OnIQFeedDynamicFeedSummaryMessage( pBuffer, msg);
            }
            else {
              DynamicFeedSummaryDone( pBuffer, msg );
            }
            }
            break;
        }
      }
      break;
    case 'N':
      {
        IQFNewsMessage* msg = m_reposNewsMessages.CheckOutL();
        msg->Assign( iter, end );
        if ( &IQFeed<T>::OnIQFeedNewsMessage != &T::OnIQFeedNewsMessage ) {
          static_cast<T*>( this )->OnIQFeedNewsMessage( pBuffer, msg);
        }
        else {
          NewsDone( pBuffer, msg );
        }
      }
      break;
    case 'F':
      {
        IQFFundamentalMessage* msg = m_reposFundamentalMessages.CheckOutL();
        msg->Assign( iter, end );
        if ( &IQFeed<T>::OnIQFeedFundamentalMessage != &T::OnIQFeedFundamentalMessage ) {
          static_cast<T*>( this )->OnIQFeedFundamentalMessage( pBuffer, msg);
        }
        else {
          FundamentalDone( pBuffer, msg );
        }
      }
      break;
    case 'T':
      {
        IQFTimeMessage* msg = m_reposTimeMessages.CheckOutL();
        msg->Assign( iter, end );
        if ( &IQFeed<T>::OnIQFeedTimeMessage != &T::OnIQFeedTimeMessage ) {
          static_cast<T*>( this )->OnIQFeedTimeMessage( pBuffer, msg);
        }
        else {
          TimeDone( pBuffer, msg );
        }
      }
      break;
    case 'S':
      {
        // TODO: use SymbolLookup as a template for Spirit parsing
        IQFSystemMessage* msg = m_reposSystemMessages.CheckOutL();
        msg->Assign( iter, end );
        //std::string s( msg->Field( 2 ) );
        //std::cout << "system message: " << s << std::endl;
        // TODO: for field comparisons, use spirit or the trie method
        if ( "KEY" == msg->Field( 2 ) ) {
          std::stringstream ss;
          ss << "S,KEY," << msg->Field( 3 ) << std::endl;
          ou::Network<IQFeed<T> >::Send( ss.str() );
          ou::Network<IQFeed<T> >::Send( "S,TIMESTAMPSOFF\n" );  // TODO: maybe send on S,KEYOK, check that there are no listeners to the event
        }
        if ( "CUST" == msg->Field( 2 ) ) {
          if ( "6.1.0.20" > msg->Field( 7 ) ) {
            std::cout << "Need IQFeed version of 6.1.0.20 or greater (" << msg->Field( 7 ) << ")" << std::endl;
            //throw s;  // can't throw exception, just accept it, as we are getting '2.5.3' as a return
          }
          else {
            if ( v61 != m_version ) { // TODO: need to do better job of this when more versions added
              m_version = v61;
              ou::Network<IQFeed<T> >::Send( "S,SET PROTOCOL,6.2\n" );
              std::string sFieldRequest( "S,SELECT UPDATE FIELDS," );
              sFieldRequest += IQFDynamicFeedMessage<T>::selector;
              sFieldRequest += "\n";
              ou::Network<IQFeed<T> >::Send( sFieldRequest );
              //std::cout << "iqfeed protocol updated" << std::endl;
            }
          }
        }
        if ( "KEYOK" == msg->Field( 2 ) ) {
        }

        if ( "SERVER DISCONNECTED" == msg->Field( 2 ) ) {
          std::cout << "IQFeed status: disconnected" << std::endl;
        }
        if ( "SERVER CONNECTED" == msg->Field( 2 ) ) {
          std::cout << "IQFeed status: connected" << std::endl;
        }

        if ( &IQFeed<T>::OnIQFeedSystemMessage != &T::OnIQFeedSystemMessage ) {
          static_cast<T*>( this )->OnIQFeedSystemMessage( pBuffer, msg);
        }
        else {
          SystemDone( pBuffer, msg );
        }
      }
      break;
    case 'E':
      {
        std::string str( iter, end );
        std::cout << "IQFeed error message: '" << str << "'" << std::endl;

        IQFErrorMessage* msg = m_reposErrorMessages.CheckOutL();
        msg->Assign( iter, end );

        if ( &IQFeed<T>::OnIQFeedErrorMessage != &T::OnIQFeedErrorMessage ) {
          static_cast<T*>( this )->OnIQFeedErrorMessage( pBuffer, msg);
        }
        else {
          ErrorDone( pBuffer, msg );
        }
      }
      break;
    case 'n':
      {
        std::string str( iter, end );
        std::cout << "IQFeed symbol not found: '" << str << "'" << std::endl;

        IQFErrorMessage* msg = m_reposErrorMessages.CheckOutL();
        msg->Assign( iter, end );

        if ( &IQFeed<T>::OnIQFeedErrorMessage != &T::OnIQFeedErrorMessage ) {
          static_cast<T*>( this )->OnIQFeedErrorMessage( pBuffer, msg);
        }
        else {
          ErrorDone( pBuffer, msg );
        }
      }
      break;
    default:
      {
        std::string str( iter, end );
        std::cout << "Unknown message type in IQFeed: '" << str << "'" << std::endl;
      }
      break;
  }

}

} // namespace iqfeed
} // namespace tf
} // namespace ou
