/************************************************************************
 * Copyright(c) 2024, One Unified. All rights reserved.                 *
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
 * File:    AppMarketTrader.hpp
 * Author:  raymond@burkholder.net
 * Project: MarketTrader
 * Created: 2024/11/10 12:18:19
 */

#pragma once

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <wx/app.h>

#include <TFIQFeed/Provider.h>
#include <TFInteractiveBrokers/IBTWS.h>

#include <TFBitsNPieces/FrameWork02.hpp>

class FrameMain;
class LuaInterface;

class AppMarketTrader:
  public wxApp,
  public ou::tf::FrameWork02<AppMarketTrader>
{
  friend ou::tf::FrameWork02<AppMarketTrader>;
  friend class boost::serialization::access;
public:

protected:
private:

  using pInstrument_t = ou::tf::Instrument::pInstrument_t;
  //using pWatch_t = ou::tf::Watch::pWatch_t;
  //using pPosition_t = ou::tf::Position::pPosition_t;
  //using pPortfolio_t = ou::tf::Portfolio::pPortfolio_t;

  using pProvider_t = ou::tf::ProviderInterfaceBase::pProvider_t;
  using pProviderIB_t = ou::tf::ib::TWS::pProvider_t;
  using pProviderIQFeed_t = ou::tf::iqfeed::Provider::pProvider_t;

  using pLuaInterface_t = std::unique_ptr<LuaInterface>;

  using fInstrumentConstructed_t = std::function<void(const std::string&)>;

  FrameMain* m_pFrameMain;

  bool m_bProvidersConnected;

  pProvider_t       m_data;
  pProvider_t       m_exec;

  pProviderIB_t     m_tws;    // live - [ execution ]
  pProviderIQFeed_t m_iqf;    // live - [ data ], simulation - [ execution ]

  pLuaInterface_t m_pLuaInterface;

  void EnableProviders();
  void ProviderConnected( int );
  void ProviderDisconnected( int );
  void DisableProviders();


  virtual bool OnInit() override;
  void OnClose( wxCloseEvent& event );
  virtual int OnExit();

  void OnFrameMainAutoMove( wxMoveEvent& );

  void SaveState();
  void LoadState();

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    ar & *m_pFrameMain;
    //ar & m_splitterData->GetSashPosition();
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    ar & *m_pFrameMain;
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};

BOOST_CLASS_VERSION(AppMarketTrader, 1)

//DECLARE_APP(AppMarketTrader)
