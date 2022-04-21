/************************************************************************
 * Copyright(c) 2012, One Unified. All rights reserved.                 *
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

// _148423314X 2017 Data Mining Algorithms in C++: Data Patterns and Algorithms for Modern Applications
//    may have some interesting thoughts on data mining inter-day and intra-day data for relationships

#include <set>
#include <algorithm>

#include <wx/menu.h>

#include <OUCommon/TimeSource.h>

#include <TFIQFeed/HistoryRequest.h>
#include <TFIQFeed/OptionChainQuery.h>

#include <TFTrading/InstrumentManager.h>

#include <TFOptions/Engine.h>

#include <TFVuTrading/TreeItem.hpp>

#include "MoneyManager.h"
#include "MasterPortfolio.h"

namespace {
  const std::string sUnderlyingPortfolioPrefix( "portfolio-" );
}

// this does not appear to be used?
//const MasterPortfolio::mapSpecs_t MasterPortfolio::m_mapSpecs = {
//        { "GLD", { 0.10, 0.20, 3, 30 } }
//      , { "SPY", { 0.10, 0.20, 3, 30 } }
//       { "QGCZ21", { 0.10, 0.20, 5, 32 } }
//        { "@ESZ21", {  0.75, 1.50, 6, 30 } }
// TODO: use trading hours, liquid trading hours for different ranges
//        { "@ESH22", {  1.00, 2.00, 6, 30 } } // NOTE: this needs to be replicated below, TODO fix the duplication requirements
//        { "@ESM22", {  3, 35 } } // TODO fix the duplication requirements - TODO: put this in config file
//    };

/*
@ES#    E-MINI S&P 500 SEPTEMBER 2021   CME     CMEMINI FUTURE

@ESM22  E-MINI S&P 500 JUNE 2022        CME     CMEMINI FUTURE
@ESU22  E-MINI S&P 500 SEPTEMBER 2022   CME     CMEMINI FUTURE
@ESZ22  E-MINI S&P 500 DECEMBER 2022    CME     CMEMINI FUTURE
@ESH23  E-MINI S&P 500 MARCH 2023       CME     CMEMINI FUTURE
@ESM23  E-MINI S&P 500 JUNE 2023        CME     CMEMINI FUTURE
@ESU23  E-MINI S&P 500 SEPTEMBER 2023   CME     CMEMINI FUTURE
@ESZ23  E-MINI S&P 500 DECEMBER 2023    CME     CMEMINI FUTURE
@ESZ24  E-MINI S&P 500 DECEMBER 2024    CME     CMEMINI FUTURE
@ESZ25  E-MINI S&P 500 DECEMBER 2025    CME     CMEMINI FUTURE
*/

MasterPortfolio::MasterPortfolio(
  boost::gregorian::date dateTrading
, ou::tf::option::SpreadSpecs spread_specs // from config file
, vSymbol_t&& vSymbol
, pPortfolio_t pMasterPortfolio
, pProvider_t pExec, pProvider_t pData1, pProvider_t pData2
, fChartRoot_t&& fChartRoot
, fSetChartDataView_t&& fSetChartDataView
)
: m_bStarted( false )
, m_nQuery {}
, m_nSharesTrading( 0 )
, m_dateTrading( dateTrading )
, m_spread_specs( spread_specs )
, m_vSymbol( std::move( vSymbol ) )
, m_fChartRoot( std::move( fChartRoot ) )
, m_fSetChartDataView( std::move( fSetChartDataView ) )
, m_pMasterPortfolio( pMasterPortfolio )
, m_pExec( pExec )
, m_pData1( pData1 )
, m_pData2( pData2 )
    //m_eAllocate( EAllocate::Waiting )
{
  assert( 0 < m_vSymbol.size() );

  assert( m_fChartRoot );
  assert( m_fSetChartDataView );

  assert( pMasterPortfolio );
  assert( pExec );
  assert( pData1 );

  switch ( pExec->ID() ) {
    case ou::tf::keytypes::EProviderIB:
      m_pIB = boost::dynamic_pointer_cast<ou::tf::ib::TWS>( pExec );  // TODO: convert to boost:: to std::
      break;
    default:
      assert( false ); // need the IB provider, or at least some provider
  }

  switch ( pData1->ID() ) {
    case ou::tf::keytypes::EProviderIQF:
      m_pIQ = boost::dynamic_pointer_cast<ou::tf::iqfeed::IQFeedProvider>( pData1 );  // TODO: convert to boost:: to std::
      break;
    default:
      assert( false ); // need the iqfeed provider
  }

  m_cePLCurrent.SetColour( ou::Colour::Fuchsia );
  m_cePLUnRealized.SetColour( ou::Colour::DarkCyan );
  m_cePLRealized.SetColour( ou::Colour::MediumSlateBlue );
  m_ceCommissionPaid.SetColour( ou::Colour::SteelBlue );

  m_cePLCurrent.SetName( "P/L Current" );
  m_cePLUnRealized.SetName( "P/L UnRealized" );
  m_cePLRealized.SetName( "P/L Realized" );
  m_ceCommissionPaid.SetName( "Commissions Paid" );

  m_pChartDataView = std::make_shared<ou::ChartDataView>();
  m_pChartDataView->Add( 0, &m_cePLCurrent );
  m_pChartDataView->Add( 0, &m_cePLUnRealized );
  m_pChartDataView->Add( 0, &m_cePLRealized );
  m_pChartDataView->Add( 2, &m_ceCommissionPaid );
  m_pChartDataView->SetNames( "Portfolio Profit / Loss", "Master P/L" );

  m_ptiTreeRoot = m_fChartRoot( "Master P/L", m_pChartDataView );
  m_ptiTreeUnderlying = m_ptiTreeRoot->AppendChild(
    "Underlying",
    [this]( ou::tf::TreeItem* ){
    },
    [this]( ou::tf::TreeItem* pti ){
      pti->NewMenu();
      pti->AppendMenuItem(
        "New Underlying",
        []( ou::tf::TreeItem* pti ){
          //std::cout << "HandleNewUnderlying: " << event.GetId() << std::endl;
        }
      );
    });
  m_ptiTreeStrategies = m_ptiTreeRoot->AppendChild(
    "Strategies",
    []( ou::tf::TreeItem* ){},
    []( ou::tf::TreeItem* ){}
    );
  //m_idTreeOptions = m_fChartAdd( m_idTreeRoot, "Options", nullptr, nullptr ); // needs to be within the associated underlying

  std::stringstream ss;
  //ss.str( "" );
  auto dt = ou::TimeSource::Instance().External();
  ss
    << ou::tf::Instrument::BuildDate( dt.date() )
    << " "
    << dt.time_of_day()
    ;
  //ss << ou::TimeSource::Instance().External();
  // will need to make this generic if need some for multiple providers.
  m_sTSDataStreamStarted = ss.str();  // will need to make this generic if need some for multiple providers.

  m_pOptionEngine = std::make_unique<ou::tf::option::Engine>( m_libor );
  m_pOptionEngine->m_fBuildWatch
    = [this](pInstrument_t pInstrument)->pWatch_t {
      // fix: need to look up and retrieve the pre-constructed watch
        //ou::tf::Watch::pWatch_t pWatch( new ou::tf::Watch( pInstrument, m_pData1 ) );
        pWatch_t pWatch;  // will cause a fault, should not need to be used -- hmm?  check this!
        return pWatch;
      };
  m_pOptionEngine->m_fBuildOption
    = [this](pInstrument_t pInstrument)->pOption_t {
      // fix: need to lookup and retrieve the pre-constructed option
        //ou::tf::option::Option::pOption_t pOption( new ou::tf::option::Option( pInstrument, m_pData1 ) );
        pOption_t pOption; // will cause a fault, should not need to be used -- hmm?  check this!
        return pOption;
      };

  m_libor.SetWatchOn( m_pIQ );

  m_pBuildInstrument = std::make_unique<ou::tf::BuildInstrument>( m_pIQ, m_pIB );

}

