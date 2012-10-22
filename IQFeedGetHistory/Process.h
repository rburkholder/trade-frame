/************************************************************************
 * Copyright(c) 2010, One Unified. All rights reserved.                 *
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

/*
  handles the various messages for:
  * scanning a list of symbols from an exchange from the CInstrumentFile
  * obtaining history from IQFeed for each symbol
  * creating the appropriate structures 
  * processing the structures looking for promising trades
*/

#include <vector>
#include <string>

#include <boost/thread/locks.hpp>

//#include <TFIQFeed/IQFeedInstrumentFile.h>
#include <TFIQFeed/IQFeedHistoryBulkQuery.h>

class CProcess: 
  public ou::tf::iqfeed::HistoryBulkQuery<CProcess>
{
  friend ou::tf::iqfeed::HistoryBulkQuery<CProcess>;
public:

  typedef ou::tf::iqfeed::HistoryBulkQuery<CProcess> inherited_t;

  CProcess(void);
  ~CProcess(void);
  void Start( void );

protected:

  // CRTP from HistoryBulkQuery<CProcess>
  void OnBars( inherited_t::structResultBar* bars );
  void OnTicks( inherited_t::structResultTicks* ticks );
  void OnCompletion( void );

private:

//  ou::tf::CInstrumentFile m_IF;
//  ou::tf::CInstrumentFile::iterator m_iterSymbols;

  std::vector<std::string> m_vExchanges;  // list of exchanges to be scanned to create: 
  std::vector<std::string> m_vSymbols;  // list of symbols to be scanned

  static const size_t m_BarWindow = 20;  // number of bars to examine

  const size_t m_cntBars;

};

