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

#include <TFTimeSeries/TimeSeries.h>

#include <TFIndicators/TSSWStats.h>
#include <TFIndicators/TSSWStochastic.h>

#include <TFTrading/Instrument.h>
#include <TFTrading/ProviderManager.h>

// 20120129 make use of the refactored <TFTrading/InstrumentData.h>

using namespace ou::tf;

class InstrumentData {
public:

  enum enumIndex { Low = 0, Price, High, Roc, Stochastic, _Count };
  typedef double var_t;

  InstrumentData( const Instrument::pInstrument_t& pInstrument, unsigned int nSigDigits = 2 );
  InstrumentData( Instrument* pInstrument, unsigned int nSigDigits = 2 );
  InstrumentData( const InstrumentData& data );
  ~InstrumentData(void);

  void AddQuoteHandler( ProviderManager::pProvider_t pProvider );
  void RemoveQuoteHandler( ProviderManager::pProvider_t pProvider );
  void AddTradeHandler( ProviderManager::pProvider_t pProvider );
  void RemoveTradeHandler( ProviderManager::pProvider_t pProvider );

  Instrument::pInstrument_t GetInstrument( void ) { return m_pInstrument; };

  void SaveSeries( const std::string& sPrefix );
  var_t& Var( enumIndex ix ) { return m_rSummary[ ix ]; };
  unsigned int SigDigits( void ) { return m_nSignificantDigits; };

  void Reset( void );

protected:
private:

  unsigned int m_nSignificantDigits;

  var_t m_rSummary[ _Count ];

  Instrument::pInstrument_t m_pInstrument;
  ou::tf::Quotes m_quotes;
  ou::tf::Trades m_trades;

  ou::tf::TSSWStatsMidQuote m_stats;
  ou::tf::TSSWStochastic m_stoch;

  void Init( void );

  void HandleQuote( const ou::tf::Quote& quote );
  void HandleTrade( const ou::tf::Trade& trade );

};

