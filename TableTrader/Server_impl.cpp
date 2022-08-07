/************************************************************************
 * Copyright(c) 2022, One Unified. All rights reserved.                 *
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
 * File:      Server_impl.cpp
 * Author:    raymond@burkholder.net
 * Project:   TableTrader
 * Created:   2022/08/03 17:14:41
 */

#include <boost/filesystem.hpp>
#include <boost/log/trivial.hpp>

#include <TFTrading/DBWrapper.h>
#include <TFTrading/OrderManager.h>
#include <TFTrading/PortfolioManager.h>
#include <TFTrading/ProviderManager.h>
#include <TFTrading/InstrumentManager.h>

#include <TFTrading/BuildInstrument.h>

#include <TFOptions/Option.h>

#include <TFIQFeed/Provider.h>
#include <TFIQFeed/OptionChainQuery.h>

#include <TFInteractiveBrokers/IBTWS.h>

#include "Server_impl.hpp"

namespace {
  static const std::string sTWS( "tws" );
  static const std::string sIQFeed( "iqfeed" );
  static const std::string sDataBaseName( "TableTrader.db" );
}

Server_impl::Server_impl()
: m_state( EConnection::quiescent )
{

  ou::tf::ProviderManager& providers( ou::tf::ProviderManager::GlobalInstance() );

  //using pProviderTWS_t = ou::tf::ib::TWS::pProvider_t;
  m_pProviderTWS = ou::tf::ib::TWS::Factory();
  m_pProviderTWS->SetName( sTWS ); // needs to match name in database
  providers.Register( m_pProviderTWS );
  m_pProviderTWS->OnConnected.Add( MakeDelegate( this, &Server_impl::Connected_TWS ) );
  m_pProviderTWS->OnDisconnected.Add( MakeDelegate( this, &Server_impl::Disconnected_TWS ) );

  //using pProviderIQFeed_t = ou::tf::iqfeed::IQFeedProvider::pProvider_t;
  m_pProviderIQFeed = ou::tf::iqfeed::IQFeedProvider::Factory();
  m_pProviderIQFeed->SetName( sIQFeed ); // needs to match name in database
  providers.Register( m_pProviderIQFeed );
  m_pProviderIQFeed->OnConnected.Add( MakeDelegate( this, &Server_impl::Connected_IQFeed ) );
  m_pProviderIQFeed->OnDisconnected.Add( MakeDelegate( this, &Server_impl::Disconnected_IQFeed ) );

  assert( m_pProviderTWS );
  assert( m_pProviderIQFeed );

  // for testing, start fresh each time
  //if ( boost::filesystem::exists( sDataBaseName ) ) {
  //  boost::filesystem::remove( sDataBaseName );
  //}

  m_pdb = std::make_unique<ou::tf::db>( sDataBaseName );

  m_pProviderTWS->Connect();
  m_pProviderIQFeed->Connect();

  m_state = EConnection::connecting;

}

Server_impl::~Server_impl() {
  if ( m_pWatchUnderlying ) {
    m_pWatchUnderlying->StopWatch();
    m_pWatchUnderlying->OnQuote.Remove( MakeDelegate( this, &Server_impl::UnderlyingQuote ) );
    m_pWatchUnderlying->OnTrade.Remove( MakeDelegate( this, &Server_impl::UnderlyingTrade ) );
  }
  if ( m_pOptionChainQuery ) {
    m_pOptionChainQuery->Disconnect();
    m_pOptionChainQuery.reset();
  }
  m_pProviderTWS->Disconnect();
  m_pProviderIQFeed->Disconnect();
}

void Server_impl::Connected_TWS( int n ) {
  BOOST_LOG_TRIVIAL(info) << "Provider TWS connected";
  Connected( n );
}

void Server_impl::Connected_IQFeed( int n ) {
  BOOST_LOG_TRIVIAL(info) << "Provider IQFeed connected";
  Connected( n );
}

void Server_impl::Connected( int ) {
  if ( m_pProviderTWS->Connected() && m_pProviderIQFeed->Connected() ) {
    m_state = EConnection::connected;
    m_pBuildInstrumentBoth = std::make_unique<ou::tf::BuildInstrument>( m_pProviderIQFeed, m_pProviderTWS );
    m_pBuildInstrumentIQFeed = std::make_unique<ou::tf::BuildInstrument>( m_pProviderIQFeed );
    // TODO: generate signal or status to interface
  }
}

