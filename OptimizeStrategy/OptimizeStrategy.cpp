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

#define FUSION_MAX_VECTOR_SIZE 13

#include <wx/bitmap.h>

#include <boost/foreach.hpp>
#include <boost/thread.hpp>  // separate thread background merge processing
#include <boost/bind.hpp>
#include <boost/asio.hpp>

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

  ou::SingletonBase::SetLocalCommonInstanceSource( ou::SingletonBase::Assigned );

  boost::thread thrdOptimizer( boost::bind( &AppOptimizeStrategy::Optimizer, this ) );

  return 1;

}

void AppOptimizeStrategy::Optimizer( void ) {
  //m_sim->SetGroupDirectory( "/semiauto/2011-Sep-23 19:17:48.252497" );
// ->  m_pProvider->SetGroupDirectory( "/app/semiauto/2011-Nov-06 18:54:22.184889" );
  //m_sim->SetGroupDirectory( "/app/semiauto/2011-Nov-07 18:53:31.016760" );
  //m_sim->SetGroupDirectory( "/app/semiauto/2011-Nov-08 18:58:29.396624" );
//  m_sim->SetExecuteAgainst( ou::tf::CSimulateOrderExecution::EAQuotes );

  ou::tf::CInstrumentManager::SetLocalCommonInstance( new ou::tf::CInstrumentManager );

  ou::tf::CInstrumentManager& mgr( ou::tf::CInstrumentManager::LocalCommonInstance() );
  m_pInstrument = mgr.Exists( "+GCQ12" ) ? mgr.Get( "+GCQ12" ) : mgr.ConstructFuture( "+GCQ12", "SMART", 2012, 8 );
  m_pInstrument->SetMultiplier( 100 );
  m_pInstrument->SetMinTick( 0.1 );

  // manage the genetic programming discovery process here
  ou::gp::Population pop( 75 );

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

  struct ProcessIndividual {
    ProcessIndividual( ou::gp::Individual& ind, pInstrument_t pInstrument )
      : m_ind( ind ), m_pInstrument( pInstrument ), m_pswStrategy( new StrategyWrapper )
    {
    }
    ~ProcessIndividual( void ) {
    }
    void Init( void ) {  // run synchronously
      // /app/semiauto/2012-Jul-22 18:08:14.285807
      // /app/semiauto/2012-Jul-23 18:41:49.332859
      // /app/semiauto/2012-Jul-24 18:37:57.017369
      // /app/semiauto/2012-Jul-25 18:50:17.756534
      // /app/semiauto/2012-Jul-26 19:17:28.757619
      StrategyEquity::registrations_t m_registrations; // contains a static component, be careful, which is used in Init of next step
      m_pswStrategy->Init( 
        m_registrations,
        m_pInstrument, date( 2012, 7, 22 ), "/app/semiauto/2012-Jul-22 18:08:14.285807",
        fastdelegate::MakeDelegate( m_ind.m_Signals.rnLong, &ou::gp::RootNode::EvaluateBoolean ),
        fastdelegate::MakeDelegate( m_ind.m_Signals.rnShort, &ou::gp::RootNode::EvaluateBoolean ) );
      const_cast<ou::gp::Individual&>(m_ind).m_Signals.EachSignal( PreProcessNodes() );
      m_ind.TreeToString( m_ind.m_ssFormula );
    }
    void Run( void ) { // run asynchronously
      m_pswStrategy->Start(); 
      std::cout << m_ind.m_ssFormula.str() << std::endl;
      m_ind.m_dblRawFitness = m_pswStrategy->GetPL();
      std::cout << "---- " << m_ind.m_id << " ----------------------------" << std::endl;
      delete m_pswStrategy;
      m_pswStrategy = 0;
    }
  private:
    ou::gp::Individual& m_ind;
    pInstrument_t m_pInstrument;
    StrategyWrapper* m_pswStrategy;
  };

  std::vector<ProcessIndividual*> vpi;

  // http://boost.2283326.n4.nabble.com/Is-thread-pool-using-asio-and-thread-group-working-as-intended-td4552297.html
  // set up some threads for running the strategy
    while ( pop.MakeNewGeneration() ) {
      std::cout << "==== N:" << pop.m_nNew << ",E:" << pop.m_nElites << ",R:" << pop.m_nReproductions << ",X:" << pop.m_nCrossOvers << " ====" << std::endl;
      const vGeneration_t& gen( pop.CurrentGeneration() );

      boost::asio::io_service srvc;
      boost::thread_group threads;

      {

        boost::asio::io_service::work work( srvc );  // keep things running while real work arrives
        for ( std::size_t ix = 0; ix < 2; ix++ ) {
          threads.create_thread( boost::bind( &boost::asio::io_service::run, &srvc ) ); // add handlers
        }

        BOOST_FOREACH( const ou::gp::Individual& ind, gen ) {
          
          if ( ind.IsComputed() ) {
            std::cout 
              << "Computed: " 
              << ind.m_dblRawFitness << std::endl
              << ind.m_ssFormula.str() << std::endl;
            std::cout << "---- " << ind.m_id << " ----------------------------" << std::endl;
          }
          else {
            ou::gp::Individual& i( const_cast<ou::gp::Individual&>( ind ) );
            i.SetComputed();
            //ProcessIndividual pi( i, m_pInstrument );
            ProcessIndividual* ppi( new ProcessIndividual( i, m_pInstrument ) );
            vpi.push_back( ppi );
            ppi->Init();
            //std::cout << ind.m_ssFormula.str() << std::endl;
  //          pi.Run();  // can't run asynchronously due to singleton managers and singleton hdf5 manager
            srvc.post( boost::bind( &ProcessIndividual::Run, ppi ) );  
          }
        }

      }

      // at some point, add the above Formula strings to master table, so random calcs which match prior randoms aren't computed

      threads.join_all();  // wait for all work to complete

      for ( std::vector<ProcessIndividual*>::iterator iter = vpi.begin(); vpi.end() != iter; iter++ ) {
        delete *iter;
      }
      vpi.clear();

      pop.CalcFitness();

      // optimization:
      // number of trades similar to number in ZigZag?
      // minimize drawdown 
      // # winning trades > # losing trades, 60/40?  70/30?
      // pareto minimum concept

      //ss.str( "" );
      //gen.front().TreeToString( ss );  // can't do this as the strategy no longer exists
      std::cout 
        << "==== " << gen.front().m_id << " @ " << gen.front().m_dblRawFitness << " ====" << std::endl
        << gen.front().m_ssFormula.str() << std::endl;
    }
//  }


}

