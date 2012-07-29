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

#include "stdafx.h"

#include <boost/foreach.hpp>
#include <boost/thread.hpp>  // separate thread background merge processing
#include <boost/bind.hpp>

#include <wx/bitmap.h>

#include <TFTrading/InstrumentManager.h>
#include <TFTrading/AccountManager.h>
#include <TFTrading/OrderManager.h>
#include <TFTrading/PortfolioManager.h>

#include <OUGP/Population.h>
#include <TFGP/NodeTimeSeries.h>

#include "OptimizeStrategy.h"

IMPLEMENT_APP(AppOptimizeStrategy)

bool AppOptimizeStrategy::OnInit( void ) {

  m_pFrameMain = new FrameMain( 0, wxID_ANY, "Strategy Optimizer" );
  wxWindowID idFrameMain = m_pFrameMain->GetId();
  //m_pFrameMain->Bind( wxEVT_SIZE, &AppStrategy1::HandleFrameMainSize, this, idFrameMain );
  //m_pFrameMain->Bind( wxEVT_MOVE, &AppStrategy1::HandleFrameMainMove, this, idFrameMain );
  //m_pFrameMain->Center();
//  m_pFrameMain->Move( -2500, 50 );
  m_pFrameMain->SetSize( 500, 600 );
  SetTopWindow( m_pFrameMain );

  wxBoxSizer* m_sizerMain;
  m_sizerMain = new wxBoxSizer(wxVERTICAL);
  m_pFrameMain->SetSizer(m_sizerMain);

  wxBoxSizer* m_sizerControls;
  m_sizerControls = new wxBoxSizer( wxHORIZONTAL );
  m_sizerMain->Add( m_sizerControls, 0, wxLEFT|wxTOP|wxRIGHT, 5 );

  wxBoxSizer* m_sizerStatus = new wxBoxSizer( wxHORIZONTAL );
  m_sizerMain->Add( m_sizerStatus, 1, wxEXPAND|wxALL, 5 );

  m_pPanelLogging = new ou::tf::PanelLogging( m_pFrameMain, wxID_ANY );
  m_sizerStatus->Add( m_pPanelLogging, 1, wxALL | wxEXPAND|wxALIGN_LEFT|wxALIGN_RIGHT|wxALIGN_TOP|wxALIGN_BOTTOM, 0);
  m_pPanelLogging->Show( true );

  m_pFrameMain->Show( true );

  boost::thread sim( boost::bind( &AppOptimizeStrategy::Optimizer, this ) );

  return 1;

}