void Server_impl::Disconnected_TWS( int n ) {
  BOOST_LOG_TRIVIAL(info) << "Provider TWS disconnected";
  Disconnected( n );
}

void Server_impl::Disconnected_IQFeed( int n ) {
  BOOST_LOG_TRIVIAL(info) << "Provider IQFeed disconnected";
  Disconnected( n );
}

void Server_impl::Disconnected( int ) {
  if ( !m_pProviderTWS->Connected() && !m_pProviderIQFeed->Connected() ) {
    m_state = EConnection::disconnected;
  }
}

void Server_impl::Start(
  const std::string& sUnderlyingFuture,
  fUpdateUnderlyingInfo_t&& fUpdateUnderlyingInfo,
  fUpdateUnderlyingPrice_t&& fUpdateUnderlyingPrice,
  fAddExpiry_t&& fAddExpiry,
  fAddExpiryDone_t&& fAddExpiryDone
) {

  assert( fUpdateUnderlyingInfo );
  m_fUpdateUnderlyingInfo = std::move( fUpdateUnderlyingInfo );

  assert( fUpdateUnderlyingPrice );
  m_fUpdateUnderlyingPrice = std::move( fUpdateUnderlyingPrice );

  assert( fAddExpiry );
  m_fAddExpiry = std::move( fAddExpiry );

  assert( fAddExpiryDone );
  m_fAddExpiryDone = std::move( fAddExpiryDone );

  m_pBuildInstrumentBoth->Queue(
    sUnderlyingFuture,
    [this]( pInstrument_t pInstrument ){
      UnderlyingInitialize( pInstrument );
    } );
}

void Server_impl::UnderlyingInitialize( pInstrument_t pInstrument ) {

  const ou::tf::Instrument::idInstrument_t& idInstrument( pInstrument->GetInstrumentName() );
  ou::tf::PortfolioManager& pm( ou::tf::PortfolioManager::GlobalInstance() );

  if ( pm.PortfolioExists( idInstrument ) ) {
    m_pPortfolio = pm.GetPortfolio( idInstrument );
  }
  else {
    // TODO will need a portfolio per table instance
    m_pPortfolio
      = pm.ConstructPortfolio(
          idInstrument, "aoTF", "USD",
          ou::tf::Portfolio::EPortfolioType::Standard,
          ou::tf::Currency::Name[ ou::tf::Currency::USD ] );
  }

  m_pWatchUnderlying = std::make_shared<ou::tf::Watch>( pInstrument, m_pProviderIQFeed );
  m_pWatchUnderlying->OnQuote.Add( MakeDelegate( this, &Server_impl::UnderlyingQuote ) );
  m_pWatchUnderlying->OnTrade.Add( MakeDelegate( this, &Server_impl::UnderlyingTrade ) );
  m_pWatchUnderlying->OnFundamentals.Add( MakeDelegate( this, &Server_impl::UnderlyingFundamentals ) );
  m_pWatchUnderlying->StartWatch();

  // don't really need underlying position at this time, just a watch
  //pPosition_t pPosition = ConstructPosition( pInstrument );
}

