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

#include <algorithm>

#include <wx/menu.h>
#include <wx/sizer.h>
#include <wx/window.h>

#include <OUCommon/TimeSource.h>

#include <TFIQFeed/HistoryRequest.h>
#include <TFIQFeed/OptionChainQuery.h>

#include <TFTrading/InstrumentManager.h>
#include <TFTrading/ComposeInstrument.hpp>

#include <TFOptions/Engine.hpp>

#include <TFVuTrading/TreeItem.hpp>
#include <TFVuTrading/FrameControls.h>
#include <TFVuTrading/PanelFinancialChart.hpp>
#include <TFVuTrading/PanelComboOrder.hpp>

#include "MoneyManager.hpp"
#include "MasterPortfolio.hpp"

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

*/

bool MasterPortfolio::m_bFramePanelComboOrder( false );
wxPoint MasterPortfolio::m_pointFramePanelComboOrder;
wxSize  MasterPortfolio::m_sizeFramePanelComboOrder;

MasterPortfolio::MasterPortfolio(
  boost::gregorian::date dateTrading
, size_t nPeriodWidth
, size_t nStochasticPeriods
, ou::tf::option::SpreadSpecs spread_specs // from config file
, vSymbol_t&& vSymbol
, pPortfolio_t pMasterPortfolio
, pProvider_t pExec, pProvider_t pData1, pProvider_t pData2
, ou::tf::PanelFinancialChart* pPanelFinancialChart
, wxWindow* pWindowParent
)
: m_bStarted( false )
, m_nSharesTrading( 0 )
, m_dateTrading( dateTrading )
, m_nPeriodWidth( nPeriodWidth )
, m_nStochasticPeriods( nStochasticPeriods )
, m_spread_specs( spread_specs )
, m_vSymbol( std::move( vSymbol ) )
, m_pMasterPortfolio( pMasterPortfolio )
, m_pExec( std::move( pExec ) ) // IB or IQF
, m_pData1( std::move( pData1 ) )  // IQF
, m_pData2( std::move( pData2 ) )  // not used
, m_pPanelFinancialChart( pPanelFinancialChart )
, m_pWindowParent( pWindowParent )
{
  assert( 0 < m_vSymbol.size() );

  assert( pMasterPortfolio );
  assert( m_pExec );
  assert( m_pData1 );

  assert( m_pPanelFinancialChart );
  assert( m_pWindowParent );

  switch ( m_pExec->ID() ) {
    case ou::tf::keytypes::EProviderIB:
      m_pIB = ou::tf::ib::TWS::Cast( m_pExec );
      break;
    case ou::tf::keytypes::EProviderIQF:
      // m_pIQ composed with m_pData1
      break;
    default:
      assert( false ); // need one of IB or IQF
  }

  switch ( m_pData1->ID() ) {
    //case ou::tf::keytypes::EProviderIB:
    //  break;
    case ou::tf::keytypes::EProviderIQF:
      m_pIQ = ou::tf::iqfeed::Provider::Cast( m_pData1 );
      break;
    default:
      assert( false ); // need the IQF provider
  }

  m_cePLCurrent.SetColour( ou::Colour::Fuchsia );
  m_cePLUnRealized.SetColour( ou::Colour::DarkCyan );
  m_cePLRealized.SetColour( ou::Colour::MediumSlateBlue );
  m_ceCommissionPaid.SetColour( ou::Colour::SteelBlue );

  m_cePLCurrent.SetName( "P/L Current" );
  m_cePLUnRealized.SetName( "P/L UnRealized" );
  m_cePLRealized.SetName( "P/L Realized" );
  m_ceCommissionPaid.SetName( "Commissions Paid" );

  m_pcdvMasterPortfolioPL = std::make_shared<ou::ChartDataView>();
  m_pcdvMasterPortfolioPL->Add( 0, &m_cePLCurrent );
  m_pcdvMasterPortfolioPL->Add( 0, &m_cePLUnRealized );
//  m_pcdvMasterPortfolioPL->Add( 0, &m_cePLRealized );
//  m_pcdvMasterPortfolioPL->Add( 2, &m_ceCommissionPaid );
  m_pcdvMasterPortfolioPL->SetNames( "Portfolio Profit / Loss", "Master P/L" );

  m_fSetChartDataView =
    [this]( pChartDataView_t pChartDataView ) {
      m_pPanelFinancialChart->SetChartDataView( pChartDataView );
    };

  m_pPanelFinancialChart->Bind( wxEVT_DESTROY, &MasterPortfolio::OnDestroy_PanelFinancialChart, this );

  m_ptiTreeRoot = m_pPanelFinancialChart->SetRoot( "Master P/L", m_pcdvMasterPortfolioPL );
  m_ptiTreeRoot->NewMenu();
  m_ptiTreeRoot->AppendMenuItem(
    "New Underlying",
    []( ou::tf::TreeItem* pti ){
      std::cout << "HandleNewUnderlying: not implemented" << std::endl;
    }
  );

  m_ptiTreeUnderlying = m_ptiTreeRoot->AppendChild(
    "Underlying",
    [this]( ou::tf::TreeItem* ){ // fOnClick_t
      m_pPanelFinancialChart->SetChartDataView( nullptr );
    },
    [this]( ou::tf::TreeItem* ){} // fOnMenu_t
    );
  m_ptiTreeStrategies = m_ptiTreeRoot->AppendChild(
    "Strategies",
    [this]( ou::tf::TreeItem* ){ // fOnClick_t
      m_pPanelFinancialChart->SetChartDataView( nullptr );
    },
    []( ou::tf::TreeItem* pti ){ // fOnMenu_t
    }
    );

  std::stringstream ss;
  //ss.str( "" );
  auto dt = ou::TimeSource::GlobalInstance().External();
  ss
    << ou::tf::Instrument::BuildDate( dt.date() )
    << "-"
    << dt.time_of_day()
    ;
  // will need to make this generic if need some for multiple providers.
  m_sTSDataStreamStarted = ss.str();  // will need to make this generic if need some for multiple providers.

  assert( m_pIQ );

  m_fedrate.SetWatchOn( m_pIQ );
  m_pOptionEngine = std::make_unique<ou::tf::option::Engine>( m_fedrate );

  if ( m_pIB ) {
    m_pBuildInstrument = std::make_unique<ou::tf::BuildInstrument>( m_pIQ, m_pIB );
  }
  else {
    m_pBuildInstrument = std::make_unique<ou::tf::BuildInstrument>( m_pIQ );
  }

}

