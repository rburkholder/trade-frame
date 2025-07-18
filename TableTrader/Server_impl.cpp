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

#include <TFTrading/BuildInstrument.hpp>

#include <TFOptions/Option.h>

#include <TFIQFeed/Provider.h>
#include <TFIQFeed/OptionChainQuery.h>

#include <TFInteractiveBrokers/IBTWS.h>

#include "Server_impl.hpp"

/*
  open interest in access summary message?
    lib/TFIQFeed/HistoryQuery.h:134:    unsigned long OpenInterest;
    lib/TFIQFeed/HistoryQuery.h:134:    unsigned long OpenInterest;
    lib/TFIQFeed/Messages.h:507
    lib/TFIQFeed/Symbol.cpp:206:      summary.nOpenInterest
    lib/TFIQFeed/Symbol.h:57:    int nOpenInterest;
*/

/*
x64/debug/AppTableTrader/etc/wt_config.xml
123             <!--<idle-timeout>900</idle-timeout>-->
virtual void idleTimeout() override
https://www.webtoolkit.eu/wt/doc/reference/html/classWt_1_1WApplication.html#a4e8020fca24d09661ee4bf13400354d9
https://www.webtoolkit.eu/wt/doc/reference/html/overview.html#application_sec
*/

namespace {
  static const std::string sTWS( "tws" );
  static const std::string sIQFeed( "iqfeed" );
  static const std::string sDataBaseName( "TableTrader.db" );
}

Server_impl::Server_impl( int ib_client_id, unsigned int ib_client_port )
: m_stateEngine( EStateEngine::init )
, m_stateConnection( EStateConnection::quiescent )
, m_nOptionsNames {}
, m_nOptionsLoaded {}
, m_dblInvestment {}
, m_dblAllocated {}
, m_nPrecision {}
, m_nMultiplier {}
, m_fUpdateUnderlyingInfo {}
, m_fUpdateUnderlyingPrice {}
, m_fOptionLoadingState {}
, m_fAddExpiry {}
, m_fAddExpiryDone {}
{

  ou::tf::ProviderManager& providers( ou::tf::ProviderManager::GlobalInstance() );

  //using pProviderTWS_t = ou::tf::ib::TWS::pProvider_t;
  m_pProviderTWS = ou::tf::ib::TWS::Factory();
  m_pProviderTWS->SetName( sTWS ); // needs to match name in database
  m_pProviderTWS->SetClientId( ib_client_id );
  m_pProviderTWS->SetClientPort( ib_client_port );
  providers.Register( m_pProviderTWS );
  m_pProviderTWS->OnConnected.Add( MakeDelegate( this, &Server_impl::Connected_TWS ) );
  m_pProviderTWS->OnDisconnected.Add( MakeDelegate( this, &Server_impl::Disconnected_TWS ) );

  //using pProviderIQFeed_t = ou::tf::iqfeed::Provider::pProvider_t;
  m_pProviderIQFeed = ou::tf::iqfeed::Provider::Factory();
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

  m_stateConnection = EStateConnection::connecting;

  m_pProviderTWS->Connect();
  m_pProviderIQFeed->Connect();

}