MasterPortfolio::~MasterPortfolio() {
  if ( m_worker.joinable() ) {
    m_worker.join();
  }
  //TODO: need to wait for m_pOptionEngine to finish
  //m_mapVolatility.clear();
  m_mapStrategyCache.clear();
  m_mapUnderlyingWithStrategies.clear();
  m_pOptionEngine.release();
  m_libor.SetWatchOff();

  if ( m_pOptionChainQuery ) {
    m_pOptionChainQuery->Disconnect();
    m_pOptionChainQuery.reset();
  }

  m_pHistoryRequest.reset(); // TODO: surface the disconnect and make synchronous

  m_pBuildInstrument.reset();
}

// auto loading portfolio from database into the map stratetgy cache
void MasterPortfolio::Add( pPortfolio_t pPortfolio ) {

  // TODO: will need to test that strategy portfolios are active??

  // will have a mixture of 'aggregate' and 'multilegged'
  mapStrategyCache_t::iterator iterCache = m_mapStrategyCache.find( pPortfolio->Id() );
  if ( m_mapStrategyCache.end() != iterCache ) {
    std::cout << "MasterPortfolio::Add already added portfolio " << pPortfolio->Id() << std::endl;
  }
  else {

    std::cout
      << "Add Portfolio (cache): "
      << "T=" << pPortfolio->GetRow().ePortfolioType
      << ",O=" << pPortfolio->GetRow().idOwner
      << ",ID=" << pPortfolio->Id()
      << std::endl;

    std::pair<mapStrategyCache_t::iterator,bool> pair
      = m_mapStrategyCache.insert( mapStrategyCache_t::value_type( pPortfolio->Id(), std::move( StrategyCache( pPortfolio ) ) ) );
    assert( pair.second );
    //m_curStrategyCache = pair.first;

    switch ( pPortfolio->GetRow().ePortfolioType ) {
      case ou::tf::Portfolio::EPortfolioType::Basket:
        // NOTE: no need to do anything with the basket, instances by underlying, created in caller
        std::cout << "MasterPortfolio Add Basket: should not arrive here" << std::endl;
        break;
      case ou::tf::Portfolio::EPortfolioType::Standard:
        // NOTE: not in use
        assert( false );
        break;
      case ou::tf::Portfolio::EPortfolioType::Aggregate: // aka underlying
        // NOTE: one aggregate per underlying, generic instrument name
        //std::cout << "MasterPortfolio Add Portfolio for Underlying (fix): " << pPortfolio->Id() << std::endl;
        break;
      case ou::tf::Portfolio::EPortfolioType::MultiLeggedPosition: // aka collar strategy
        // NOTE: this is the combo level portfolio of positions, needs to be associated with owner
        //    which allows it to be submitted to ManageStrategy - for now each is a collar portfolio
        //std::cout << "MasterPortfolio Add Portfolio for Strategy: " << pPortfolio->Id() << "," << pPortfolio->GetRow().idOwner << std::endl;
        { // example: idOwner "portfolio-SPY", idPortfolio "collar-SPY-rise-20210730-427.5-20210706-429-427"
          mapStrategyCache_t::iterator iter = m_mapStrategyCache.find( pPortfolio->GetRow().idOwner ); // look for  owner (underlying)
          assert( m_mapStrategyCache.end() != iter );
          StrategyCache& cacheOwning( iter->second );
          std::pair<mapPortfolio_iter,bool> pair2 // insert child
            = cacheOwning.m_mapPortfolio.insert( mapPortfolio_t::value_type( pPortfolio->Id(), pPortfolio ) );
          assert( pair2.second );
        }
        break;
      default: assert( false );
    } // switch
  }
}

