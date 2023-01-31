/************************************************************************
 * Copyright(c) 2023, One Unified. All rights reserved.                 *
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
 * File:    AppImpliedVolatility.hpp
 * Author:  raymond@burkholder.net
 * Project: ImpliedVolatility
 * Created: January 27, 2023  19:20:09
 */

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <wx/app.h>
#include <wx/splitter.h>

#include <TFIQFeed/Provider.h>

#include <OUCharting/ChartDataView.h>
#include <OUCharting/ChartEntryIndicator.h>

#include <TFBitsNPieces/FrameWork02.hpp>

#include "Choices.hpp"

class FrameMain;

namespace ou {
namespace tf {
  class PanelLogging;
  class WinChartView;
  class BuildInstrument;
namespace v2 {
  class PanelProviderControl;
}
} // namespace tf
} // namespace ou

class AppImpliedVolatility:
  public wxApp,
  public ou::tf::FrameWork02<AppImpliedVolatility>
{
  friend ou::tf::FrameWork02<AppImpliedVolatility>;
  friend class boost::serialization::access;
public:
protected:
private:

  using pProviderIQFeed_t = ou::tf::iqfeed::IQFeedProvider::pProvider_t;

  config::Choices m_choices;

  FrameMain* m_pFrameMain;
  ou::tf::PanelLogging* m_pPanelLogging;
  ou::tf::WinChartView* m_pWinChartView;
  ou::tf::v2::PanelProviderControl* m_pPanelProviderControl;

  wxSplitterWindow* m_splitterData;

  pProviderIQFeed_t m_iqfeed; // live - data

  std::unique_ptr<ou::tf::BuildInstrument> m_pBuildInstrument;

  //std::unique_ptr<ou::tf::db> m_pdb;

  ou::ChartDataView m_dvChart; // the data

  virtual bool OnInit();
  virtual int OnExit();
  void OnClose( wxCloseEvent& event );

  void HandleMenuActionCloseAndDone();
  void HandleMenuActionSaveValues();

  void ConfirmProviders();

  void SaveState();
  void LoadState();

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    ar & *m_pFrameMain;
    ar & m_splitterData->GetSashPosition();
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    ar & *m_pFrameMain;
    if ( 2 <= version ) {
      int x;
      ar & x;
      m_splitterData->SetSashPosition( x );
    }
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};

BOOST_CLASS_VERSION(AppImpliedVolatility, 1)

DECLARE_APP(AppImpliedVolatility)
