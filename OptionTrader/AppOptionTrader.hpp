/************************************************************************
 * Copyright(c) 2025, One Unified. All rights reserved.                 *
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
 * File:    AppOptionTrader.hpp
 * Author:  raymond@burkholder.net
 * Project: OptionTrader
 * Created: July 17, 2025 08:41:21
 */

#pragma once

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <wx/app.h>

#include <OUCharting/ChartDataView.h>

#include <TFTrading/DBOps.h>
#include <TFTrading/ComposeInstrument.hpp>

#include <TFIQFeed/Provider.h>

#include <TFOptions/Chains.h>

#include <TFBitsNPieces/FrameWork02.hpp>

class FrameMain;
class OptionManager;

namespace ou {
namespace tf {
  class InstrumentViews;
}
}

class AppOptionTrader:
  public wxApp
, public ou::tf::FrameWork02<AppOptionTrader>
{
  friend class boost::serialization::access;
  friend ou::tf::FrameWork02<AppOptionTrader>;
public:
protected:
private:

  bool m_bComposeInstrumentIQFeed_ready;

  FrameMain* m_pFrameMain;
  ou::tf::InstrumentViews* m_pInstrumentViews;

  ou::ChartDataView m_cdv;

  using pIQFeed_t = ou::tf::iqfeed::Provider::pProvider_t;
  pIQFeed_t m_piqfeed;

  ou::tf::DBOps m_db;

  using pComposeInstrument_t = std::shared_ptr<ou::tf::ComposeInstrument>;
  pComposeInstrument_t m_pComposeInstrumentIQFeed;

  using pOptionManager_t = std::unique_ptr<OptionManager>;
  pOptionManager_t m_pOptionManager;

  void ConnectionsStart();
  void HandleIQFeedConnected( int );
  void ConnectionsReady();

  using pInstrument_t = ou::tf::Instrument::pInstrument_t;
  using fInstrumentOption_t = std::function<void(size_t, pInstrument_t /* option */)>; // needs to match CollectATM
  void QueryChains( pInstrument_t, fInstrumentOption_t&& ); // underlying

  virtual bool OnInit();
  virtual int OnExit();
  void OnClose( wxCloseEvent& event );

  void SaveState();
  void LoadState();

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    ar & *m_pFrameMain;
    ar & *m_pInstrumentViews;
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    ar & *m_pFrameMain;
    ar & *m_pInstrumentViews;
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};

BOOST_CLASS_VERSION(AppOptionTrader, 1)

DECLARE_APP(AppOptionTrader)