// auto loading position from database into the mapStrategyCache (and from runtime creations?)
void MasterPortfolio::Add( pPosition_t pPosition ) {

  std::cout
    << "Add Position (cache): "
    << pPosition->GetRow().sName
    << ",quan=" << pPosition->GetActiveSize()
    << ",id=" << pPosition->GetRow().idPosition << ","
    << pPosition->Notes()
    << std::endl;

  mapStrategyCache_iter iterStrategyCache = m_mapStrategyCache.find( pPosition->IdPortfolio() );  // need to preload the iterator for random adds
  assert( m_mapStrategyCache.end() != iterStrategyCache );

  StrategyCache& cache( iterStrategyCache->second );

  if ( pPosition->GetRow().idPortfolio != cache.m_pPortfolio->Id() ) {
    // diagnostics, then fail
    std::string idInstrument( pPosition->GetInstrument()->GetInstrumentName() );
    const idPortfolio_t idPortfolio1( pPosition->IdPortfolio() );
    const idPortfolio_t idPortfolio2( cache.m_pPortfolio->Id() );
    assert( false );
  }

  std::pair<mapPosition_t::iterator,bool> pairPosition
    = cache.m_mapPosition.insert( mapPosition_t::value_type( pPosition->GetRow().sName, pPosition ) );
  assert( pairPosition.second );

  // NOTE: don't make an iqfeedname to position map as same instrument can be used
  //   in positions across portfolios (key = idPortfolio,idPosition)

}

double MasterPortfolio::UpdateChart() {

  double dblPLUnRealized {};
  double dblPLRealized {};
  double dblCommissionPaid {};
  double dblPLCurrent {};

  boost::posix_time::ptime dt( ou::TimeSource::Instance().External() );

  m_pMasterPortfolio->QueryStats( dblPLUnRealized, dblPLRealized, dblCommissionPaid, dblPLCurrent );
  //double dblCurrent = dblUnRealized + dblRealized - dblCommissionsPaid;

  m_cePLCurrent.Append( dt, dblPLCurrent );
  m_cePLUnRealized.Append( dt, dblPLUnRealized );
  m_cePLRealized.Append( dt, dblPLRealized );
  m_ceCommissionPaid.Append( dt, dblCommissionPaid );

  for ( mapUnderlyingWithStrategies_t::value_type& vt:m_mapUnderlyingWithStrategies ) {
    UnderlyingWithStrategies& uws( vt.second );
    // NOTE: this is a background thread
    if ( !uws.mapStrategyActive.empty() ) {
      uws.pUnderlying->UpdateChart( dt );
    }
  }

  return dblPLCurrent;
}

void MasterPortfolio::Test() {
  std::for_each(
    m_mapUnderlyingWithStrategies.begin(), m_mapUnderlyingWithStrategies.end(),
    [](mapUnderlyingWithStrategies_t::value_type& vt){
      //vt.second.pManageStrategy->Test();
      }
    );
}

// look for potential underlying symbols to support strategy implementation
void MasterPortfolio::Load( ptime dtLatestEod ) {

  m_dtLatestEod = dtLatestEod;

  if ( !m_mapUnderlyingWithStrategies.empty() ) {
    std::cout << "MasterPortfolio: already loaded." << std::endl;
  }
  else {

    m_setSymbols.clear();

    // ensure overnight positions are represented in the new day
    for ( const mapStrategyCache_t::value_type& vt: m_mapStrategyCache ) {
      const StrategyCache& cache( vt.second );
      if ( !cache.m_bAccessed ) {
        const std::string& idPortfolio( vt.first );
        const std::string sTemp( idPortfolio.substr( 0, sUnderlyingPortfolioPrefix.size() ) ); // some are strategy-, some are 'strangle-'
        if ( sTemp == sUnderlyingPortfolioPrefix ) {
          const std::string sUnderlying( idPortfolio.substr( sUnderlyingPortfolioPrefix.size() ) );
          std::cout << vt.first << " (" << sUnderlying << ") being examined:";

          // there are no positions in the underlying, need to drill down to the strategy
          // where are the existing strategies brought in?
          bool bPositionActive( false );
          for ( const mapPosition_t::value_type& vt: cache.m_mapPosition ) {
            bPositionActive |= ( 0 != vt.second->GetRow().nPositionActive );
          }
          if ( bPositionActive ) {
            std::cout << "cached strategy " << vt.first << " has active positions for " << sUnderlying;
            m_setSymbols.insert( sUnderlying );
          }
          // TODO: is a test required for active sub-portfolios?  ie, strategy portfolios?
          //       then will need to recurse the test?
          //    no, should run this test on the baskets as well, then bring in underlying
          //    at this point, this test will come up empty, as no positions in the underlying
          //      just sub-portfolios for the strategies
          std::cout << std::endl;
        }
      }
    }

    // TODO: need to remove this?  replaced by the statically defined map at the top of this file?

    //m_setSymbols.insert( "GLD" );
    //m_setSymbols.insert( "SPY" );
    //setSymbols.insert( "SLV" );
    //m_setSymbols.insert( "QGCZ21" );
    //m_setSymbols.insert( "@ESH22" );  // TODO: need to use m_mapSpecs
    for ( vSymbol_t::value_type& value: m_vSymbol ) {
      std::cout << "MasterPortfolio load symbol: " << value << std::endl;
      m_setSymbols.insert( value );
    }

    // QGC#    GOLD DECEMBER 2021
    // QGCZ21  GOLD DECEMBER 2021
    // add?  // USB, XLP, XBI

    // TODO: perform a watch on the name to extract expiry from fundamental message

    // TODO: check if instrument already exists prior to building a new one

    // 1) connect OptionChainQuery, 2) connect HistoryRequest, 3) start symbol processing
    m_pOptionChainQuery = std::make_unique<ou::tf::iqfeed::OptionChainQuery>(
      [this](){
        m_pHistoryRequest = ou::tf::iqfeed::HistoryRequest::Construct(
          [this](){ // fConnected_t
            ProcessSeedList();
          }
        );
        m_pHistoryRequest->Connect();
      }
    );
    m_pOptionChainQuery->Connect();

  }
}