int AppOptimizeStrategy::OnExit(  void ) {

  return 0;
}

void AppOptimizeStrategy::HandlePopulateDatabase( void ) {

  ou::tf::CAccountManager::pAccountAdvisor_t pAccountAdvisor 
    = ou::tf::CAccountManager::LocalCommonInstance().ConstructAccountAdvisor( 
    "aaRay", "Raymond Burkholder", "One Unified" );

  ou::tf::CAccountManager::pAccountOwner_t pAccountOwner
    = ou::tf::CAccountManager::LocalCommonInstance().ConstructAccountOwner( 
    "aoRay", "aaRay", "Raymond", "Burkholder" );

  ou::tf::CAccountManager::pAccount_t pAccountIB
    = ou::tf::CAccountManager::LocalCommonInstance().ConstructAccount( 
    "ib01", "aoRay", "Raymond Burkholder", ou::tf::keytypes::EProviderIB, "Interactive Brokers", "acctid", "login", "password" );

  ou::tf::CAccountManager::pAccount_t pAccountIQFeed
    = ou::tf::CAccountManager::LocalCommonInstance().ConstructAccount( 
    "iq01", "aoRay", "Raymond Burkholder", ou::tf::keytypes::EProviderIQF, "IQFeed", "acctid", "login", "password" );

  ou::tf::CPortfolioManager::pPortfolio_t pPortfolio
    = ou::tf::CPortfolioManager::LocalCommonInstance().ConstructPortfolio( 
    "pflioGold", "aoRay", "options" );

}

