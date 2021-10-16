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

#include <string>

#include "LibCommon/FastDelegate.h"
using namespace fastdelegate;

//#include "IQFeedRetrieveHistory.h"

// 
// CHistoryCollector: base class for Daily Bars HD, Trades/Quotes HT
//

class CHistoryCollector {
public:
  CHistoryCollector( IQFeedProvider *pProvider, const char *szSymbol, unsigned long nCount );
  virtual ~CHistoryCollector( void );

  virtual void Start( void );
  std::string &Symbol( void ) { return m_sSymbol; };

  typedef FastDelegate1<CHistoryCollector *> OnRetrievalCompleteHandler;
  void SetOnRetrievalComplete( OnRetrievalCompleteHandler function ) {
    OnRetrievalComplete = function;
  }
  virtual void WriteData( void ) = 0;
protected:
  std::string m_sSymbol;
  unsigned long m_nCount;  // number of items to retrieve
  IQFeedHistory *m_phistory;
  OnRetrievalCompleteHandler OnRetrievalComplete;
  void OnCompletion( IQFeedHistory *pHistory );
  void FinalizeCreation( void ) {
    m_phistory->SetOnRequestComplete( MakeDelegate( this, &CHistoryCollector::OnCompletion ) );
  }
  IQFeedProvider *m_pProvider;
private:
};

// 
// CHistoryCollectorDaily: 
//

class CHistoryCollectorDaily: public CHistoryCollector {
public:
  CHistoryCollectorDaily( IQFeedProvider *pProvider, const char *szSymbol, unsigned long nCount );
  virtual ~CHistoryCollectorDaily( void );
  virtual void Start( void );
  virtual void WriteData( void );
protected:
  CBars m_bars;
private:
};

// 
// CHistoryCollectorTicks:
//

class CHistoryCollectorTicks: public CHistoryCollector {
public:
  CHistoryCollectorTicks( IQFeedProvider *pProvider, const char *szSymbol, unsigned long nCount );
  virtual ~CHistoryCollectorTicks( void );
  virtual void Start( void );
  virtual void WriteData( void );
protected:
  CTrades m_trades;
  CQuotes m_quotes;
private:
};