void MasterPortfolio::ProcessSeedList() {

  if ( 0 == m_setSymbols.size() ) {
    // TODO: when m_setSymbols is empty, disconnect m_pHistoryRequest, m_pOptionChainQuery?
  }
  else {
    setSymbols_t::iterator iterSetSymbols = m_setSymbols.begin();
    const std::string sSymbol( *iterSetSymbols );
    m_setSymbols.erase( iterSetSymbols );

    m_pBuildInstrument->Queue(
      sSymbol,
      [this]( pInstrument_t pInstrument ){
        pWatch_t pWatch = std::make_shared<ou::tf::Watch>( pInstrument, m_pIQ );
        AddUnderlying( pWatch );
      } );
  }
}

void MasterPortfolio::AddUnderlying( pWatch_t pWatch ) {

  const std::string& sUnderlying( pWatch->GetInstrumentName() );

  if ( m_mapUnderlyingWithStrategies.end() != m_mapUnderlyingWithStrategies.find( sUnderlying ) ) {
    std::cout << "NOTE: underlying " << sUnderlying << " already added" << std::endl;
  }
  else {

    const ou::tf::Portfolio::idPortfolio_t idPortfolioUnderlying( sUnderlyingPortfolioPrefix + sUnderlying );

    pPortfolio_t pPortfolioUnderlying;
    mapStrategyCache_iter iterStrategyCache = m_mapStrategyCache.find( idPortfolioUnderlying );
    if ( m_mapStrategyCache.end() != iterStrategyCache ) { // use existing portfolio
      StrategyCache& cache( iterStrategyCache->second );
      pPortfolioUnderlying = cache.m_pPortfolio;
    }
    else { // create new portfolio
      ou::tf::Portfolio::idAccountOwner_t idAccountOwner( "aoRay" );  // TODO: need bring account owner from caller
      pPortfolioUnderlying
        = ou::tf::PortfolioManager::Instance().ConstructPortfolio(
            idPortfolioUnderlying, idAccountOwner, m_pMasterPortfolio->Id(), ou::tf::Portfolio::EPortfolioType::Aggregate, ou::tf::Currency::Name[ ou::tf::Currency::USD ], "Underlying Aggregate"
        );
      Add( pPortfolioUnderlying );  // update the archive
    }

    pUnderlying_t pUnderlying = std::make_unique<Underlying>( pWatch, pPortfolioUnderlying );

    auto result
      = m_mapUnderlyingWithStrategies.emplace( std::make_pair( sUnderlying, UnderlyingWithStrategies( std::move( pUnderlying ) ) ) );
    assert( result.second );
    UnderlyingWithStrategies& uws( result.first->second );

    const std::string& sIqfSymbol( pWatch->GetInstrument()->GetInstrumentName( ou::tf::keytypes::eidProvider_t::EProviderIQF ) );

    m_pHistoryRequest->Request(
      sIqfSymbol,
      200,
      [&uws]( const ou::tf::Bar& bar ) { // fBar_t
        uws.m_barsHistory.Append( bar );
      },
      [this,&uws,sUnderlying,&sIqfSymbol](){ // fDone_t

        const ou::tf::Bar& bar( uws.m_barsHistory.last() );
        std::cout
          << sUnderlying << " bar dates: "
          << "eod=" << m_dtLatestEod << ","
          << "bar=" << bar.DateTime()
          << std::endl;
        assert( m_dtLatestEod.date() <= bar.DateTime().date() ); // what condition does this test satisfy?

        uws.statistics.setPivots.CalcPivots( bar );

        const Statistics& statistics( uws.statistics );
        using PS = ou::tf::PivotSet;
        const ou::tf::PivotSet& ps( statistics.setPivots );
        uws.pUnderlying->SetPivots(
          ps.GetPivotValue( PS::R2 ), ps.GetPivotValue( PS::R1 ),
          ps.GetPivotValue( PS::PV ),
          ps.GetPivotValue( PS::S1 ), ps.GetPivotValue( PS::S2 )
          );

        m_pOptionEngine->RegisterWatch( uws.pUnderlying->GetWatch() );

        uws.pti = m_ptiTreeUnderlying->AppendChild(
          sUnderlying,
          [this,&uws,sUnderlying]( ou::tf::TreeItem* pti ){
            m_fSetChartDataView( uws.pUnderlying->GetChartDataView() );
          },
          [this,sUnderlying,sIqfSymbol]( ou::tf::TreeItem* pti ){
            pti->NewMenu();
            pti->AppendMenuItem(
              "Add Strategy",
              [this,sUnderlying]( ou::tf::TreeItem* pti ){
                std::cout << "Add Strategy for: " << sUnderlying << " (todo)" << std::endl;
              });
            pti->AppendMenuItem(
              "IQFeed Name",
              [this,sUnderlying,sIqfSymbol]( ou::tf::TreeItem* pti ){
                std::cout << "Lookup: " << sUnderlying << "=" << sIqfSymbol << std::endl;
              });
          });

        // TODO: run this in a thread, takes a while to process large option lists

        uws.pUnderlying->PopulateChains(
          [this,&uws](const std::string& sIQFeedUnderlying, ou::tf::option::fOption_t&& fOption ){
            using query_t = ou::tf::iqfeed::OptionChainQuery;
            m_pOptionChainQuery->QueryFuturesOptionChain( // TODO: need selection of equity vs futures
              sIQFeedUnderlying,
              "pc", "", "", "",
              [this,&uws,fOption_=std::move( fOption )]( const query_t::OptionList& list ){
                std::cout
                  << "chain request " << list.sUnderlying << " has "
                  //<< chains.vCall.size() << " calls, "
                  //<< chains.vPut.size() << " puts"
                  << list.vSymbol.size() << " options"
                  << std::endl;

                // TODO: will have to do this during/after chains for all underlyings are retrieved
                // TODO: provide a fDone_t function to StartStrategies ne StartUnderlying?
                m_nQuery = 1; // iniial lock of the loop, process each option, sync or async dependin gif cached
                for ( const query_t::vSymbol_t::value_type& value: list.vSymbol ) {
                  //std::cout << "MasterPortfolio::AddUnderlying option: " << value << std::endl;
                  m_nQuery++;
                  m_pBuildInstrument->Queue(
                    value,
                    [this,&uws,fOption_]( pInstrument_t pInstrument ) {
                      //std::cout << "  Option Name: " << pInstrument->GetInstrumentName() << std::endl;
                      fOption_( std::make_shared<ou::tf::option::Option>( pInstrument, m_pIQ ) );
                      auto previous = m_nQuery.fetch_sub( 1 );
                      if ( 1 == previous ) {
                        StartUnderlying( uws );
                        ProcessSeedList();  // continue processing list of underlying
                      }
                    } );
                }
                auto previous = m_nQuery.fetch_sub( 1 );
                if ( 1 == previous ) {
                  StartUnderlying( uws );
                  ProcessSeedList();  // continue processing list of underlying
                }
              });
          } );

        //m_mapVolatility.insert( mapVolatility_t::value_type( iip_.dblDailyHistoricalVolatility, sUnderlying ) );

      }
    );
  }
}