Server_impl::~Server_impl() {
  if ( m_pWatchUnderlying ) {
    //m_pWatchUnderlying->StopWatch();
    m_pWatchUnderlying->OnFundamentals.Remove( MakeDelegate( this, &Server_impl::UnderlyingFundamentals ) );
    //m_pWatchUnderlying->OnQuote.Remove( MakeDelegate( this, &Server_impl::UnderlyingQuote ) );
    //m_pWatchUnderlying->OnTrade.Remove( MakeDelegate( this, &Server_impl::UnderlyingTrade ) );
  }

  m_stateConnection = EStateConnection::disconnecting;

  if ( m_pOptionChainQuery ) {
    m_pOptionChainQuery->Disconnect();
    m_pOptionChainQuery.reset();
  }
  m_pProviderTWS->Disconnect();
  m_pProviderTWS->OnConnected.Remove( MakeDelegate( this, &Server_impl::Connected_TWS ) );
  m_pProviderTWS->OnDisconnected.Remove( MakeDelegate( this, &Server_impl::Disconnected_TWS ) );

  m_pProviderIQFeed->Disconnect();
  m_pProviderIQFeed->OnConnected.Remove( MakeDelegate( this, &Server_impl::Connected_IQFeed ) );
  m_pProviderIQFeed->OnDisconnected.Remove( MakeDelegate( this, &Server_impl::Disconnected_IQFeed ) );

  ou::tf::ProviderManager& providers( ou::tf::ProviderManager::GlobalInstance() );
  providers.Release( m_pProviderTWS->GetName() );
  providers.Release( m_pProviderIQFeed->GetName() );
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
    m_stateConnection = EStateConnection::connected;
    m_pBuildInstrumentBoth = std::make_unique<ou::tf::BuildInstrument>( m_pProviderIQFeed, m_pProviderTWS );
    m_pBuildInstrumentIQFeed = std::make_unique<ou::tf::BuildInstrument>( m_pProviderIQFeed );
    // TODO: generate signal or status to interface
    // start the timer for the web at this point
    // ie, web needs to ensure connection prior showing the symbol selection.
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
    m_stateConnection = EStateConnection::disconnected;
  }
}

void Server_impl::SessionAttach( const std::string& sSessionId, const std::string& sClientAddress ) {
  assert( m_mapSession.end() == m_mapSession.find( sSessionId ) );
  auto pair = m_mapSession.emplace( sSessionId, Session() );
  assert( pair.second );
  pair.first->second.m_sClientAddress = sClientAddress;
}

void Server_impl::SessionDetach( const std::string& sSessionId ) {
  mapSession_t::iterator iter = m_mapSession.find( sSessionId );
  assert( m_mapSession.end() != iter );
  // TODO: perform any clean up here
  m_mapSession.erase( iter );
}

void Server_impl::UnderlyingPopulation() {
  m_stateEngine = EStateEngine::underlying_populate;
}

void Server_impl::Underlying_Updates(
  fUpdateUnderlyingInfo_t&& fUpdateUnderlyingInfo,
  fUpdateUnderlyingPrice_t&& fUpdateUnderlyingPrice
) {

  assert( fUpdateUnderlyingInfo );
  m_fUpdateUnderlyingInfo = std::move( fUpdateUnderlyingInfo );

  assert( fUpdateUnderlyingPrice );
  m_fUpdateUnderlyingPrice = std::move( fUpdateUnderlyingPrice );

}

// Underlying_Acquire ->
//  UnderlyingInitialize ->
//   UnderlyingFundamentals ->
//    InstrumentToOption

void Server_impl::Underlying_Acquire(
  const std::string& sIQFeedUnderlying
, fOptionLoadingState_t&& fOptionLoadingState
, fOptionLoadingDone_t&& fOptionLoadingDone
) {

  m_stateEngine = EStateEngine::underlying_acquire;

  assert( fOptionLoadingState );
  m_fOptionLoadingState = std::move( fOptionLoadingState );

  assert( fOptionLoadingDone );
  m_fOptionLoadingDone = std::move( fOptionLoadingDone );

  m_pBuildInstrumentBoth->Queue(
    sIQFeedUnderlying,
    [this]( pInstrument_t pInstrument, bool bConstructed ){
      UnderlyingInitialize( pInstrument );
    } );
}

