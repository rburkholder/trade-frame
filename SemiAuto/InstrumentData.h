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

#include <OUCommon/SmartVar.h>

#include <TFTimeSeries/TimeSeries.h>

#include <TFIndicators/TSSWStats.h>
#include <TFIndicators/TSSWStochastic.h>

#include <TFTrading/Instrument.h>
#include <TFTrading/ProviderManager.h>

using namespace ou::tf;

class InstrumentData {
public:

  enum enumIndex { Low = 0, Price, High, Roc, Stochastic, _Count };
  typedef ou::SmartVar<double> var_t;

  InstrumentData( const CInstrument::pInstrument_t& pInstrument );
  InstrumentData( CInstrument* pInstrument );
  InstrumentData( const InstrumentData& data );
  ~InstrumentData(void);

  void AddQuoteHandler( CProviderManager::pProvider_t pProvider );
  void RemoveQuoteHandler( CProviderManager::pProvider_t pProvider );
  void AddTradeHandler( CProviderManager::pProvider_t pProvider );
  void RemoveTradeHandler( CProviderManager::pProvider_t pProvider );

  CInstrument::pInstrument_t GetInstrument( void ) { return m_pInstrument; };

  void SaveSeries( const std::string& sPrefix );
  var_t& Var( enumIndex ix ) { return m_rSummary[ ix ]; };

protected:
private:

  bool m_bHasData;

  var_t m_rSummary[ _Count ];

  CInstrument::pInstrument_t m_pInstrument;
  CQuotes m_quotes;
  CTrades m_trades;

  TSSWStatsMidQuote m_stats;
  TSSWStochastic m_stoch;

  void HandleQuote( const CQuote& quote );
  void HandleTrade( const CTrade& trade );

};