MasterPortfolio::pManageStrategy_t MasterPortfolio::ConstructStrategy( UnderlyingWithStrategies& uws ) {

  //const IIPivot& iip_( uws.iip );

  const std::string& sUnderlying( uws.pUnderlying->GetWatch()->GetInstrumentName() );

  const idPortfolio_t& idPortfolioUnderlying( uws.pUnderlying->GetPortfolio()->Id() );

  //mapSpecs_t::const_iterator iterSpreadSpecs
  //  = m_mapSpecs.find( uws.pUnderlying->GetWatch()->GetInstrument()->GetInstrumentName( ou::tf::keytypes::eidProvider_t::EProviderIQF ) );

  namespace ph = std::placeholders;

  pManageStrategy_t pManageStrategy = std::make_shared<ManageStrategy>(
        //iip.sPath, // TODO: supply to Underlying instead
        //iip_.bar,
        1.0, // TODO: defaults to rising for now, use BollingerTransitions::ReadDailyBars for directional selection
        uws.pUnderlying->GetWatch(),
        uws.pUnderlying->GetPortfolio(),
        m_dateTrading,
        m_spread_specs,
    // ManageStrategy::fGatherOptions_t
        [this]( const std::string& sIQFeedUnderlyingName, ou::tf::option::fOption_t&& fOption ){
          mapUnderlyingWithStrategies_t::const_iterator iter = m_mapUnderlyingWithStrategies.find( sIQFeedUnderlyingName );
          if ( m_mapUnderlyingWithStrategies.end() == iter ) {
            std::cout << "** can't find Underlying: " << sIQFeedUnderlyingName << std::endl;
            assert( false );
          }
          else {
            const Underlying& underlying( *iter->second.pUnderlying );
            underlying.WalkChains( std::move( fOption ) );
          }
        },
    // ManageStrategy::fConstructOption_t
        [this,idPortfolioUnderlying](const std::string& sIQFeedOptionName, ManageStrategy::fConstructedOption_t&& fOption ){
          // NOTE: once considered use of the position caches, but won't work as, at this point,
          //   idPortfolio is not known, and positions are dependent on the portfolio
          // TODO: there is an assert inside the method which will need to be remedied
          m_pBuildInstrument->Queue(
            sIQFeedOptionName,
            [this,fOption_=std::move(fOption)](pInstrument_t pInstrument ){
              pOption_t pOption( new ou::tf::option::Option( pInstrument, m_pData1 ) );
              fOption_( pOption );
              // TODO: cache the option for SaveSeries?
            } );
        },
    // ManageStrategy::fConstructPosition_t
        [this]( const idPortfolio_t& idPortfolio, pWatch_t pWatch, const std::string& sNote )->ManageStrategy::pPosition_t{

              pPosition_t pPosition;

              mapStrategyCache_iter iter = m_mapStrategyCache.find( idPortfolio );
              if ( m_mapStrategyCache.end() != iter ) {
                StrategyCache& cache( iter->second );
                mapPosition_t::iterator iterPosition = cache.m_mapPosition.find( pWatch->GetInstrumentName() );
                if ( cache.m_mapPosition.end() != iterPosition ) {
                  // NOTE: this is using a different watch than what was passed in
                  pPosition = iterPosition->second;
                  if ( pPosition->IsActive() ) {
                    std::cout << "NOTE: re-use of active position: " << pPosition->GetRow().sName << std::endl;
                  }
                }
              }

              auto& manager( ou::tf::PortfolioManager::Instance() );
              if ( !pPosition ) {
                const std::string& sInstrumentName( pWatch->GetInstrumentName() );
                pPosition = manager.ConstructPosition(
                  idPortfolio, sInstrumentName, "Basket", "ib01", "iq01", m_pExec, pWatch );
                Add( pPosition );  // update the archive
              }

              assert( pPosition );

              //if ( 0 != sNote.size() ) {
                pPosition->SetNotes( sNote );
                manager.PositionUpdateNotes( pPosition );
              //}

              // TODO: cache the watch for SaveSeries?
              return pPosition;
          },
    // ManageStrategy::fConstructPortfolio_t
          [this]( const idPortfolio_t& idPortfolioNew, const idPortfolio_t& idPortfolioOwner )->pPortfolio_t {
            pPortfolio_t pPortfolio;
            mapStrategyCache_iter iterStrategyCache = m_mapStrategyCache.find( idPortfolioNew );
            if ( m_mapStrategyCache.end() != iterStrategyCache ) {
              pPortfolio = iterStrategyCache->second.m_pPortfolio;
            }
            else {
              ou::tf::Portfolio::idAccountOwner_t idAccountOwner( "basket" ); // need to re-factor this with the other instance
              pPortfolio
                = ou::tf::PortfolioManager::Instance().ConstructPortfolio(
                    idPortfolioNew, idAccountOwner, idPortfolioOwner, ou::tf::Portfolio::EPortfolioType::MultiLeggedPosition, ou::tf::Currency::Name[ ou::tf::Currency::USD ], "Combo"
                );
              Add( pPortfolio );  // update the archive
            }
            return pPortfolio;
          },
    // ManageStrategy::fRegisterOption_t
          std::bind( &ou::tf::option::Engine::RegisterOption, m_pOptionEngine.get(), ph::_1 ),
    // ManageStrategy::fStartCalc_t
          [this]( pOption_t pOption, pWatch_t pUnderlying ){
            m_pOptionEngine->Add( pOption, pUnderlying );
          },
    // ManageStrategy::m_fStopCalc
          [this]( pOption_t pOption, pWatch_t pUnderlying ){
            m_pOptionEngine->Remove( pOption, pUnderlying );
          },
    // ManageStrategy::m_fFirstTrade
          [this](ManageStrategy& ms, const ou::tf::Trade& trade){  // assumes same thread entry
            // calculate the starting parameters
            // TODO: need to do this differently
            //mapStrategy_t::iterator iter = m_mapStrategy.find( ms.GetPortfolio()->Id() );
            //assert( m_mapStrategy.end() != iter );
            //Strategy& strategy( iter->second );
            //strategy.priceOpen = trade.Price();
            //IIPivot::Pair pair = strategy.iip.Test( trade.Price() );
            //strategy.dblBestProbability = pair.first;
            //std::cout << "FirstTrade " << m_mapPivotProbability.size() << " - " << strategy.iip.sName << ": " << (int)pair.second << ", " << pair.first << std::endl;
            //if ( IIPivot::Direction::Unknown != pair.second ) {
            //  switch ( m_eAllocate ) {
            //    case EAllocate::Waiting:
            //      //mapPivotProbability_t::iterator iterRanking =
            //        m_mapPivotProbability.insert( mapPivotProbability_t::value_type( pair.first, Ranking( strategy.iip.sName, pair.second ) ) );
            //      break;
            //    case EAllocate::Process:
            //      break;
            //    case EAllocate::Done:
            //      break;
            //  }
            //}
          },
    // ManageStrategy::m_fAuthorizeUnderlying
          [this]( pOrder_t& pOrder, pPosition_t& pPosition, pPortfolio_t& pPortfolio )->bool{
            //ou::tf::MoneyManager& mm( ou::tf::MoneyManager::GlobalInstance() );
            //bool bAuthorized = mm.Authorize( pOrder, pPosition, pPortfolio );
            //if ( bAuthorized ) {
            //  if ( !strategy.bChartActivated ) {
            //    strategy.idTree = m_fChartAdd( m_idTreeStrategies, strategy.pManageStrategy->GetPortfolio()->Id(), strategy.pChartDataView );
            //    strategy.bChartActivated = true;
            //  }
            //}
            //ou::tf::DatedDatum::volume_t volume = strategy.pManageStrategy->CalcShareCount( dblAmountToTradePerInstrument );
            //m_nSharesTrading += strategy.pManageStrategy->CalcShareCount( dblAmountToTradePerInstrument );
            //strategy.pManageStrategy->SetFundsToTrade( dblAmountToTradePerInstrument );
            //strategy.pManageStrategy->Start();
            return true;
          },
    // ManageStrategy::m_fAuthorizeOption
          [this/*,&strategy*/]( pOrder_t& pOrder, pPosition_t& pPosition, pPortfolio_t& pPortfolio, pWatch_t& pWatch )->bool{
            ou::tf::MoneyManager& mm( ou::tf::MoneyManager::GlobalInstance() );
            //bool bAuthorized = mm.Authorize( pOrder, pPosition, pPortfolio, pWatch );
            //if ( bAuthorized ) {
            //  if ( !strategy.bChartActivated ) {
            //    strategy.idTree = m_fChartAdd( m_idTreeStrategies, strategy.pManageStrategy->GetPortfolio()->Id(), strategy.pChartDataView );
            //    strategy.bChartActivated = true;
            //  }
            //}
            //ou::tf::DatedDatum::volume_t volume = strategy.pManageStrategy->CalcShareCount( dblAmountToTradePerInstrument );
            //m_nSharesTrading += strategy.pManageStrategy->CalcShareCount( dblAmountToTradePerInstrument );
            //strategy.pManageStrategy->SetFundsToTrade( dblAmountToTradePerInstrument );
            //strategy.pManageStrategy->Start();
            //return bAuthorized;
            return true;
          },
    // ManageStrategy::m_fAuthorizeSimple
          [this,sUnderlying,&uws]( const idPortfolio_t& idPortfolio, const std::string& sName, bool bExists )->bool{
            // generally used to authorize the underlying, irrespective of options in the combo
            // TODO: create authorization based upon margin requirements of the combo

            // can be called with StrategyInWaiting or pre-existing ManageStrategy

            ou::tf::MoneyManager& mm( ou::tf::MoneyManager::GlobalInstance() );
            bool bAuthorized = mm.Authorize( sName );
            if ( bAuthorized ) {
              if ( bExists ) {
                // nothing to do
              }
              else {
                // move from pStrategyInWaiting to mapStrategyActive
                iterUnderlyingWithStrategies_t iterUWS = m_mapUnderlyingWithStrategies.find( sUnderlying );
                assert( m_mapUnderlyingWithStrategies.end() != iterUWS );
                UnderlyingWithStrategies& uws( iterUWS->second );
                assert( uws.pStrategyInWaiting );

                AddAsActiveStrategy( uws, std::move( uws.pStrategyInWaiting ), idPortfolio );
              }
            }
            return bAuthorized;
          },
    // ManageStrategy::m_fBar (6 second)
          [this](ManageStrategy& ms, const ou::tf::Bar& bar){
            // calculate sentiment
            //m_sentiment.Update( bar );
          }
      );

  // TODO: will need this generic for equities and futures
  pManageStrategy->InitForUS24HourFutures( m_dateTrading );;
  return pManageStrategy;

} // ConstructStrategy