MasterPortfolio::~MasterPortfolio() {

  if ( m_worker.joinable() ) {
    m_worker.join();
  }

  assert( nullptr == m_ptiTreeRoot );
  assert( nullptr == m_ptiTreeUnderlying );
  assert( nullptr == m_ptiTreeStrategies );

  //TODO: need to wait for m_pOptionEngine to finish
  //m_mapVolatility.clear();
  m_mapStrategyCache.clear();
  m_mapUnderlyingWithStrategies.clear();
  m_pOptionEngine.reset();
  m_fedrate.SetWatchOff();

  m_pBuildInstrument.reset();
  m_pComposeInstrument.reset();

  m_pHistoryRequest.reset(); // TODO: surface the disconnect and make synchronous

  if ( m_pOptionChainQuery ) {
    m_pOptionChainQuery->Disconnect();
    m_pOptionChainQuery.reset();
  }

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

double MasterPortfolio::UpdateChart() { // called for GUI refresh

  double dblPLUnRealized {};
  double dblPLRealized {};
  double dblCommissionPaid {};
  double dblPLCurrent {};

  boost::posix_time::ptime dt( ou::TimeSource::GlobalInstance().External() );

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

  if ( !m_pMasterPortfolio ) {
    std::cout << "no master portfolio loaded" << std::endl;
    return;
  }

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
            assert( m_pIQ );
            switch ( m_pExec->ID() ) {
              case ou::tf::keytypes::eidProvider_t::EProviderIB:
                m_pComposeInstrument = std::make_shared<ou::tf::ComposeInstrument>(
                  m_pIQ, m_pIB,
                  [this](){
                    ProcessSeedList();
                  }
                );
                break;
              case ou::tf::keytypes::eidProvider_t::EProviderIQF:
                m_pComposeInstrument = std::make_shared<ou::tf::ComposeInstrument>(
                  m_pIQ,
                  [this](){
                    ProcessSeedList();
                  }
                );
                break;
              default:
                assert( false );
            }
          }
        );
        m_pHistoryRequest->Connect();
      }
    );
    m_pOptionChainQuery->Connect();

  }
}

