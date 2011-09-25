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

//#include <TFIndicators/TimeSeriesSlidingWindow.h>
#include <TFIndicators/TSSWStats.h>
#include <TFIndicators/TSSWStochastic.h>

#include <TFTrading/Instrument.h>
#include <TFTrading/ProviderManager.h>

using namespace ou::tf;

class InstrumentData {
public:

  struct structSummary {
    double m_dblHigh;
    double m_dblLow;
    double m_dblPrice;
    double m_dblRoc;
    double m_dblStochastic;
    structSummary( void ) : m_dblHigh( 0 ), m_dblLow( 0 ), m_dblPrice( 0 ), m_dblRoc( 0 ), m_dblStochastic( 0 ) {};
    structSummary( const structSummary& u ): 
      m_dblHigh( u.m_dblHigh ), m_dblLow( u.m_dblLow ), m_dblPrice( u.m_dblPrice ), m_dblRoc( u.m_dblRoc ), m_dblStochastic( u.m_dblStochastic ) {};
  };

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
  const structSummary& GetSummary( void ) const { return m_Summary; };

protected:
private:

  structSummary m_Summary;

  CInstrument::pInstrument_t m_pInstrument;
  CQuotes m_quotes;
  CTrades m_trades;

  TSSWStatsMidQuote m_stats;
  TSSWStochastic m_stoch;

  void HandleQuote( const CQuote& quote );
  void HandleTrade( const CTrade& trade );

};