void MasterPortfolio::StartUnderlying( UnderlyingWithStrategies& uws ) {

  const std::string& sUnderlying( uws.pUnderlying->GetWatch()->GetInstrumentName() );
  const idPortfolio_t& idPortfolioUnderlying( uws.pUnderlying->GetPortfolio()->Id() );  // "portfolio-GLD"

  std::cout << "StartUnderlying " << sUnderlying << std::endl;

  bool bConstructDefaultStrategy( true );

  // look for existing strategies, which means loading from the Strategy cache
  mapStrategyCache_iter iter = m_mapStrategyCache.find( idPortfolioUnderlying );
  if ( m_mapStrategyCache.end() == iter ) {}
  else {
    // iterate the strategy portfolios, load them and get started
    StrategyCache& cacheUnderlying( iter->second );
    assert( cacheUnderlying.m_mapPosition.empty() ); // looking at list of strategies, ie, portfolio of the strategy, no positions at this level
    assert( !cacheUnderlying.m_bAccessed );
    for ( mapPortfolio_t::value_type& vt: cacheUnderlying.m_mapPortfolio ) { // cycle strategy portfolios

      // TODO: need to determine if comboPortfolio is active
      const idPortfolio_t& idPortfolioStrategy( vt.second->Id() );

      mapStrategyCache_iter iterStrategyCache = m_mapStrategyCache.find( idPortfolioStrategy );
      StrategyCache& cacheCombo( iterStrategyCache->second );
      assert( !cacheCombo.m_mapPosition.empty() );
      pManageStrategy_t pManageStrategy( ConstructStrategy( uws ) );

      bool bActivated( false );
      for ( mapPosition_t::value_type& vt: cacheCombo.m_mapPosition ) {
        if ( vt.second->IsActive() ) {
          pManageStrategy->AddPosition( vt.second );  // one or more active positions will move it
          bActivated = true;
        }
      }
      cacheUnderlying.m_bAccessed = true;
      if ( bActivated ) {
        bConstructDefaultStrategy = false;
        AddAsActiveStrategy( uws, std::make_unique<Strategy>( std::move( pManageStrategy ) ), idPortfolioStrategy );
      }
    }
  }

  if ( bConstructDefaultStrategy) { // create a new strategy by default

    assert( !uws.pStrategyInWaiting );  // need empty location

    pManageStrategy_t pManageStrategy( ConstructStrategy( uws ) );
    uws.pStrategyInWaiting = std::make_unique<Strategy>( std::move( pManageStrategy ) );
    uws.pStrategyInWaiting->pManageStrategy->Run();
  }
}

