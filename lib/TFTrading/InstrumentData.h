/************************************************************************
 * Copyright(c) 2012, One Unified. All rights reserved.                 *
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

#include <TFTrading/Instrument.h>
#include <TFTrading/ProviderManager.h>

namespace ou { // One Unified
namespace tf { // TradeFrame

class InstrumentData {
public:

  InstrumentData( const Instrument::pInstrument_t& pInstrument, unsigned int nSigDigits = 2 );
  InstrumentData( Instrument* pInstrument, unsigned int nSigDigits = 2 );
  InstrumentData( const InstrumentData& data );
  virtual ~InstrumentData(void);

  void AddQuoteHandler( CProviderManager::pProvider_t pProvider );
  void RemoveQuoteHandler( CProviderManager::pProvider_t pProvider );
  void AddTradeHandler( CProviderManager::pProvider_t pProvider );
  void RemoveTradeHandler( CProviderManager::pProvider_t pProvider );
  void AddGreekHandler( CProviderManager::pProvider_t pProvider );
  void RemoveGreekHandler( CProviderManager::pProvider_t pProvider );

  Instrument::pInstrument_t GetInstrument( void ) { return m_pInstrument; };

  void SaveSeries( const std::string& sPrefix );
  unsigned int SigDigits( void ) { return m_nSignificantDigits; };

  void Reset( void );

protected:

  Quotes m_quotes;
  Trades m_trades;
  Greeks m_greeks;

private:

  unsigned int m_nSignificantDigits;

  Instrument::pInstrument_t m_pInstrument;

  void Init( void );

  void HandleQuote( const Quote& quote );
  void HandleTrade( const Trade& trade );
  void HandleGreek( const Greek& greek );
};

} // namespace tf
} // namespace ou
