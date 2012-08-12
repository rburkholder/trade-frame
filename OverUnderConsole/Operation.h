/************************************************************************
 * Copyright(c) 2011, One Unified. All rights reserved.                 *
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

#include <string>

#include <TFInteractiveBrokers/IBTWS.h>
// history comes before provider
#include <TFIQFeed/IQFeedHistoryQuery.h>
#include <TFIQFeed/IQFeedProvider.h>
#include <TFTrading/InstrumentManager.h>

#include "StatesOfTrading.h"

class Operation {
public:

  struct structSymbolInfo {
    std::string sName;
    double S3, S2, S1, PV, R1, R2, R3;
    double dblClose;
  };

  Operation( const structSymbolInfo& si, ou::tf::CIQFeedProvider::pProvider_t, ou::tf::IBTWS::pProvider_t );
  ~Operation(void);

  unsigned int CalcShareCount( double dblFunds );
  bool& ToBeTraded( void ) { return m_bToBeTraded; };
  void Start( double dblAmountToTrade );
  void SaveSeries( const std::string& sPrefix );
  void Stop( void );

  
protected:
private:

  typedef ou::tf::IBTWS::pInstrument_t pInstrument_t;

  structSymbolInfo m_si;
  ou::tf::IBTWS::pProvider_t m_ptws;
  ou::tf::CIQFeedProvider::pProvider_t m_piqfeed;

  ou::tf::CInstrument::pInstrument_t m_pInstrument;

  MachineMarketStates m_md;  // market data state chart

  bool m_bToBeTraded;

  void StartWatch( void );
  void StopWatch( void );

  void HandleIBContractDetails( const ou::tf::IBTWS::ContractDetails& details, const pInstrument_t& pInstrument );
  void HandleIBContractDetailsDone( void );

  // will need to migrate to a container when doing more than one instrument
  void HandleQuote( const ou::tf::Quote& quote );
  void HandleTrade( const ou::tf::Trade& trade );
  void HandleOpen( const ou::tf::Trade& trade );
};