void MasterPortfolio::AddAsActiveStrategy( UnderlyingWithStrategies& uws, pStrategy_t&& pStrategy, const idPortfolio_t& idPortfolioStrategy ) {

  pChartDataView_t pChartDataView = pStrategy->pManageStrategy->GetChartDataView();

  auto result = uws.mapStrategyActive.emplace(
    std::make_pair( idPortfolioStrategy, std::move( pStrategy ) )
    );
  assert( result.second );
  Strategy& strategy( *result.first->second );

  uws.pUnderlying->PopulateChartDataView( pChartDataView ); // add price & volume

  // TODO: will need to move this to ConstructPortfolio, and then add an Authorized/Not-Authorized marker

  if ( !strategy.bChartActivated ) {

    pManageStrategy_t pManageStrategy = strategy.pManageStrategy;

    // TODO: need to duplicate menu, or turn into a shared ptr to attach to both sub-trees
    //strategy.idTreeItem = m_fChartAdd( m_idTreeStrategies, idPortfolio, pChartDataView, pMenuPopupStrategy );
    strategy.pti = m_ptiTreeStrategies->AppendChild(
      idPortfolioStrategy,
      [this,pChartDataView]( ou::tf::TreeItem* pti ){
        m_fSetChartDataView(pChartDataView );
      },
      [this,pManageStrategy,idPortfolioStrategy]( ou::tf::TreeItem* pti ){
        pti->NewMenu();
        pti->AppendMenuItem(
          "Close",
          [this,pManageStrategy, idPortfolioStrategy]( ou::tf::TreeItem* pti){
            std::cout << "Closing: " << idPortfolioStrategy << std::endl;
            pManageStrategy->ClosePositions();
          });
      } );
    strategy.bChartActivated = true;
  }

  strategy.pManageStrategy->Run();

}

