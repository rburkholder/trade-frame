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

#include <TFBitsNPieces/FrameWork02.hpp>

#include "LuaControl.hpp"

class FrameMain;

class AppMarketTrader:
  public wxApp,
  public ou::tf::FrameWork02<AppMarketTrader>
{
  friend ou::tf::FrameWork02<AppMarketTrader>;
  friend class boost::serialization::access;
public:
protected:
private:

  FrameMain* m_pFrameMain;

  lua::Control m_LuaControl;

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