void MasterPortfolio::ProcessSeedList() {
  // process one name per invocation here

  if ( 0 == m_setSymbols.size() ) {
    // TODO: when m_setSymbols is empty, disconnect m_pHistoryRequest, m_pOptionChainQuery?
  }
  else {
    setSymbols_t::iterator iterSetSymbols = m_setSymbols.begin();
    const std::string sSymbol( *iterSetSymbols );
    m_setSymbols.erase( iterSetSymbols );

    m_pComposeInstrument->Compose(
      sSymbol,
      [this]( pInstrument_t pInstrument, bool bConstructed ){
        assert( pInstrument );
        if ( bConstructed ) {
          ou::tf::InstrumentManager& im( ou::tf::InstrumentManager::GlobalInstance() );
          im.Register( pInstrument );  // is a CallAfter required, or can this run in a thread?
        }
        pWatch_t pWatch = std::make_shared<ou::tf::Watch>( pInstrument, m_pIQ );
        AddUnderlying( pWatch );
      }
    );
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
        = ou::tf::PortfolioManager::GlobalInstance().ConstructPortfolio(
            idPortfolioUnderlying, idAccountOwner, m_pMasterPortfolio->Id(), ou::tf::Portfolio::EPortfolioType::Aggregate, ou::tf::Currency::Name[ ou::tf::Currency::USD ], "Underlying Aggregate"
        );
      Add( pPortfolioUnderlying );  // update the archive
    }

    pUnderlying_t pUnderlying = std::make_unique<Underlying>( pWatch, pPortfolioUnderlying, m_nPeriodWidth, m_nStochasticPeriods );

    auto result
      = m_mapUnderlyingWithStrategies.emplace(
          std::make_pair( sUnderlying, UnderlyingWithStrategies( std::move( pUnderlying ) ) ) );
    assert( result.second );
    UnderlyingWithStrategies& uws( result.first->second );

    namespace ph = std::placeholders;

    uws.m_pOptionRegistry = std::make_shared<OptionRegistry>(
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
    // ManageStrategy::m_fSetChartDataView
          [this]( pChartDataView_t p ){
            m_fSetChartDataView( p );
          }
    );

    uws.InitiOptionRegistry();

    const std::string& sIqfSymbol( pWatch->GetInstrumentName( ou::tf::keytypes::eidProvider_t::EProviderIQF ) );

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

        m_pOptionEngine->RegisterUnderlying( uws.pUnderlying->GetWatch() );

        uws.ptiUnderlying = m_ptiTreeUnderlying->AppendChild(
          sUnderlying,
          [this,&uws,sUnderlying]( ou::tf::TreeItem* pti ){ // fOnClick_t
            m_fSetChartDataView( uws.pUnderlying->GetChartDataView() );
          },
          [this,sUnderlying,sIqfSymbol,&uws]( ou::tf::TreeItem* pti ){ // fOnBuildPopUp_t
            pti->NewMenu();
            pti->AppendMenuItem(
              "Add Call Collar (flex)",
              [this,sUnderlying,&uws]( ou::tf::TreeItem* pti ){
                std::cout << "Add Call Collar (flex) for: " << sUnderlying << std::endl;
                ConstructDefaultStrategy( uws, ou::tf::option::LegNote::Algo::Collar, ou::tf::option::ComboTraits::EMarketDirection::Rising );
              });
            pti->AppendMenuItem(
              "Add Put Collar (flex)",
              [this,sUnderlying,&uws]( ou::tf::TreeItem* pti ){
                std::cout << "Add Put Collar (flex) for: " << sUnderlying << std::endl;
                ConstructDefaultStrategy( uws, ou::tf::option::LegNote::Algo::Collar, ou::tf::option::ComboTraits::EMarketDirection::Falling );
              });
            pti->AppendMenuItem(
              "Add Bullish Credit Spread",
              [this,sUnderlying,&uws]( ou::tf::TreeItem* pti ){
                std::cout << "Add Bullish Credit Spread for: " << sUnderlying << std::endl;
                ConstructDefaultStrategy( uws, ou::tf::option::LegNote::Algo::BullPut, ou::tf::option::ComboTraits::EMarketDirection::Rising );
              });
            pti->AppendMenuItem(
              "Add Bearish Credit Spread",
              [this,sUnderlying,&uws]( ou::tf::TreeItem* pti ){
                std::cout << "Add Bearish Credit Spread for: " << sUnderlying << std::endl;
                ConstructDefaultStrategy( uws, ou::tf::option::LegNote::Algo::BearCall, ou::tf::option::ComboTraits::EMarketDirection::Falling );
              });
            pti->AppendMenuItem(
              "Add Bullish Protected Synthetic ",
              [this,sUnderlying,&uws]( ou::tf::TreeItem* pti ){
                std::cout << "Add Bullish Protected Synthetic for: " << sUnderlying << std::endl;
                ConstructDefaultStrategy( uws, ou::tf::option::LegNote::Algo::RiskReversal, ou::tf::option::ComboTraits::EMarketDirection::Rising );
              });
            pti->AppendMenuItem(
              "Add Bearish Protected Synthetic ",
              [this,sUnderlying,&uws]( ou::tf::TreeItem* pti ){
                std::cout << "Add Bearish Protected Synthetic for: " << sUnderlying << std::endl;
                ConstructDefaultStrategy( uws, ou::tf::option::LegNote::Algo::RiskConversion, ou::tf::option::ComboTraits::EMarketDirection::Falling );
              });
            pti->AppendMenuItem(
              "Add Call Back Spread",
              [this,sUnderlying,&uws]( ou::tf::TreeItem* pti ){
                std::cout << "Add Call Back Spread for: " << sUnderlying << std::endl;
                ConstructDefaultStrategy( uws, ou::tf::option::LegNote::Algo::CallBackSpread, ou::tf::option::ComboTraits::EMarketDirection::Rising );
              });
            pti->AppendMenuItem(
              "Add Put Back Spread",
              [this,sUnderlying,&uws]( ou::tf::TreeItem* pti ){
                std::cout << "Add Put Back Spread for: " << sUnderlying << std::endl;
                ConstructDefaultStrategy( uws, ou::tf::option::LegNote::Algo::PutBackSpread, ou::tf::option::ComboTraits::EMarketDirection::Falling );
              });
            pti->AppendMenuItem(
              "Add Call Calendar Spread",
              [this,sUnderlying,&uws]( ou::tf::TreeItem* pti ){
                std::cout << "Add Call Calendar Spread for: " << sUnderlying << std::endl;
                ConstructDefaultStrategy( uws, ou::tf::option::LegNote::Algo::CalendarCall, ou::tf::option::ComboTraits::EMarketDirection::Rising );
              });
            pti->AppendMenuItem(
              "Add Put Calendar Spread",
              [this,sUnderlying,&uws]( ou::tf::TreeItem* pti ){
                std::cout << "Add Put Calendar Spread for: " << sUnderlying << std::endl;
                ConstructDefaultStrategy( uws, ou::tf::option::LegNote::Algo::CalendarPut, ou::tf::option::ComboTraits::EMarketDirection::Falling );
              });
            pti->AppendMenuItem(
              "IQFeed Name",
              [this,sUnderlying,sIqfSymbol]( ou::tf::TreeItem* pti ){
                std::cout << "Lookup: " << sUnderlying << "=" << sIqfSymbol << std::endl;
              });
          });

        // until iqfeed supplies the multiplier/contract_size in the fundamental record
        uint32_t multiplier( uws.pUnderlying->GetWatch()->GetInstrument()->GetMultiplier() );

        // TODO: run this in a thread, takes a while to process large option lists
        uws.m_nQuery = 1; // initial lock of the loop, process each option, sync or async depending if cached
        uws.pUnderlying->PopulateChains(
          [this,&uws,multiplier](pInstrument_t pInstrument, ou::tf::option::fOption_t&& fOption ){ // fGatherOptions_t
            using query_t = ou::tf::iqfeed::OptionChainQuery;

            auto f =
              [this,&uws,multiplier,fOption_=std::move( fOption )]( const query_t::OptionList& list ){ // fOptionList_t
                std::cout
                  << "chain request " << list.sUnderlying << " has "
                  //<< chains.vCall.size() << " calls, "
                  //<< chains.vPut.size() << " puts"
                  << list.vSymbol.size() << " options"
                  << std::endl;

                // TODO: will have to do this during/after chains for all underlyings are retrieved
                for ( const query_t::vSymbol_t::value_type& value: list.vSymbol ) {
                  //std::cout << "MasterPortfolio::AddUnderlying option: " << value << std::endl;
                  uws.m_nQuery++;
                  m_pBuildInstrument->Queue(
                    value,
                    [this,&uws,multiplier,fOption_]( pInstrument_t pInstrument, bool bConstructed ) {
                      if ( pInstrument ) {
                        if ( bConstructed ) {
                          pInstrument->SetMultiplier( multiplier );
                          ou::tf::InstrumentManager& im( ou::tf::InstrumentManager::GlobalInstance() );
                          im.Register( pInstrument );  // is a CallAfter required, or can this run in a thread?
                        }
                        //std::cout << "  Option Name: " << pInstrument->GetInstrumentName() << std::endl;
                        pOption_t pOption = std::make_shared<ou::tf::option::Option>( pInstrument, m_pIQ );
                        fOption_( pOption );
                      }
                      auto previous = uws.m_nQuery.fetch_sub( 1 );
                      if ( 1 == previous ) {
                        StartUnderlying( uws );
                        ProcessSeedList();  // continue processing list of underlying
                      }
                    } );
                }
                auto previous = uws.m_nQuery.fetch_sub( 1 );
                if ( 1 == previous ) {
                  StartUnderlying( uws );
                  ProcessSeedList();  // continue processing list of underlying
                }
              };

            const std::string& sIQFeedUnderlying( pInstrument->GetInstrumentName( ou::tf::Instrument::eidProvider_t::EProviderIQF ) );
            switch ( uws.pUnderlying->GetWatch()->GetInstrument()->GetInstrumentType() ) {
              case ou::tf::InstrumentType::Future:
                m_pOptionChainQuery->QueryFuturesOptionChain(
                  sIQFeedUnderlying,
                  "pc", "", "", "",
                  std::move( f )
                );
                break;
              case ou::tf::InstrumentType::Stock:
                m_pOptionChainQuery->QueryEquityOptionChain(
                  sIQFeedUnderlying,
                  "pc", "", "4", "0","0","0",
                  std::move( f )
                );
                break;
              default:
                assert( false );
                break;
            }
          }
        );

        //m_mapVolatility.insert( mapVolatility_t::value_type( iip_.dblDailyHistoricalVolatility, sUnderlying ) );
      }
    );
  }
}

