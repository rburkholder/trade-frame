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

#include <TFIQFeed/Provider.h>

#include <TFBitsNPieces/FrameWork02.hpp>

class FrameMain;

namespace ou {
namespace tf {
  class AcquireFundamentals;
  class BookOfOptionChains;
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

  //config::Choices m_choices;

  FrameMain* m_pFrameMain;
  ou::tf::BookOfOptionChains* m_pBookOfOptionChains;

  ou::ChartDataView m_cdv;

  using pIQFeed_t = ou::tf::iqfeed::Provider::pProvider_t;
  pIQFeed_t m_piqfeed;

  using pAcquireFundamentals_t = std::shared_ptr<ou::tf::AcquireFundamentals>;
  pAcquireFundamentals_t m_pAcquireFundamentals_burial;

  void HandleIQFeedConnected( int );
  void HandleMenuActionAddSymbol();

  virtual bool OnInit();
  virtual int OnExit();
  void OnClose( wxCloseEvent& event );

  void SaveState();
  void LoadState();

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    ar & *m_pFrameMain;
    ar & *m_pBookOfOptionChains;
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    ar & *m_pFrameMain;
    ar & *m_pBookOfOptionChains;
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};

BOOST_CLASS_VERSION(AppOptionTrader, 1)

DECLARE_APP(AppOptionTrader)
