#pragma once

#include "IQFeedRetrieveHistory.h"

#include "FastDelegate.h"
using namespace fastdelegate;

#include <string>
#include <queue>
#include <map>
#include <fstream>
using namespace std;

// 
// CHistoryCollector: base class for Daily Bars HD, Trades/Quotes HT
//

class CHistoryCollector {
public:
  CHistoryCollector( const char *szSymbol, unsigned long nCount );
  virtual ~CHistoryCollector( void );

  virtual void Start( void );
  string &Symbol( void ) { return m_sSymbol; };

  typedef FastDelegate1<CHistoryCollector *> OnRetrievalCompleteHandler;
  void SetOnRetrievalComplete( OnRetrievalCompleteHandler function ) {
    OnRetrievalComplete = function;
  }
  virtual void Archive( ofstream &ofs ) = 0;
protected:
  string m_sSymbol;
  unsigned long m_nCount;  // number of items to retrieve
  IQFeedHistory *m_phistory;
  OnRetrievalCompleteHandler OnRetrievalComplete;
  void OnCompletion( IQFeedHistory *pHistory );
  void FinalizeCreation( void ) {
    m_phistory->SetOnRequestComplete( MakeDelegate( this, &CHistoryCollector::OnCompletion ) );
  }
private:
};

// 
// CHistoryCollectorDaily: 
//

class CHistoryCollectorDaily: public CHistoryCollector {
public:
  CHistoryCollectorDaily( const char *szSymbol, unsigned long nCount );
  virtual ~CHistoryCollectorDaily( void );
  virtual void Start( void );
  virtual void Archive( ofstream &ofs );
protected:
  CBars m_bars;
  //IQFeedHistoryHD *m_pBarHistory;
private:
};

// 
// CHistoryCollectorTicks:
//

class CHistoryCollectorTicks: public CHistoryCollector {
public:
  CHistoryCollectorTicks( const char *szSymbol, unsigned long nCount );
  virtual ~CHistoryCollectorTicks( void );
  virtual void Start( void );
  virtual void Archive( ofstream &ofs );
protected:
  CTrades m_trades;
  CQuotes m_quotes;
  //IQFeedHistoryHT *m_pTickHistory;
private:
};

// 
// CScripts:
//

class CScripts {
public:
  CScripts(void);
  virtual ~CScripts(void);
  enum EHistoryType { Daily, Tick, Minute };
  void GetIQFeedHistoryForSymbolRange( EHistoryType typeHistory, unsigned long nDays );
protected:
  static const unsigned short nMaxActiveCollectors = 4;
  queue<CHistoryCollector *> m_qHistoryCollectors; // collectors waiting activiation
  map<string, CHistoryCollector *> m_mapActiveHistoryCollectors; // collectors which are active
  typedef pair<string, CHistoryCollector *> m_pair_mapActiveHistoryCollectors;

  void StartHistoryCollection( void );
  void HistoryCollectorIsComplete( CHistoryCollector *phc );

  std::ofstream ofs;

private:
  //_CrtMemState memstate1, memstate2, memstate3;
};
