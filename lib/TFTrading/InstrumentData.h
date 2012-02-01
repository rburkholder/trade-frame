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

  InstrumentData( const CInstrument::pInstrument_t& pInstrument, unsigned int nSigDigits = 2 );
  InstrumentData( CInstrument* pInstrument, unsigned int nSigDigits = 2 );
  InstrumentData( const InstrumentData& data );
  virtual ~InstrumentData(void);

  void AddQuoteHandler( CProviderManager::pProvider_t pProvider );
  void RemoveQuoteHandler( CProviderManager::pProvider_t pProvider );
  void AddTradeHandler( CProviderManager::pProvider_t pProvider );
  void RemoveTradeHandler( CProviderManager::pProvider_t pProvider );
  void AddGreekHandler( CProviderManager::pProvider_t pProvider );
  void RemoveGreekHandler( CProviderManager::pProvider_t pProvider );

  CInstrument::pInstrument_t GetInstrument( void ) { return m_pInstrument; };

  void SaveSeries( const std::string& sPrefix );
  unsigned int SigDigits( void ) { return m_nSignificantDigits; };

  void Reset( void );

protected:

  CQuotes m_quotes;
  CTrades m_trades;
  CGreeks m_greeks;

private:

  unsigned int m_nSignificantDigits;

  CInstrument::pInstrument_t m_pInstrument;

  void Init( void );

  void HandleQuote( const CQuote& quote );
  void HandleTrade( const CTrade& trade );
  void HandleGreek( const CGreek& greek );
};

} // namespace tf
} // namespace ou