MasterPortfolio::pManageStrategy_t MasterPortfolio::ConstructStrategy(
  UnderlyingWithStrategies& uws,
  ou::tf::option::LegNote::Algo algo,
  ou::tf::option::ComboTraits::EMarketDirection direction
) {

  const double dblPivot = uws.statistics.setPivots.GetPivotValue( ou::tf::PivotSet::EPivots::PV );
  assert( 0 < dblPivot );

  const std::string& sUnderlying( uws.pUnderlying->GetWatch()->GetInstrumentName() );
  const idPortfolio_t& idPortfolioUnderlying( uws.pUnderlying->GetPortfolio()->Id() );

  //mapSpecs_t::const_iterator iterSpreadSpecs
  //  = m_mapSpecs.find( uws.pUnderlying->GetWatch()->GetInstrument()->GetInstrumentName( ou::tf::keytypes::eidProvider_t::EProviderIQF ) );

  // until iqfeed supplies contract_size/multiplier in futuresoptios fundamental
  //int32_t multiplier( uws.pUnderlying->GetWatch()->GetInstrument()->GetMultiplier() );

  pManageStrategy_t pManageStrategy = std::make_shared<ManageStrategy>(
        //1.0, // TODO: defaults to rising for now, use BollingerTransitions::ReadDailyBars for directional selection
        //uws.pUnderlying->SetPivots(double dblR2, double dblR1, double dblPV, double dblS1, double dblS2)
        dblPivot, // gt is long, lt is short
        direction,
        algo,
        uws.pUnderlying->GetWatch(),
        uws.pUnderlying->GetPortfolio(),
        m_spread_specs,
    // ManageStrategy::fGatherOptions_t
        [this]( pInstrument_t pInstrument, ou::tf::option::fOption_t&& fOption ){
          const std::string& sIQFeedUnderlyingName( pInstrument->GetInstrumentName( ou::tf::Instrument::eidProvider_t::EProviderIQF ) );
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
        [this](const std::string& sIQFeedOptionName, ManageStrategy::fConstructedOption_t&& fOption ){
          //ou::tf::InstrumentManager& im( ou::tf::InstrumentManager::GlobalInstance() );
          //pInstrument_t pInstrument = im.LoadInstrument( m_pIQ->ID(), sIQFeedOptionName ); // doesn't seem to be an exists by altinstrumentname, something to fix
          mapOptions_t::iterator iter = m_mapOptions.find( sIQFeedOptionName );
          if ( m_mapOptions.end() == iter ) {
            m_pBuildInstrument->Queue( // isn't currently checking for duplicates
              sIQFeedOptionName,
              [this,fOption_=std::move(fOption)](pInstrument_t pInstrument, bool bConstructed ){
                // bConstructed means that instrument was not found in database, instead was constructed
                pOption_t pOption;
                if ( bConstructed) {
                  ou::tf::InstrumentManager& im( ou::tf::InstrumentManager::GlobalInstance() );
                  im.Register( pInstrument );  // is a CallAfter required, or can this run in a thread?
                }
                pOption = std::make_shared<ou::tf::option::Option>( pInstrument, m_pIQ );
                m_mapOptions.emplace( mapOptions_t::value_type( pInstrument->GetInstrumentName( m_pIQ->ID() ), pOption ) );
                fOption_( pOption );
                // TODO: cache the option for SaveSeries?
              } );
          }
          else {
            fOption( iter->second );
          }
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

              auto& manager( ou::tf::PortfolioManager::GlobalInstance() );
              if ( !pPosition ) {
                const std::string& sInstrumentName( pWatch->GetInstrumentName() );
                pPosition = manager.ConstructPosition(
                  idPortfolio, sInstrumentName, "Basket", m_pExec->GetName(), m_pData1->GetName(), m_pExec, pWatch );
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
                = ou::tf::PortfolioManager::GlobalInstance().ConstructPortfolio(
                    idPortfolioNew, idAccountOwner, idPortfolioOwner, ou::tf::Portfolio::EPortfolioType::MultiLeggedPosition, ou::tf::Currency::Name[ ou::tf::Currency::USD ], "Combo"
                );
              Add( pPortfolio );  // update the archive
            }
            return pPortfolio;
          },
   // ManageStrategy::m_fSetChartDataView
          [this]( pChartDataView_t p ){
            m_fSetChartDataView( p );
          },
    // ManageStrategy::OptionRegistry
          uws.m_pOptionRegistry,
    // ManageStrategy::m_fInterfaceBookOptionChain
          [this,&uws]()->std::pair<wxWindow*,ou::tf::InterfaceBookOptionChain*>{

            ou::tf::FrameControls* pFrame = new ou::tf::FrameControls( m_pWindowParent, wxID_ANY, "Option Chain Orders" );
            //pFrame->SetAutoLayout( true );

            pFrame->Bind( wxEVT_MOVE, &MasterPortfolio::HandleFramePanelComboOrder_Move, this );
            pFrame->Bind( wxEVT_SIZE, &MasterPortfolio::HandleFramePanelComboOrder_Size, this );

            ou::tf::PanelComboOrder* pPanel = new ou::tf::PanelComboOrder( pFrame, wxID_ANY );
            pFrame->Attach( pPanel );

            if ( m_bFramePanelComboOrder ) {
              //pFrame->SetSize( m_sizeFramePanelComboOrder );
              //pFrame->Move( m_pointFramePanelComboOrder );
              pFrame->SetSize( m_pointFramePanelComboOrder.x, m_pointFramePanelComboOrder.y, m_sizeFramePanelComboOrder.GetWidth(), m_sizeFramePanelComboOrder.GetHeight() );
            }
            else {
              //pFrame->CentreOnParent();
              m_pointFramePanelComboOrder = pFrame->GetPosition();
              m_sizeFramePanelComboOrder = pFrame->GetSize();
              m_bFramePanelComboOrder = true;
            }

            pFrame->Layout();
            pFrame->Show( true ); // TODO: center above main

            auto dt = ou::TimeSource::GlobalInstance().Internal();

            pPanel->CallAfter(
              [this,&uws,dt,pPanel](){
                //m_pNotebookOptionChains->SetName( sUnderlying );
                uws.pUnderlying->WalkChains(
                  [this,dt,pPanel]( pOption_t pOption ){
                    if ( dt < pOption->GetInstrument()->GetExpiryUtc() ) {
                      const ou::tf::option::Option& option( *pOption );
                      pPanel->Add(
                        option.GetExpiry()
                      , option.GetStrike()
                      , option.GetOptionSide()
                      , option.GetInstrumentName( ou::tf::keytypes::eidProvider_t::EProviderIQF )
                      );
                    }
                  }
                );
              });

            return std::pair( pFrame, pPanel );

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
  const std::string& sName( uws.pUnderlying->GetWatch()->GetInstrumentName() );
  switch ( uws.pUnderlying->GetWatch()->GetInstrument()->GetInstrumentType() ) {
    case ou::tf::InstrumentType::Future:
      pManageStrategy->InitFor24HourMarkets( m_dateTrading );
      std::cout << sName << " market open " << pManageStrategy->GetMarketOpen() << std::endl;
      break;
    case ou::tf::InstrumentType::Stock:
      pManageStrategy->InitForUSEquityExchanges( m_dateTrading );
      std::cout << sName << " market open " << pManageStrategy->GetMarketOpen() << std::endl;
      break;
    default:
      assert( false );
  }

  return pManageStrategy;

} // ConstructStrategy

void MasterPortfolio::StartUnderlying( UnderlyingWithStrategies& uws ) {

  const std::string& sUnderlying( uws.pUnderlying->GetWatch()->GetInstrumentName() );
  const idPortfolio_t& idPortfolioUnderlying( uws.pUnderlying->GetPortfolio()->Id() );  // "portfolio-GLD"

  std::cout << "Start Underlying " << sUnderlying << std::endl;

  uws.pUnderlying->FilterChains();

  bool bConstructDefaultStrategy( false );  // 2024/05/26 disable building a default strategy, use manual trading for now

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

      bool bStrategyActive( false ); // test prior to construction
      for ( mapPosition_t::value_type& vt: cacheCombo.m_mapPosition ) {
        if ( vt.second->IsActive() ) {
          bStrategyActive = true;
        }
      }

      if ( bStrategyActive ) {
        bConstructDefaultStrategy = false;

        pManageStrategy_t pManageStrategy( ConstructStrategy( uws, ou::tf::option::LegNote::Algo::Existing, ou::tf::option::ComboTraits::EMarketDirection::NotApplicable ) );
        Add_ManageStrategy_ToTree( idPortfolioStrategy, pManageStrategy );

        for ( mapPosition_t::value_type& vt: cacheCombo.m_mapPosition ) {
          pPosition_t pPosition = vt.second;
          if ( pPosition->IsActive() ) {
            pInstrument_t pInstrument = pPosition->GetInstrument();
            switch ( pInstrument->GetInstrumentType() ) {
              case ou::tf::InstrumentType::Option:
              case ou::tf::InstrumentType::FuturesOption: {
                const std::string& sIQFeedName( pInstrument->GetInstrumentName( m_pIQ->ID() ) );
                mapOptions_t::iterator iter = m_mapOptions.find( sIQFeedName );
                if ( m_mapOptions.end() == iter ) {
                  pOption_t pOption = std::dynamic_pointer_cast<ou::tf::option::Option>( pPosition->GetWatch() );
                  m_mapOptions.emplace( mapOptions_t::value_type( sIQFeedName, pOption ) );
                }
                }
                break;
              default:
                break;
            }
            pManageStrategy->AddPosition( pPosition );  // one or more active positions will move it
          }
        }

        cacheUnderlying.m_bAccessed = true;
        AddAsActiveStrategy( uws, std::make_unique<Strategy>( std::move( pManageStrategy ) ), idPortfolioStrategy );
      }
    }
  }

  if ( bConstructDefaultStrategy) { // create a new strategy as required
    ConstructDefaultStrategy( uws, ou::tf::option::LegNote::Algo::Collar, ou::tf::option::ComboTraits::EMarketDirection::Select );
  }
}

void MasterPortfolio::ConstructDefaultStrategy(
  UnderlyingWithStrategies& uws,
  ou::tf::option::LegNote::Algo algo,
  ou::tf::option::ComboTraits::EMarketDirection direction
) {
  const std::string& sUnderlying( uws.pUnderlying->GetWatch()->GetInstrumentName() );
  if ( uws.pStrategyInWaiting ) {
    std::cout
      << "MasterPortfolio::ConstructDefaultStrategy has strategy-in-waiting: "
      << sUnderlying
      << std::endl;
  }
  else {
    pManageStrategy_t pManageStrategy( ConstructStrategy( uws, algo, direction ) );
    Add_ManageStrategy_ToTree( sUnderlying + "-construction" , pManageStrategy );
    uws.pStrategyInWaiting = std::make_unique<Strategy>( std::move( pManageStrategy ) );
    uws.pStrategyInWaiting->pManageStrategy->Run();
  }
}

void MasterPortfolio::Add_ManageStrategy_ToTree( const idPortfolio_t& idPortfolioStrategy, pManageStrategy_t pManageStrategy ) {

  // TODO: need to duplicate menu, or turn into a shared ptr to attach to both sub-trees
  ou::tf::TreeItem* ptiManageStrategy = m_ptiTreeStrategies->AppendChild(
    idPortfolioStrategy,
    [this,pManageStrategy]( ou::tf::TreeItem* pti ){ // OnClick
      m_fSetChartDataView( pManageStrategy->GetChartDataView() );
    },
    [this,pManageStrategy]( ou::tf::TreeItem* pti ){ // OnMenu
    } );
  pManageStrategy->SetTreeItem( ptiManageStrategy );
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
    strategy.bChartActivated = true;
    std::cout << "MasterPortfolio::AddAsActiveStrategy: " << idPortfolioStrategy << " activated" << std::endl;
  }
  else {
    std::cout << "MasterPortfolio::AddAsActiveStrategy: " << idPortfolioStrategy << " already activated" << std::endl;
  }

  strategy.pManageStrategy->Run();

}

