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

#pragma once

// Started 2012/10/15
// 2013/01/06:
// Auto selects and trades a basket of equities, enters at market start, exits prior to end of regular trading hours
// Once hedging and option calculations are complete, can run auto hedging on basket and see if that works
// Current trading rules are too variable, big gains on one day when in directional market,
//   big losses on another when market moves sideways
// Data has been saved for both types of days, perhaps running GP routines on the data will
//  yield more consistently positive results.  Could try for better money management as well.
// 2019/03/24
//   using pivots, then checking opening against pivot to see which direction to trade based
//     upon probabilities. Use weeklies for symbol selection.

#include <thread>
#include <string>
#include <memory>

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <wx/app.h>
#include <wx/timer.h>

#include <TFBitsNPieces/FrameWork01.h>

// may need to inherit and add more functionality to the class:
#include <TFTrading/DBOps.h>

#include <TFVuTrading/FrameMain.h>
#include <TFVuTrading/PanelLogging.h>

#include "MasterPortfolio.hpp"

class wxSizer;
class wxRadioButton;
class PanelPortfolioStats;

namespace ou {
namespace tf {
  class FrameControls;
  class PanelFinancialChart;
}
}

class AppBasketTrading:
  public wxApp, public ou::tf::FrameWork01<AppBasketTrading> {
    friend class boost::serialization::access;
    friend ou::tf::FrameWork01<AppBasketTrading>;
public:
protected:
private:

  using vSymbol_t = std::vector<std::string>;

  using pPortfolio_t = ou::tf::PortfolioManager::pPortfolio_t;
  using pPosition_t  = ou::tf::PortfolioManager::pPosition_t;

  boost::gregorian::date m_dateTrading; // save the config file instead?
  size_t m_nPeriodWidth;
  size_t m_nStochasticPeriods;
  ou::tf::option::SpreadSpecs m_spread_specs; // save the config file instead?
  ptime m_dtLatestEod;
  vSymbol_t m_vSymbol;

  std::thread m_worker;

  FrameMain* m_pFrameMain;
//  PanelOptionsParameters* m_pPanelOptionsParameters;
  ou::tf::PanelLogging* m_pPanelLogging;
//  PanelBasketTradingMain* m_pPanelBasketTradingMain;
  PanelPortfolioStats* m_pPanelPortfolioStats;
  ou::tf::PanelFinancialChart* m_pPanelFinancialChart;

  std::string m_sDbName;
  std::string m_sStateFileName;
  ou::tf::DBOps m_db;

  std::string m_sPortfolioStrategyAggregate;

  std::unique_ptr<MasterPortfolio> m_pMasterPortfolio;
  pPortfolio_t m_pPortfolioMaster;
  pPortfolio_t m_pPortfolioCurrencyUSD;
  pPortfolio_t m_pPortfolioStrategyAggregate;

  bool m_bData1Connected;
  bool m_bExecConnected;

  wxTimer m_timerGuiRefresh;

  enum EBuySell { Neutral, Buy, Sell }; // deprecated
  EBuySell m_enumBuySell;

  double m_dblMaxPL;
  double m_dblMinPL;

  virtual bool OnInit();
  void OnClose( wxCloseEvent& event );
  virtual int OnExit();

  void Init();

  void OnData1Connected( int );
//  void OnData2Connected( int ) {};
  void OnExecConnected( int );
  void OnData1Disconnected( int );
//  void OnData2Disconnteted( int ) {};
  void OnExecDisconnected( int );

  void OnConnected();

  void BuildMasterPortfolio();

  void HandleButtonLoad();
//  void HandleButtonStart();
  void HandleButtonClosePositions();
  void HandleButtonSave();
  void HandleEmitInfo();
  void HandleEmitIV();
  void HandleButtonTest();

  void HandleTakeProfits();
  void HandleCloseForProfits();

  void HandleAddComboAllowed();
  void HandleAddComboForced();

  void HandlePopulateDatabase();
  void HandleLoadDatabase();

  void HandlePortfolioLoad( pPortfolio_t& pPortfolio );
  void HandlePositionLoad( pPosition_t& pPosition );

  void HandleDbOnLoad( ou::db::Session& session );
  void HandleDbOnPopulate( ou::db::Session& session );
  void HandleRegisterTables( ou::db::Session& session );
  void HandleRegisterRows( ou::db::Session& session );

  void HandleGuiRefresh( wxTimerEvent& event );

  void HandleMenuActionTestSelection();

  void SaveState();
  void LoadState();

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    ar & *m_pFrameMain;
    ar & *m_pPanelProviderControl;
    ar & m_enumBuySell;
    ar & *m_pPanelFinancialChart;
    //ar & *m_pMasterPortfolio;
    {
      ar & MasterPortfolio::m_bFramePanelComboOrder;
      ar & MasterPortfolio::m_pointFramePanelComboOrder.x;
      ar & MasterPortfolio::m_pointFramePanelComboOrder.y;
      ar & MasterPortfolio::m_sizeFramePanelComboOrder.GetWidth();
      ar & MasterPortfolio::m_sizeFramePanelComboOrder.GetHeight();
    }
    //ar & m_splitPanels->GetSashPosition();
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    ar & *m_pFrameMain;
    if ( 5 <= version ) {
      ar & *m_pPanelProviderControl;
    }
    if ( 2 <= version ) {
      ar & m_enumBuySell;
      switch ( m_enumBuySell ) {
        case EBuySell::Buy:
          //m_rbBuy->SetValue( true );
          //m_pMasterPortfolio->SetDefaultOrderSide( ou::tf::OrderSide::Buy );
          break;
        case EBuySell::Neutral:
          //m_rbNeutral->SetValue( true );
          //m_pMasterPortfolio->SetDefaultOrderSide( ou::tf::OrderSide::Unknown );
          break;
        case EBuySell::Sell:
          //m_rbSell->SetValue( true );
          //m_pMasterPortfolio->SetDefaultOrderSide( ou::tf::OrderSide::Sell );
          break;
      }
    }
    if ( 4 <= version ) {
      ar & *m_pPanelFinancialChart;
    }
    if ( 7 <= version ) {
      //ar & *m_pMasterPortfolio;
      if ( 8 <= version ) {
        ar & MasterPortfolio::m_bFramePanelComboOrder;
      }
      else {
        MasterPortfolio::m_bFramePanelComboOrder = true;
      }
      int x,y,h,w;
      ar & x;
      ar & y;
      ar & w;
      ar & h;

      MasterPortfolio::m_pointFramePanelComboOrder = wxPoint( x, y );
      MasterPortfolio::m_sizeFramePanelComboOrder = wxSize( w, h );
    }
    if ( 6 <= version ) {
      // deprecated
    }
    //m_splitPanels->SetSashPosition( x );
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};

BOOST_CLASS_VERSION(AppBasketTrading, 8)
DECLARE_APP(AppBasketTrading)

