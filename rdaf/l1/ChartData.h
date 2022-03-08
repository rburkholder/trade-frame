/************************************************************************
 * Copyright(c) 2022, One Unified. All rights reserved.                 *
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
 * File:    ChartData.h
 * Author:  raymond@burkholder.net
 * Project: rdaf/l1
 * Created: February 6, 2022 16:15
 * retrofit back to LiveChart
 */

#pragma once

#include <thread>

#include <OUCharting/ChartDVBasics.h>

#include <TFTrading/Watch.h>
#include <TFTrading/ProviderManager.h>

class TRint;
class TFile;
class TTree;

namespace config {
  class Options;
}

class ChartData: public ou::ChartDVBasics {
public:

  using pProvider_t = ou::tf::ProviderInterfaceBase::pProvider_t;
  using pWatch_t    = ou::tf::Watch::pWatch_t;

  ChartData(
    const std::string& sFileName,
    const std::string& sIQFeedSymbol,
    const config::Options&,
    pProvider_t
    );
  virtual ~ChartData();

  pWatch_t GetWatch( void ) { return m_pWatch; };

  void StartWatch();
  void StopWatch();

  void SaveValues( const std::string& sPrefix );

protected:
private:

  const config::Options& m_options;

  bool m_bWatching;

  pWatch_t m_pWatch;

  ou::tf::Quote m_quote;

  struct TreeQuote {
    double time;
    double ask;
    uint64_t askvol;
    double bid;
    uint64_t bidvol;
  };

  struct TreeTrade {
    double time;
    double price;
    uint64_t vol;
    int64_t direction;
  };

  std::thread m_threadRdaf;
  std::unique_ptr<TRint> m_prdafApp;

  std::unique_ptr<TFile> m_pFile;

  TreeQuote m_treeQuote;
  TreeTrade m_treeTrade;

  // https://root.cern/doc/master/classTTree.html
  using pTTree_t = std::shared_ptr<TTree>;
  pTTree_t m_pTreeQuote;
  pTTree_t m_pTreeTrade;

  void StartRdaf( const std::string& sFilePrefix );
  static void ThreadRdaf( ChartData*, const std::string& sFilePrefix );

  void HandleQuote( const ou::tf::Quote& quote );
  void HandleTrade( const ou::tf::Trade& trade );

};