void MasterPortfolio::ClosePositions() {
  std::for_each(
    m_mapUnderlyingWithStrategies.begin(), m_mapUnderlyingWithStrategies.end(),
    [](mapUnderlyingWithStrategies_t::value_type& uws){
      uws.second.ClosePositions();
    } );
}

void MasterPortfolio::SaveSeries( const std::string& sPrefix ) {
  std::string sPath( sPrefix + m_sTSDataStreamStarted );
  m_fedrate.SaveSeries( sPath );
  std::for_each(
    m_mapUnderlyingWithStrategies.begin(), m_mapUnderlyingWithStrategies.end(),
    [&sPath](mapUnderlyingWithStrategies_t::value_type& uws){
      uws.second.SaveSeries( sPath );
    } );
  std::cout << "done." << std::endl;
}

void MasterPortfolio::EmitInfo() {
  double dblNet {};
  std::for_each(
    m_mapUnderlyingWithStrategies.begin(), m_mapUnderlyingWithStrategies.end(),
    [&dblNet](mapUnderlyingWithStrategies_t::value_type& vt){
      UnderlyingWithStrategies& uws( vt.second );
      dblNet += uws.EmitInfo();
    } );
  std::cout << "Active Portfolios net: " << dblNet << std::endl;

  double dblPLUnRealized {};
  double dblPLRealized {};
  double dblCommissionPaid {};
  double dblPLCurrent {};

  m_pMasterPortfolio->QueryStats( dblPLUnRealized, dblPLRealized, dblCommissionPaid, dblPLCurrent );
  std::cout
    << "Master Portfolio: " << ou::TimeSource::GlobalInstance().Internal()
    << ",unrealized=" << dblPLUnRealized
    << ",realized=" << dblPLRealized
    << ",commission=" << dblCommissionPaid
    << ",current=" << dblPLCurrent
    << std::endl;
}

