/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
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

#include <stdexcept>

#include <LibTimeSeries/DatedDatum.h>
#include <LibTrading/Symbol.h>

#ifndef IB_USE_STD_STRING
#define IB_USE_STD_STRING
#endif

#include "TWS/EWrapper.h"

class CIBSymbol : public CSymbol<CIBSymbol> {
  friend class CIBTWS;
public:

  typedef CSymbol<CIBSymbol> inherited_t;
  typedef inherited_t::pInstrument_t pInstrument_t;
  typedef inherited_t::pSymbol_t pSymbol_t;

  //CIBSymbol( TickerId id, pInstrument_t pInstrument );
  CIBSymbol( inherited_t::symbol_id_t, pInstrument_t pInstrument, TickerId id );
  ~CIBSymbol(void);

  TickerId GetTickerId( void ) { return m_TickerId; };

  void Greeks( double optPrice, double undPrice, double pvDividend, 
    double impliedVol, double delta, double gamma, double vega, double theta );

  double OptionPrice( void ) { return m_dblOptionPrice; };
  double ImpliedVolatility( void ) { 
    if ( !m_bOptionsSet ) throw std::logic_error( "ImplVol not set" );
    return m_greek.ImpliedVolatility(); 
  };
  double Delta( void ) { 
    if ( !m_bOptionsSet ) throw std::logic_error( "Delta not set" );
    return m_greek.Delta(); 
  };
  double Gamma( void ) { 
    if ( !m_bOptionsSet ) throw std::logic_error( "Gamma not set" );
    return m_greek.Gamma(); 
  };

protected:

  TickerId m_TickerId;

  bool m_bAskFound;
  bool m_bAskSizeFound;
  bool m_bBidFound;
  bool m_bBidSizeFound;

  bool m_bLastTimeStampFound;
  bool m_bLastFound;
  bool m_bLastSizeFound;

  int m_nAskSize;
  int m_nBidSize;
  int m_nLastSize;
  int m_nVolume;

  double m_dblAsk;
  double m_dblBid;
  double m_dblLast;

  double m_dblHigh;
  double m_dblLow;
  double m_dblClose;

  bool m_bOptionsSet;  // option info is available
  double m_dblOptionPrice;
  double m_dblUnderlyingPrice;
  double m_dblPvDividend;
  CGreek m_greek;

  void SetQuoteTradeWatchInProgress( void ) { m_bQuoteTradeWatchInProgress = true; };
  void ResetQuoteTradeWatchInProgress( void ) { m_bQuoteTradeWatchInProgress = false; };
  bool GetQuoteTradeWatchInProgress( void ) { return m_bQuoteTradeWatchInProgress; };
  bool m_bQuoteTradeWatchInProgress;

  void SetDepthWatchInProgress( void ) { m_bDepthWatchInProgress = true; };
  void ResetDepthWatchInProgress( void ) { m_bDepthWatchInProgress = false; };
  bool GetDepthWatchInProgress( void ) { return m_bDepthWatchInProgress; };
  bool m_bDepthWatchInProgress;

  void AcceptTickPrice( TickType tickType, double price );
  void AcceptTickSize( TickType tickType, int size );
  void AcceptTickString( TickType tickType, const IBString &value );

  void BuildQuote( void );
  void BuildTrade( void );

private:

  long m_conId;  // matches IB contract id

};