void Server_impl::UnderlyingFundamentals( const ou::tf::Watch::Fundamentals& fundamentals ) {

  assert( 0 < fundamentals.nContractSize );
  m_nMultiplier = fundamentals.nContractSize;
  m_nPrecision = fundamentals.nPrecision;
  m_fUpdateUnderlyingInfo( m_pWatchUnderlying->GetInstrumentName(), fundamentals.nContractSize );

  using vSymbol_t = ou::tf::iqfeed::OptionChainQuery::vSymbol_t;
  using OptionList = ou::tf::iqfeed::OptionChainQuery::OptionList;

  if ( m_pOptionChainQuery ) {
    PopulateExpiry(); // TODO: may need to skip further ahead
  }
  else {
    m_pOptionChainQuery = std::make_unique<ou::tf::iqfeed::OptionChainQuery>(
      [this](){
        m_pOptionChainQuery->QueryFuturesOptionChain(
          m_pWatchUnderlying->GetInstrument()->GetInstrumentName( ou::tf::keytypes::EProviderIQF ),
          "cp", "", "234", "1",
          [this]( const OptionList& list ){
            m_nOptionsLoaded = 0;
            m_nOptionsNames = list.vSymbol.size();
            BOOST_LOG_TRIVIAL(info) << list.sUnderlying << " has " <<  list.vSymbol.size() << " options";

            ou::tf::InstrumentManager& im( ou::tf::InstrumentManager::GlobalInstance() );
            for ( const vSymbol_t::value_type& sSymbol: list.vSymbol ) {
              pInstrument_t pInstrument;

              pInstrument = im.LoadInstrument( ou::tf::keytypes::EProviderIQF, sSymbol );
              if ( pInstrument ) { // skip the build
                InstrumentToOption( pInstrument );
              }
              else {
                m_pBuildInstrumentIQFeed->Queue(
                  sSymbol,
                  [this,&im]( pInstrument_t pInstrument ) {

                    assert( pInstrument->IsFuturesOption() );

                    if ( 0 == pInstrument->GetMultiplier() ) {
                      pInstrument->SetMultiplier( m_nMultiplier );
                    }

                    im.Register( pInstrument );

                    InstrumentToOption( pInstrument );
                  }
                );
              }
            }
          }
          );
      }
    );
    m_pOptionChainQuery->Connect(); // TODO: auto-connect instead?
  }
}

// arrival from two different threads
void Server_impl::InstrumentToOption( pInstrument_t pInstrument ) {

  pOption_t pOption = std::make_shared<ou::tf::option::Option>( pInstrument, m_pProviderIQFeed );

  BuiltOption* pBuiltOption( nullptr );
  {
    std::scoped_lock<std::mutex> lock( m_mutex );
    m_nOptionsLoaded++;
    mapChains_t::iterator iterChain = ou::tf::option::GetChain( m_mapChains, pOption );
    pBuiltOption = ou::tf::option::UpdateOption<chain_t,BuiltOption>( iterChain->second, pOption );
  }

  assert( pBuiltOption );
  pBuiltOption->pOption = pOption;

  if ( m_nOptionsNames == m_nOptionsLoaded ) {
    PopulateExpiry();
  }
}

void Server_impl::UnderlyingQuote( const ou::tf::Quote& quote ) {
  m_quoteUnderlying = quote;
}

void Server_impl::UnderlyingTrade( const ou::tf::Trade& trade ) {
  //BOOST_LOG_TRIVIAL(info) << "Trade " << trade.Volume() << "@" << trade.Price();
  const double price = m_tradeUnderlying.Price();
  if ( price != trade.Price() ) {
    //m_fUpdateUnderlyingPrice( price, m_nPrecision );
  }
  m_tradeUnderlying = trade;
}

void Server_impl::PopulateExpiry() {
  for ( const mapChains_t::value_type& vt: m_mapChains ) {
    m_fAddExpiry( vt.first );
  }
  m_fAddExpiryDone();
}

void Server_impl::PopulateStrikes(
  boost::gregorian::date date,
  fPopulateStrike_t&& fPopulateStrike,
  fPopulateStrikeDone_t&& fPopulateStrikeDone
) {

  m_citerChains = m_mapChains.find( date );
  assert( m_mapChains.end() != m_citerChains );

  m_citerChains->second.Strikes(
    [this,fPopulateStrike_=std::move(fPopulateStrike)](double dblStrike, const chain_t::strike_t& strike ){
      fPopulateStrike_( dblStrike, m_nPrecision );
  } );
  fPopulateStrikeDone();
}

const std::string& Server_impl::Ticker( ou::tf::OptionSide::EOptionSide side, double strike ) const {
  const chain_t& chain( m_citerChains->second );
  double closest {};
  switch ( side ) {
    case ou::tf::OptionSide::Call:
      {
        closest = chain.Call_Atm( strike );
        const chain_t::strike_t& Strike( chain.GetExistingStrike( closest ) );
        assert( Strike.call.pOption );
        return Strike.call.pOption->GetInstrumentName();
      }
      break;
    case ou::tf::OptionSide::Put:
      {
        closest = chain.Put_Atm( strike );
        const chain_t::strike_t& Strike( chain.GetExistingStrike( closest ) );
        assert( Strike.put.pOption );
        return Strike.put.pOption->GetInstrumentName();
      }
      break;
    default:
      assert( false );
  }
}