void MasterPortfolio::EmitIV() {
  std::for_each(
    m_mapUnderlyingWithStrategies.begin(), m_mapUnderlyingWithStrategies.end(),
    [](mapUnderlyingWithStrategies_t::value_type& vt){
      UnderlyingWithStrategies& uws( vt.second );
      uws.EmitIV();
    } );
}

void MasterPortfolio::CloseForProfits() {
  std::for_each(
    m_mapUnderlyingWithStrategies.begin(), m_mapUnderlyingWithStrategies.end(),
    [](mapUnderlyingWithStrategies_t::value_type& uws){
      uws.second.CloseForProfits();
    } );
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

void MasterPortfolio::HandleFramePanelComboOrder_Move( wxMoveEvent& event ) {
  m_pointFramePanelComboOrder = event.GetPosition();
  event.Skip();
}

void MasterPortfolio::HandleFramePanelComboOrder_Size( wxSizeEvent& event ) {
  m_sizeFramePanelComboOrder = event.GetSize();
  event.Skip();
}

void MasterPortfolio::OnDestroy_PanelFinancialChart( wxWindowDestroyEvent& event ) {

  m_pPanelFinancialChart->Unbind( wxEVT_DESTROY, &MasterPortfolio::OnDestroy_PanelFinancialChart, this );

  // captured pManageStrategy needs to be cleared
  if ( m_ptiTreeStrategies ) {
    m_ptiTreeStrategies->Delete();
    m_ptiTreeStrategies = nullptr;
  }

  if ( m_ptiTreeUnderlying ) {
    m_ptiTreeUnderlying->Delete();
    m_ptiTreeUnderlying = nullptr;
  }

  if ( m_ptiTreeRoot ) {
    m_ptiTreeRoot->Delete();
    m_ptiTreeRoot = nullptr;
  }

  event.Skip( true );
}
