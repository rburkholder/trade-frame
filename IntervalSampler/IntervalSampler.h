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

#include <TFVuTrading/FrameMain.h>
#include <TFVuTrading/PanelLogging.h>

#include "Capture.h"

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

  ptime m_dtNextRotation;
  std::ofstream m_out;

  using pCapture_t = std::unique_ptr<Capture>;
  using vCapture_t = std::vector<pCapture_t>;
  vCapture_t m_vCapture;

  void HandleIQFeedConnecting( int );
  void HandleIQFeedConnected( int );
  void HandleIQFeedDisconnecting( int );
  void HandleIQFeedDisconnected( int );
  void HandleIQFeedError( size_t );

  void OutputFileOpen( ptime dt );
  void OutputFileCheck( ptime dt );

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