void AppOptimizeStrategy::Optimizer( void ) {
  //m_sim->SetGroupDirectory( "/semiauto/2011-Sep-23 19:17:48.252497" );
// ->  m_pProvider->SetGroupDirectory( "/app/semiauto/2011-Nov-06 18:54:22.184889" );
  //m_sim->SetGroupDirectory( "/app/semiauto/2011-Nov-07 18:53:31.016760" );
  //m_sim->SetGroupDirectory( "/app/semiauto/2011-Nov-08 18:58:29.396624" );
//  m_sim->SetExecuteAgainst( ou::tf::CSimulateOrderExecution::EAQuotes );

  ou::tf::CInstrumentManager& mgr( ou::tf::CInstrumentManager::Instance() );
  m_pInstrument = mgr.Exists( "+GCQ12" ) ? mgr.Get( "+GCQ12" ) : mgr.ConstructFuture( "+GCQ12", "SMART", 2012, 8 );
  m_pInstrument->SetMultiplier( 100 );
  m_pInstrument->SetMinTick( 0.1 );

  // manage the genetic programming discovery process here
  ou::gp::Population pop( 100 );

  pop.RegisterDouble<StrategyEquity::NodeTypesTimeSeries_t>();

  /*
  steps:
  * create skeletal structure with various time series
  * add time series to arrays for use in randomized selection
    * strategy is provided the routines for registering the time series it is using, 
    * so needs to inherit from something which has those routines available
  * need the random selector, which is templated on time series type
    * probably also inherited by the strategy
  * add the time series nodes to the set of node types for doubles
  * strategy:
    * updates the various time series
    * calcs using gp formulas
    * executes against results
  * at end of time series, calculate 'winner'
  */

  // add optimization code so that copied individuals are not recomputed

  std::stringstream ss;
  struct PreProcessNodes {
    void operator()( ou::gp::Node& node ) {
      node.PreProcess();  // set time series on nodes of type time series
      switch ( node.NodeCount() ) {
      case 0:
        break;
      case 1:
        (*this)( node.ChildCenter() );
        break;
      case 2: 
        (*this)( node.ChildLeft() );
        (*this)( node.ChildRight() );
        break;
      }
    }
    void operator()( ou::gp::RootNode** pNode ) {
      (*this)( **pNode );
    }
  };

  // can parallize this once all is working sequentially
  while ( pop.MakeNewGeneration( true ) ) {
    const vGeneration_t& gen( pop.CurrentGeneration() );
    BOOST_FOREACH( const ou::gp::Individual& ind, gen ) {
      m_pswStrategy = new StrategyWrapper;
      StrategyEquity::registrations_t m_registrations;
      m_pswStrategy->Init( 
        m_registrations,
        m_pInstrument, date( 2012, 7, 22 ), "/app/semiauto/2012-Jul-22 18:08:14.285807",
        fastdelegate::MakeDelegate( ind.m_Signals.rnLong, &ou::gp::RootNode::EvaluateBoolean ),
        fastdelegate::MakeDelegate( ind.m_Signals.rnShort, &ou::gp::RootNode::EvaluateBoolean ) );
      // update nodes with specific time series:
      const_cast<ou::gp::Individual&>(ind).m_Signals.EachSignal( PreProcessNodes() );
      // output the time series description:
      ss.str( "" );
      ind.TreeToString( ss );
      std::cout << ss.str() << std::endl;
      m_pswStrategy->Start();  // run provider synchronously
      const_cast<ou::gp::Individual&>( ind ).m_dblRawFitness = m_pswStrategy->GetPL();
      delete m_pswStrategy;
      m_pswStrategy = 0;
    }
    pop.CalcFitness();

    // optimization:
    // number of trades similar to number in ZigZag?
    // minimize drawdown 
    // # winning trades > # losing trades, 60/40?  70/30?
    // pareto minimum concept

    ss.str( "" );
    gen.front().TreeToString( ss );
    std::cout << gen.front().m_dblRawFitness << ", " << ss << std::endl;
  }

}

int AppOptimizeStrategy::OnExit(  void ) {

  delete m_pswStrategy;
  m_pswStrategy = 0;

  return 0;
}

void AppOptimizeStrategy::HandlePopulateDatabase( void ) {

  ou::tf::CAccountManager::pAccountAdvisor_t pAccountAdvisor 
    = ou::tf::CAccountManager::Instance().ConstructAccountAdvisor( 
    "aaRay", "Raymond Burkholder", "One Unified" );

  ou::tf::CAccountManager::pAccountOwner_t pAccountOwner
    = ou::tf::CAccountManager::Instance().ConstructAccountOwner( 
    "aoRay", "aaRay", "Raymond", "Burkholder" );

  ou::tf::CAccountManager::pAccount_t pAccountIB
    = ou::tf::CAccountManager::Instance().ConstructAccount( 
    "ib01", "aoRay", "Raymond Burkholder", ou::tf::keytypes::EProviderIB, "Interactive Brokers", "acctid", "login", "password" );

  ou::tf::CAccountManager::pAccount_t pAccountIQFeed
    = ou::tf::CAccountManager::Instance().ConstructAccount( 
    "iq01", "aoRay", "Raymond Burkholder", ou::tf::keytypes::EProviderIQF, "IQFeed", "acctid", "login", "password" );

  ou::tf::CPortfolioManager::pPortfolio_t pPortfolio
    = ou::tf::CPortfolioManager::Instance().ConstructPortfolio( 
    "pflioGold", "aoRay", "options" );

}

