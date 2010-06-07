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

#include <LibTrading/InstrumentFile.h>
#include <LibIQFeed/IQFeedHistoryBulkQuery.h>
#include <LibIndicators/Darvas.h>

class CProcess: 
  public CIQFeedHistoryBulkQuery<CProcess>,
  public CDarvas<CProcess>
{
  friend CIQFeedHistoryBulkQuery<CProcess>;
  friend CDarvas<CProcess>;
public:

  typedef CIQFeedHistoryBulkQuery<CProcess> inherited_t;

  CProcess(void);
  ~CProcess(void);
  void Start( void );

protected:

  // CRTP from CIQFeedHistoryBulkQuery<CProcess>
  void OnBars( inherited_t::structResultBar* bars );
  void OnTicks( inherited_t::structResultTicks* ticks );
  void OnCompletion( void );

  // CRTP from CDarvas<CProcess>
//  void ConservativeTrigger( void ) {};
  void AggressiveTrigger( void );
  void SetStop( double stop ) { m_dblStop = stop; };
//  void StopTrigger( void ) {};
  void BreakOutAlert( size_t );

private:

  size_t m_cntBars;

  double m_dblStop;

  CInstrumentFile m_IF;
  CInstrumentFile::iterator m_iterSymbols;

  std::vector<std::string> m_vExchanges;  // list of exchanges to be scanned to create: 
  std::vector<std::string> m_vSymbols;  // list of symbols to be scanned

  std::stringstream m_ss;

  static const size_t m_BarWindow = 20;  // number of bars to examine
  size_t m_ixRelative;
  bool m_bTriggered;
};

