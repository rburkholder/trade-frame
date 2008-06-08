#pragma once

#include "IQFeedRetrieveHistory.h"

#include "FastDelegate.h"
using namespace fastdelegate;

#include <string>
using namespace std;
// 
// CHistoryCollector: base class for Daily Bars HD, Trades/Quotes HT
//

class CHistoryCollector {
public:
  CHistoryCollector( CIQFeedProvider *pProvider, const char *szSymbol, unsigned long nCount );
  virtual ~CHistoryCollector( void );

  virtual void Start( void );
  string &Symbol( void ) { return m_sSymbol; };

  typedef FastDelegate1<CHistoryCollector *> OnRetrievalCompleteHandler;
  void SetOnRetrievalComplete( OnRetrievalCompleteHandler function ) {
    OnRetrievalComplete = function;
  }
  virtual void WriteData( void ) = 0;
protected:
  string m_sSymbol;
  unsigned long m_nCount;  // number of items to retrieve
  IQFeedHistory *m_phistory;
  OnRetrievalCompleteHandler OnRetrievalComplete;
  void OnCompletion( IQFeedHistory *pHistory );
  void FinalizeCreation( void ) {
    m_phistory->SetOnRequestComplete( MakeDelegate( this, &CHistoryCollector::OnCompletion ) );
  }
  CIQFeedProvider *m_pProvider;
private:
};

// 
// CHistoryCollectorDaily: 
//

class CHistoryCollectorDaily: public CHistoryCollector {
public:
  CHistoryCollectorDaily( CIQFeedProvider *pProvider, const char *szSymbol, unsigned long nCount );
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
  CHistoryCollectorTicks( CIQFeedProvider *pProvider, const char *szSymbol, unsigned long nCount );
  virtual ~CHistoryCollectorTicks( void );
  virtual void Start( void );
  virtual void WriteData( void );
protected:
  CTrades m_trades;
  CQuotes m_quotes;
private:
};

