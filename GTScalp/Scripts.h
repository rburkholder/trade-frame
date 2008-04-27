#pragma once

#include "IQFeedHistoryCollector.h"

#include <queue>
#include <map>
#include <fstream>
using namespace std;

#include "boost/date_time/posix_time/posix_time.hpp"
//#include "boost/date_time/gregorian/gregorian_types.hpp" 
using namespace boost::posix_time;
using namespace boost::gregorian;

// 
// CScripts:
//

class CScripts {
public:
  CScripts(void);
  virtual ~CScripts(void);
  enum EHistoryType { Daily, Tick, Minute };
  void GetIQFeedHistoryForSymbol( char *szSymbol, EHistoryType typeHistory, unsigned long nDays );
  void GetIQFeedHistoryForSymbolRange( EHistoryType typeHistory, unsigned long nDays );
  void TestDataSet( void );
  void IterateGroups( void );
  enum enumDayStart { NoDayStart, DaySelect, BarCount, DayCount };
  enum enumScanType { NoScanType, Darvas, Bollinger, Breakout };
  void Scan( enumScanType scantype, enumDayStart dstype, int count, ptime dtStart, ptime dtEnd );
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