void Server_impl::UnderlyingInitialize( pInstrument_t pInstrument ) {

  const ou::tf::Instrument::idInstrument_t& idInstrument( pInstrument->GetInstrumentName() );
  ou::tf::PortfolioManager& pm( ou::tf::PortfolioManager::GlobalInstance() );

  if ( pm.PortfolioExists( idInstrument ) ) {
    m_pPortfolioUnderlying = pm.GetPortfolio( idInstrument );
  }
  else {
    // TODO will need a portfolio per table instance
    m_pPortfolioUnderlying
      = pm.ConstructPortfolio(
          idInstrument, "aoTF", "USD",
          ou::tf::Portfolio::EPortfolioType::Aggregate,
          ou::tf::Currency::Name[ ou::tf::Currency::USD ] );
  }

  m_pWatchUnderlying = std::make_shared<ou::tf::Watch>( pInstrument, m_pProviderIQFeed );
  m_pWatchUnderlying->RecordSeries( false );
  //m_pWatchUnderlying->OnQuote.Add( MakeDelegate( this, &Server_impl::UnderlyingQuote ) );
  //m_pWatchUnderlying->OnTrade.Add( MakeDelegate( this, &Server_impl::UnderlyingTrade ) );

  // this causes a call to UnderlyingFundamentals(), which loads the option chains
  m_pWatchUnderlying->OnFundamentals.Add( MakeDelegate( this, &Server_impl::UnderlyingFundamentals ) );
  m_pWatchUnderlying->StartWatch();

}

namespace {
  size_t nOptionsLoadedReportingIntervalStart( 200 );
}

