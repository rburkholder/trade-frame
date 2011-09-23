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

#include <TFTimeSeries/TimeSeries.h>

//#include <TFIndicators/TimeSeriesSlidingWindow.h>
#include <TFIndicators/TSSWStats.h>
#include <TFIndicators/TSSWStochastic.h>

#include <TFTrading/Instrument.h>
#include <TFTrading/ProviderManager.h>

using namespace ou::tf;

class InstrumentData {
public:

  InstrumentData( const CInstrument::pInstrument_t& pInstrument );
  InstrumentData( CInstrument* pInstrument );
  InstrumentData( const InstrumentData& data );
  ~InstrumentData(void);

  void AddQuoteHandler( CProviderManager::pProvider_t pProvider );
  void RemoveQuoteHandler( CProviderManager::pProvider_t pProvider );
  void AddTradeHandler( CProviderManager::pProvider_t pProvider );
  void RemoveTradeHandler( CProviderManager::pProvider_t pProvider );

  void SaveSeries( void );

protected:
private:

  CInstrument::pInstrument_t m_pInstrument;
  CQuotes m_quotes;
  CTrades m_trades;

  TSSWStatsMidQuote m_stats;
  TSSWStochastic m_stoch;

  void HandleQuote( const CQuote& quote );
  void HandleTrade( const CTrade& trade );

};

