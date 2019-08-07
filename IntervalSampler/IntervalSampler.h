/************************************************************************
 * Copyright(c) 2019, One Unified. All rights reserved.                 *
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
 * File:    IntervalSampler.h
 * Author:  raymond@burkholder.net
 * Project: IntervalSampler
 * Created on August 6, 2019, 11:11 AM
 */

#ifndef INTERVALSAMPLER_H
#define INTERVALSAMPLER_H

#include <vector>
#include <functional>
#include <fstream>

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <wx/wx.h>

#include <TFIQFeed/IQFeedProvider.h>

#include <TFTimeSeries/BarFactory.h>

#include <TFTrading/Watch.h>

#include <TFVuTrading/FrameMain.h>
#include <TFVuTrading/PanelLogging.h>

class AppIntervalSampler:
  public wxApp
{
  friend class boost::serialization::access;
public:
protected:
private:

  std::string m_sStateFileName;

  FrameMain* m_pFrameMain;
  ou::tf::PanelLogging* m_pPanelLogging;

  using pProviderIQFeed_t = ou::tf::IQFeedProvider::pProvider_t;

  pProviderIQFeed_t m_pIQFeed;
  bool m_bIQFeedConnected;

  using vSymbol_t = std::vector<std::string>;
  vSymbol_t m_vSymbol;

  std::ofstream m_out;

  using pWatch_t = ou::tf::Watch::pWatch_t;
  struct Watch {

    using fBarComplete_t
      = std::function<void(
                            const ou::tf::Instrument::idInstrument_t&,
                            const ou::tf::Bar&,
                            const ou::tf::Quote&,
                            const ou::tf::Trade&
                            )>;

    pWatch_t m_pWatch;
    ou::tf::BarFactory m_bf;
    fBarComplete_t m_fBarComplete;
    Watch() {};
    void Assign(
      ou::tf::BarFactory::duration_t duration,
      pWatch_t pWatch,
      fBarComplete_t&& fBarComplete
    ) {
      assert( nullptr != fBarComplete );
      m_fBarComplete = std::move( fBarComplete );
      m_bf.SetBarWidth( duration );
      m_pWatch = pWatch;
      m_bf.SetOnBarComplete( MakeDelegate( this, &Watch::HandleBarComplete ) );
      //m_pWatch->OnQuote.Add( MakeDelegate( this, &Watch::HandleQuote ) );
      m_pWatch->OnTrade.Add( MakeDelegate( this, &Watch::HandleTrade ) );
      m_pWatch->StartWatch();
    }
    ~Watch() {
      if ( m_pWatch ) m_pWatch->StopWatch();
      m_bf.SetOnBarComplete( nullptr );
      m_pWatch->OnTrade.Remove( MakeDelegate( this, &Watch::HandleTrade ) );
      m_pWatch.reset(); // TODO: need to wait for queue to flush
    }
    void HandleTrade( const ou::tf::Trade& trade ) {
      m_bf.Add( trade );
    }
    void HandleBarComplete( const ou::tf::Bar& bar ) {
      m_fBarComplete(
        m_pWatch->GetInstrument()->GetInstrumentName(),
        bar,
        m_pWatch->LastQuote(),
        m_pWatch->LastTrade()
        );
    }
  };

  using vWatch_t = std::vector<Watch>;
  vWatch_t m_vWatch;

  void HandleIQFeedConnecting( int );
  void HandleIQFeedConnected( int );
  void HandleIQFeedDisconnecting( int );
  void HandleIQFeedDisconnected( int );
  void HandleIQFeedError( size_t );

  virtual bool OnInit();
  virtual int OnExit();
  void OnClose( wxCloseEvent& event );

  void SaveState();
  void LoadState();

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    ar & *m_pFrameMain;
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    ar & *m_pFrameMain;
    }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};

BOOST_CLASS_VERSION(AppIntervalSampler, 1)
DECLARE_APP(AppIntervalSampler)

#endif /* INTERVALSAMPLER_H */