void Server_impl::UnderlyingFundamentals( const ou::tf::Watch::Fundamentals& fundamentals ) {

  assert( 0 < fundamentals.nContractSize );
  m_nMultiplier = fundamentals.nContractSize;
  m_nPrecision = fundamentals.nPrecision;
  m_fUpdateUnderlyingInfo( m_pWatchUnderlying->GetInstrumentName(), fundamentals.nContractSize );

  m_stateConnection = EStateConnection::fundamentals;

  using vSymbol_t = ou::tf::iqfeed::OptionChainQuery::vSymbol_t;
  using OptionList = ou::tf::iqfeed::OptionChainQuery::OptionList;

  if ( m_pOptionChainQuery ) {
    PopulateExpiry(); // TODO: may need to skip further ahead, this may not be/probably not used
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
            m_nOptionsLoadedReportingInterval = nOptionsLoadedReportingIntervalStart;
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
                  [this,&im]( pInstrument_t pInstrument, bool bConstructed ) {

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

// arrival from two different threads from UnderlyingFundamentals
void Server_impl::InstrumentToOption( pInstrument_t pInstrument ) {

  pOption_t pOption = std::make_shared<ou::tf::option::Option>( pInstrument, m_pProviderIQFeed );

  BuiltOption* pBuiltOption( nullptr );
  {
    std::scoped_lock<std::mutex> lock( m_mutexChainPopulate );
    m_nOptionsLoaded++;
    pInstrument_t pInstrument( pOption->GetInstrument() );
    mapChains_t::iterator iterChain = ou::tf::option::GetChain( m_mapChains, pInstrument );
    pBuiltOption = ou::tf::option::UpdateOption<chain_t,BuiltOption>( iterChain->second, pInstrument );
  }

  assert( pBuiltOption );
  pBuiltOption->pOption = pOption;

  if ( m_fOptionLoadingState ) {
    m_nOptionsLoadedReportingInterval--;
    if ( 0 == m_nOptionsLoadedReportingInterval ) {
      m_nOptionsLoadedReportingInterval = nOptionsLoadedReportingIntervalStart;
      m_fOptionLoadingState( m_nOptionsNames, m_nOptionsLoaded );
    }
  }

  if ( m_nOptionsNames == m_nOptionsLoaded ) {
    m_fOptionLoadingDone();
    m_fOptionLoadingState = nullptr;
    m_fOptionLoadingDone = nullptr;
  }
}

void Server_impl::ChainSelection(
  fAddExpiry_t&& fAddExpiry,
  fAddExpiryDone_t&& fAddExpiryDone
) {

  assert( fAddExpiry );
  m_fAddExpiry = std::move( fAddExpiry );

  assert( fAddExpiryDone );
  m_fAddExpiryDone = std::move( fAddExpiryDone );

  PopulateExpiry();
}

void Server_impl::TriggerUpdates( const std::string& sSessionId ) {

  mapSession_t::iterator iterSession = m_mapSession.find( sSessionId );
  assert( m_mapSession.end() != iterSession );
  Session& session( iterSession->second ); // unused for now

  double dblPortfolioUnRealized {};
  double dblPortfolioRealized {};
  double dblPortfolioCommissionsPaid {};
  double dblPortfolioTotal {};
  if ( m_pPortfolioOptions ) {
    m_pPortfolioOptions->QueryStats( dblPortfolioUnRealized, dblPortfolioRealized, dblPortfolioCommissionsPaid, dblPortfolioTotal );
  }

  if ( EStateConnection::fundamentals == m_stateConnection ) {

    if ( m_pWatchUnderlying ) {
      if ( m_fUpdateUnderlyingPrice ) m_fUpdateUnderlyingPrice( m_pWatchUnderlying->LastTrade().Price(), m_nPrecision, dblPortfolioTotal );

      for ( mapUIOption_t::value_type& vt: m_mapUIOption ) {
        UIOption& uio( vt.second );
        if ( uio.m_fRealTime ) {

          const ou::tf::Quote& quote( uio.m_pOption->LastQuote() );

          double dblPriceForAlloc {};
          switch ( uio.m_orderSide ) {
            case ou::tf::OrderSide::Buy:
              dblPriceForAlloc = quote.Ask();
              break;
            case ou::tf::OrderSide::Sell:
              dblPriceForAlloc = quote.Bid();
              break;
            default:
              assert( false );
          }
          uio.UpdateContracts( dblPriceForAlloc );

          if ( UIOption::IBContractState::unknown == uio.m_stateIBContract ) {
            if ( 0.0 < dblPriceForAlloc ) { // simple way to identify fundamentals have arrived for symbol

              uio.m_stateIBContract = UIOption::IBContractState::acquiring;

              pInstrument_t pInstrument = uio.m_pOption->GetInstrument();
              if ( 0 == pInstrument->GetContract() ) {
                const ou::tf::iqfeed::Fundamentals& fundamentals( uio.m_pOption->GetFundamentals() );

                fRequestContract_t fRequestContract =
                  [this,root=fundamentals.sExchangeRoot,pInstrument](){
                    pInstrument_t pInstrument_( pInstrument );
                    m_pProviderTWS->RequestContractDetails(
                      root,
                      pInstrument_,
                      [this]( const ou::tf::ib::TWS::ContractDetails& details, pInstrument_t& pInstrument ){
                        assert( 0 != pInstrument->GetContract() );
                        m_pProviderTWS->Sync( pInstrument );
                        // TODO: need to write to database
                      },
                      [this,pInstrument]( bool bStatus ){
                        if ( !bStatus ) {
                          const std::string& sInstrumentName( pInstrument->GetInstrumentName() );
                          BOOST_LOG_TRIVIAL(debug) << "TWS acquire contract failed: " << sInstrumentName;
                        }

                        {
                          std::scoped_lock<std::mutex> lock( m_mutexRequestContract );
                          m_fRequestContract_InProgress = nullptr;
                          if ( 0 < m_vRequestContract_Pending.size() ) {
                            m_fRequestContract_InProgress = std::move( m_vRequestContract_Pending.back() );
                            m_vRequestContract_Pending.pop_back();
                            m_fRequestContract_InProgress();
                          }
                        }
                      }
                    );
                  };

                {
                  std::scoped_lock<std::mutex> lock( m_mutexRequestContract );
                  if ( m_fRequestContract_InProgress ) { // queue the request
                    m_vRequestContract_Pending.emplace_back( std::move( fRequestContract ) );
                  }
                  else {
                    m_fRequestContract_InProgress = std::move( fRequestContract );
                    m_fRequestContract_InProgress();
                  }
                }

              };
            }
          }

          const auto& summary( uio.m_pOption->GetSummary() );  // look for open interest

          double dblPnL {};
          if ( uio.m_pPosition ) {
            double dblUnRealized;
            double dblRealized;
            double dblCommissionsPaid;
            //double dblTotal;
            uio.m_pPosition->QueryStats( dblUnRealized, dblRealized, dblCommissionsPaid, dblPnL );
          }
          uio.m_fRealTime( summary.nOpenInterest, quote.Bid(), quote.Ask(), m_nPrecision, summary.nTotalVolume, uio.m_nContracts, dblPnL );
        }
      }
    }
  }
}

void Server_impl::UnderlyingQuote( const ou::tf::Quote& quote ) {
  //m_quoteUnderlying = quote;
}

void Server_impl::UnderlyingTrade( const ou::tf::Trade& trade ) {
  //BOOST_LOG_TRIVIAL(info) << "Trade " << trade.Volume() << "@" << trade.Price();
  //m_tradeUnderlying = trade;
}

void Server_impl::PopulateExpiry() {

  m_stateEngine = EStateEngine::chains_populate;

  for ( const mapChains_t::value_type& vt: m_mapChains ) {
    m_fAddExpiry( vt.first );
  }
  m_fAddExpiryDone();
}

boost::gregorian::date Server_impl::Expiry() const { // used for state recovery
  switch ( m_stateEngine ) {
    case EStateEngine::strike_populate:
    case EStateEngine::table_populate:
    case EStateEngine::order_management_active:
      return m_citerChains->first;
      break;
    default:
      assert( false );
  }
}

void Server_impl::PopulateStrikes(
  boost::gregorian::date date,
  fPopulateStrike_t&& fPopulateStrike,
  fPopulateStrikeDone_t&& fPopulateStrikeDone
) {

  m_stateEngine = EStateEngine::strike_populate;

  m_citerChains = m_mapChains.find( date );
  assert( m_mapChains.end() != m_citerChains );

  m_citerChains->second.Strikes(
    [this,fPopulateStrike_=std::move(fPopulateStrike)](double dblStrike, const chain_t::strike_t& strike ){
      fPopulateStrike_( dblStrike, m_nPrecision );
  } );
  fPopulateStrikeDone();

  m_stateEngine = EStateEngine::table_populate;

}

const std::string& Server_impl::Ticker( double strike, ou::tf::OptionSide::EOptionSide side ) const {
  const chain_t& chain( m_citerChains->second );
  switch ( side ) {
    case ou::tf::OptionSide::Call:
      {
        double closest = chain.Call_Atm( strike );
        const chain_t::strike_t& Strike( chain.GetExistingStrike( closest ) );
        assert( Strike.call.pOption );
        return Strike.call.pOption->GetInstrumentName();
      }
      break;
    case ou::tf::OptionSide::Put:
      {
        double closest = chain.Put_Atm( strike );
        const chain_t::strike_t& Strike( chain.GetExistingStrike( closest ) );
        assert( Strike.put.pOption );
        return Strike.put.pOption->GetInstrumentName();
      }
      break;
    default:
      assert( false );
  }
}

void Server_impl::AddStrike(
  double dblStrike
, ou::tf::OptionSide::EOptionSide optionSide
, ou::tf::OrderSide::EOrderSide orderSide
, fRealTime_t&& fRealTime
, fAllocated_t&& fAllocated
, fFill_t&& fFillEntry
, fFill_t&& fFillExit
) {

  const chain_t& chain( m_citerChains->second );
  double closest = chain.Atm( dblStrike );
  const chain_t::strike_t& Strike( chain.GetExistingStrike( closest ) );

  pOption_t pOption;

  switch ( optionSide ) {
    case ou::tf::OptionSide::Call:
      assert( Strike.call.pOption );
      pOption = Strike.call.pOption;
      break;
    case ou::tf::OptionSide::Put:
      assert( Strike.put.pOption );
      pOption = Strike.put.pOption;
      break;
    default:
      assert( false );
  }

  mapUIOption_t::iterator iterUIOption = m_mapUIOption.find( closest );
  assert( m_mapUIOption.end() == iterUIOption );
  auto pair = m_mapUIOption.emplace( closest, std::move( UIOption( pOption, orderSide ) ) );
  assert( pair.second );

  UIOption& uio( pair.first->second );

  uio.m_fRealTime  = std::move( fRealTime );
  uio.m_fAllocated = std::move( fAllocated );
  uio.m_fFillEntry = std::move( fFillEntry );
  uio.m_fFillExit  = std::move( fFillExit );

}

void Server_impl::DelStrike( double dblStrike ) {

  const chain_t& chain( m_citerChains->second );
  double closest = chain.Atm( dblStrike );

  mapUIOption_t::iterator iterUIOption = m_mapUIOption.find( closest );
  assert( m_mapUIOption.end() != iterUIOption );

  m_mapUIOption.erase( iterUIOption );

  UpdateAllocations();
}

void Server_impl::SyncStrikeSelections( fSelectStrike_t&& fSelectStrike ) {
  for ( const mapUIOption_t::value_type& vt: m_mapUIOption ) {
    fSelectStrike( vt.first );
  }
}

void Server_impl::ChangeInvestment( double dblInvestment ) {

  m_dblInvestment = dblInvestment;

  UpdateAllocations();
}

void Server_impl::ChangeAllocation( double dblStrike, double dblRatio ) { // pct/100 by caller

  UIOption& uio( GetUIOption( dblStrike ) );

  m_dblAllocated -= uio.m_dblAllocated;

  uio.m_dblRatioAllocation = dblRatio;
  uio.m_dblAllocated = m_dblInvestment * dblRatio;

  m_dblAllocated += uio.m_dblAllocated;

  if ( uio.m_fAllocated ) {
    uio.m_fAllocated( m_dblAllocated, m_dblAllocated > m_dblInvestment, uio.m_dblAllocated );
  }
}

void Server_impl::UpdateAllocations() {

  m_dblAllocated = 0;

  for ( mapUIOption_t::value_type& vt: m_mapUIOption ) {
    UIOption& uio( vt.second );
    uio.m_dblAllocated = m_dblInvestment * uio.m_dblRatioAllocation;
    m_dblAllocated += uio.m_dblAllocated;

    if ( uio.m_fAllocated ) {
      uio.m_fAllocated( m_dblAllocated, m_dblAllocated > m_dblInvestment, uio.m_dblAllocated );
    }
  }
}

Server_impl::UIOption& Server_impl::GetUIOption( double dblStrike ) {
  const chain_t& chain( m_citerChains->second );
  const double closest = chain.Atm( dblStrike );
  mapUIOption_t::iterator iter = m_mapUIOption.find( closest );
  assert( m_mapUIOption.end() != iter );
  //UIOption& uio( iter->second );
  return iter->second;
}

std::string Server_impl::SetAsMarket( double dblStrike ) {
  UIOption& uio( GetUIOption( dblStrike ) );
  uio.m_eOrderType = UIOption::EOrderType::market;
  return "";
}

std::string Server_impl::SetAsLimit( double dblStrike, double dblLimit ) {
  UIOption& uio( GetUIOption( dblStrike ) );
  uio.m_eOrderType = UIOption::EOrderType::limit;
  uio.m_dblLimit = dblLimit;
  return "";
}

std::string Server_impl::SetAsScale( double dblStrike, double dblLimit, uint32_t nInitialQuan, uint32_t nIncQuan, double dblIncPrice ) {
  UIOption& uio( GetUIOption( dblStrike ) );
  std::string sMessage;
  uio.m_eOrderType = UIOption::EOrderType::scale;
  uio.m_dblLimit = dblLimit;
  uio.m_nInitialQuantity = nInitialQuan;
  uio.m_nIncrementalQuantity = nIncQuan;
  uio.m_dblIncrementalPrice = dblIncPrice;
  if ( uio.m_nContracts < ( nInitialQuan + nIncQuan ) ) {
    sMessage += "(initial_quantity + incremental_quantity > total_quantity)";
  }
  return sMessage;
}


bool Server_impl::PlaceOrders( const std::string& sPortfolioTimeStamp ) {

  m_stateEngine = EStateEngine:: order_management_active;

  ou::tf::PortfolioManager& pm( ou::tf::PortfolioManager::GlobalInstance() );

  const std::string& sUnderlying( m_pWatchUnderlying->GetInstrumentName() ); // aggregate portfolio name

  const std::string sPortfolioName
    = sUnderlying
    + "-"
    + sPortfolioTimeStamp
    ;

  if ( pm.PortfolioExists( sPortfolioName ) ) {
    m_pPortfolioOptions = pm.GetPortfolio( sPortfolioName );
  }
  else {
    m_pPortfolioOptions
      = pm.ConstructPortfolio(
          sPortfolioName, "aoTF", sUnderlying,  // TODO: pull account owner out of the db constants
          ou::tf::Portfolio::EPortfolioType::Basket,
          ou::tf::Currency::Name[ ou::tf::Currency::USD ] );
  }

  size_t nOrdersPlaced {};

  for ( mapUIOption_t::value_type& vt: m_mapUIOption ) {

    UIOption& uio( vt.second );

    if ( 0 < uio.m_nContracts ) {

      if ( 0 < uio.m_pOption->GetInstrument()->GetContract() ) {

        const std::string& sInstrumentName( uio.m_pOption->GetInstrumentName() );

        if ( pm.PositionExists( sPortfolioName, sInstrumentName ) ) {
          uio.m_pPosition = pm.GetPosition( sPortfolioName, sInstrumentName );
        }
        else {
          uio.m_pPosition = pm.ConstructPosition(
            sPortfolioName, sInstrumentName, "tt_auto",
            sTWS, sIQFeed, m_pProviderTWS, m_pProviderIQFeed,
            uio.m_pOption->GetInstrument()
          );
        }

        assert( uio.m_pPosition );
        //assert( !uio.m_pOrderEntry );

        pOrder_t pOrder;

        switch( uio.m_eOrderType ) {
          case UIOption::EOrderType::market:
            pOrder = uio.m_pPosition->ConstructOrder(
              ou::tf::OrderType::Market,
              uio.m_orderSide,
              uio.m_nContracts
            );
            pOrder->OnOrderFilled.Add( MakeDelegate( &uio, &UIOption::HandleOrderFilledEntry ) );
            break;
          case UIOption::EOrderType::limit:
            pOrder = uio.m_pPosition->ConstructOrder(
              ou::tf::OrderType::Limit,
              uio.m_orderSide,
              uio.m_nContracts,
              uio.m_dblLimit
            );
            pOrder->OnOrderFilled.Add( MakeDelegate( &uio, &UIOption::HandleOrderFilledEntry ) );
            break;
          case UIOption::EOrderType::scale:
            {
              uio.m_fScaling =
                [this,&uio](){
                  uint32_t nToOrder = uio.m_nIncrementalQuantity;
                  uint32_t nRemaining = uio.m_nContracts - uio.m_nScaleOrderQuantity;
                  if ( 0 == nRemaining ) {} // done
                  else {
                    if ( nToOrder > nRemaining ) nToOrder = nRemaining;
                    uio.m_nScaleOrderQuantity += nToOrder;

                    switch( uio.m_orderSide ) {
                      case ou::tf::OrderSide::Buy:
                        uio.m_dblScaleOrderPrice -= uio.m_dblIncrementalPrice;  // fade the price level
                        assert( 0.0 < uio.m_dblScaleOrderPrice );
                        break;
                      case ou::tf::OrderSide::Sell:
                        // this won't work properly, will need to use a stop
                        uio.m_dblScaleOrderPrice += uio.m_dblIncrementalPrice;
                        break;
                      default:
                        assert( false );
                    }

                    pOrder_t pOrder;
                    pOrder = uio.m_pPosition->ConstructOrder(
                      ou::tf::OrderType::Limit,
                      uio.m_orderSide,
                      nToOrder,
                      uio.m_dblScaleOrderPrice
                    );
                    pOrder->OnOrderFilled.Add( MakeDelegate( &uio, &UIOption::HandleOrderFilledEntryScaled ) );
                    uio.m_pPosition->PlaceOrder( pOrder );

                  }
                }; // m_fScaling

              uio.m_nScaleOrderQuantity = uio.m_nInitialQuantity;
              uio.m_dblScaleOrderPrice = uio.m_dblLimit;

              pOrder = uio.m_pPosition->ConstructOrder(
                ou::tf::OrderType::Limit,
                uio.m_orderSide,
                uio.m_nInitialQuantity,
                uio.m_dblScaleOrderPrice
              );
              pOrder->OnOrderFilled.Add( MakeDelegate( &uio, &UIOption::HandleOrderFilledEntryScaled ) );
            }
            break;
        }

        uio.m_pPosition->PlaceOrder( pOrder ); // position probably has list of orders
        nOrdersPlaced++;

      }
      else {
        assert( false );
      }
    }
  }
  return ( 0 < nOrdersPlaced );
}

void Server_impl::CancelAll() {
  for ( mapUIOption_t::value_type& vt: m_mapUIOption ) {
    UIOption& uio( vt.second );
    if ( uio.m_pPosition ) {
      uio.m_pPosition->CancelOrders();
    }
  }
}

void Server_impl::CloseAll() {
  for ( mapUIOption_t::value_type& vt: m_mapUIOption ) {
    UIOption& uio( vt.second );
    if ( uio.m_pPosition ) {

      //assert( !uio.m_pOrderExit );
      pOrder_t pOrder;

      if ( 0 < uio.m_pPosition->GetActiveSize() ) {
        ou::tf::OrderSide::EOrderSide side( ou::tf::OrderSide::Unknown );
        switch ( uio.m_orderSide ) {
          case ou::tf::OrderSide::Buy:
            side = ou::tf::OrderSide::Sell;
            break;
          case ou::tf::OrderSide::Sell:
            side = ou::tf::OrderSide::Buy;
            break;
          default:
            assert( false );
        }

        pOrder = uio.m_pPosition->ConstructOrder(
          ou::tf::OrderType::Market,
          side,
          uio.m_pPosition->GetActiveSize()
        );
        pOrder->OnOrderFilled.Add( MakeDelegate( &uio, &UIOption::HandleOrderFilledExit ) );
        uio.m_pPosition->PlaceOrder( pOrder );
      }

    }
  }

  try {
    ou::tf::PortfolioManager& pm( ou::tf::PortfolioManager::GlobalInstance() );
    m_pPortfolioOptions->SetActive( false );
    //pm.UpdatePortfolio( m_pPortfolioOptions->GetRow().idPortfolio ); // UpdatePortfolio needs work
    pm.PortfolioUpdateActive( m_pPortfolioOptions );
  }
  catch ( const std::runtime_error &e ) {
    BOOST_LOG_TRIVIAL(debug) << "Server_impl active=false did not work - " << e.what();
  }
  catch (...) {
    BOOST_LOG_TRIVIAL(debug) << "Server_impl active=false did not work";
  }

}

void Server_impl::ResetForNewUnderlying() {

  ResetForNewExpiry(); // clear detail first

  m_pWatchUnderlying = nullptr;
  m_pPortfolioUnderlying = nullptr;
  m_pPortfolioOptions = nullptr;

  m_fUpdateUnderlyingInfo = std::move( [](const std::string&,int){} );
  m_fUpdateUnderlyingPrice = std::move( [](double,int,double){});
  m_fOptionLoadingState = std::move( [](size_t,size_t){} );
  m_fOptionLoadingDone = std::move( [](){} );

  m_nOptionsNames = m_nOptionsLoaded = 0;
  m_nPrecision = m_nMultiplier = 0;
  BOOST_LOG_TRIVIAL(debug) << "ResetForNewUnderlying clear chains - begin";
  m_mapChains.clear();
  BOOST_LOG_TRIVIAL(debug) << "ResetForNewUnderlying clear chains - end";
  m_citerChains = m_mapChains.end();

}

void Server_impl::ResetForNewExpiry() {
  // TODO: will need to redraw the expiry list from which to choose
  ResetForNewTable(); // clear detail first
  // simply repopulate the ui from the chains
}

void Server_impl::ResetForNewTable() {
  BOOST_LOG_TRIVIAL(debug) << "ResetForNewTable clear chains - step 1";
  m_mapUIOption.clear();
  BOOST_LOG_TRIVIAL(debug) << "ResetForNewTable clear chains - step 2";
  UpdateAllocations();
  BOOST_LOG_TRIVIAL(debug) << "ResetForNewTable clear chains - done";
}