void MasterPortfolio::ClosePositions( void ) {
  std::for_each(
    m_mapUnderlyingWithStrategies.begin(), m_mapUnderlyingWithStrategies.end(),
    [](mapUnderlyingWithStrategies_t::value_type& uws){
      uws.second.ClosePositions();
    } );
}

void MasterPortfolio::SaveSeries( const std::string& sPrefix ) {
  std::string sPath( sPrefix + m_sTSDataStreamStarted );
  m_libor.SaveSeries( sPath );
  std::for_each(
    m_mapUnderlyingWithStrategies.begin(), m_mapUnderlyingWithStrategies.end(),
    [&sPath](mapUnderlyingWithStrategies_t::value_type& uws){
      uws.second.SaveSeries( sPath );
    } );
  std::cout << "done." << std::endl;
}

void MasterPortfolio::EmitInfo( void ) {
  double dblNet {};
  std::for_each(
    m_mapUnderlyingWithStrategies.begin(), m_mapUnderlyingWithStrategies.end(),
    [&dblNet](mapUnderlyingWithStrategies_t::value_type& vt){
      UnderlyingWithStrategies& uws( vt.second );
      dblNet += uws.EmitInfo();
    } );
  std::cout << "Active Portfolios net: " << dblNet << std::endl;
}

void MasterPortfolio::CloseExpiryItm( boost::gregorian::date date ) {
  std::cout << "Not Implemented: MasterPortfolio::CloseExpiryItm" << std::endl;
  /*
  std::for_each(
    m_mapStrategy.begin(), m_mapStrategy.end(),
    [&date](mapStrategy_t::value_type& pair){
      Strategy& strategy( pair.second );
        strategy.pManageStrategy->CloseExpiryItm( date );
    } );
    */
}

void MasterPortfolio::CloseFarItm() {
  std::cout << "Not Implemented: MasterPortfolio::CloseFarItm" << std::endl;
  /*
  std::for_each(
    m_mapStrategy.begin(), m_mapStrategy.end(),
    [](mapStrategy_t::value_type& pair){
      Strategy& strategy( pair.second );
        strategy.pManageStrategy->CloseFarItm();
    } );
    */
}

void MasterPortfolio::CloseForProfits() {
  std::for_each(
    m_mapUnderlyingWithStrategies.begin(), m_mapUnderlyingWithStrategies.end(),
    [](mapUnderlyingWithStrategies_t::value_type& uws){
      uws.second.CloseForProfits();
    } );
}

void MasterPortfolio::CloseItmLeg() {
  std::cout << "Not Implemented: MasterPortfolio::CloseItmLeg" << std::endl;
  /*
  std::for_each(
    m_mapStrategy.begin(), m_mapStrategy.end(),
    [](mapStrategy_t::value_type& pair){
      Strategy& strategy( pair.second );
        strategy.pManageStrategy->CloseItmLeg();
    } );
    */
}

void MasterPortfolio::AddCombo( bool bForced ) {
  std::cout << "Not Implemented: MasterPortfolio::AddCombo" << std::endl;
  /*
  for ( mapStrategy_t::value_type& vt: m_mapStrategy ) {
    Strategy& strategy( vt.second );
    strategy.pManageStrategy->AddCombo( bForced );
  }
  */
}

void MasterPortfolio::TakeProfits() {
  std::for_each(
    m_mapUnderlyingWithStrategies.begin(), m_mapUnderlyingWithStrategies.end(),
    [](mapUnderlyingWithStrategies_t::value_type& uws){
      uws.second.TakeProfits();
    } );
}
