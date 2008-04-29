#pragma once

#include "TimeSeries.h"

#include <string>
using namespace std;

class CSymbolSelectionFilter {
public:
  enum enumDayCalc { NoDayCalc, DaySelect, BarCount, DayCount };
  CSymbolSelectionFilter( enumDayCalc dstype, int count, bool bUseStart, ptime dtStart, bool bUseEnd, ptime dtEnd );
  virtual ~CSymbolSelectionFilter(void);
  CTimeSeries<CBar> *Bars( void ) { return &m_bars; };
  virtual bool Validate( void ) { return true; };
  void Start( void );
  virtual void Process( const string &sSymbol, const string &sPath ) = 0;
protected:
  CTimeSeries<CBar> m_bars;
  enumDayCalc m_DayStartType;
  int m_nCount;
  bool m_bUseStart;
  ptime m_dtStart;
  bool m_bUseLast;
  ptime m_dtLast;
private:
};

// others:
//    volatile:  what I used to find ICE for trading:  volume and price range
//   find recently offered securities, which are trading flat, and run both sides : YHOO
//   use watch/unwatch to loop through securities looking for movers
//   compare with others in same SIC code to see if all are moving or just the one


class CSelectSymbolWithDarvas: public CSymbolSelectionFilter {
public:
  CSelectSymbolWithDarvas( enumDayCalc dstype, int count, bool bUseStart, ptime dtStart, bool bUseEnd, ptime dtEnd);
  virtual ~CSelectSymbolWithDarvas(void );
  bool Validate( void );
  void Process( const string &sSymbol, const string &sPath );
protected:
private:
};

class CSelectSymbolWithBollinger: public CSymbolSelectionFilter {
public:
  CSelectSymbolWithBollinger( enumDayCalc dstype, int count, bool bUseStart, ptime dtStart, bool bUseEnd, ptime dtEnd);
  virtual ~CSelectSymbolWithBollinger(void );
  void Process( const string &sSymbol, const string &sPath );
protected:
private:
};

class CSelectSymbolWithBreakout: public CSymbolSelectionFilter {
public:
  CSelectSymbolWithBreakout( enumDayCalc dstype, int count, bool bUseStart, ptime dtStart, bool bUseEnd, ptime dtEnd);
  virtual ~CSelectSymbolWithBreakout(void );
  void Process( const string &sSymbol, const string &sPath );
protected:
private:
};
