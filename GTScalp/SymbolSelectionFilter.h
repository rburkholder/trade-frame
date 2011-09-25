#pragma once

#include "TimeSeries.h"

#include "FastDelegate.h"

#include <string>
using namespace std;

class CSymbolSelectionFilter {
public:
  enum enumDayCalc { NoDayCalc, DaySelect, BarCount, DayCount };
  CSymbolSelectionFilter( enumDayCalc dstype, int count, bool bUseStart, ptime dtStart, bool bUseEnd, ptime dtEnd );
  virtual ~CSymbolSelectionFilter(void);
  CTimeSeries<CBar> *Bars( void ) { return &m_bars; };
  virtual bool Validate( void ) { return true; };
  typedef fastdelegate::FastDelegate3<const string &, const string &,const string &> OnAddSymbolHandler;
  void SetOnAddSymbolHandler( OnAddSymbolHandler function ) {
    OnAddSymbol = function;
  }
  void Start( void );
  virtual void Process( const string &sSymbol, const string &sPath ) = 0;
  virtual void WrapUp( void ) {};
protected:
  OnAddSymbolHandler OnAddSymbol;
  CTimeSeries<CBar> m_bars;
  enumDayCalc m_DayStartType;
  int m_nCount;
  bool m_bUseStart;
  ptime m_dtStart;
  bool m_bUseLast;
  ptime m_dtLast;
  std::string m_sPath;
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

class CSelectSymbolWithXWeekHigh: public CSymbolSelectionFilter {
public:
  CSelectSymbolWithXWeekHigh( enumDayCalc dstype, int count, bool bUseStart, ptime dtStart, bool bUseEnd, ptime dtEnd);
  virtual ~CSelectSymbolWithXWeekHigh(void );
  void Process( const string &sSymbol, const string &sPath );
protected:
private:
};

struct MaxNegativesCompare {
  bool operator() ( double dbl1, double dbl2 ) {
    return dbl2 < dbl1; // reverse form of operator so most negative at end of list
  }
};

class CSelectSymbolWith10Percent: public CSymbolSelectionFilter {
public:
  CSelectSymbolWith10Percent( enumDayCalc dstype, int count, bool bUseStart, ptime dtStart, bool bUseEnd, ptime dtEnd);
  virtual ~CSelectSymbolWith10Percent(void );
  bool Validate( void );
  void Process( const string &sSymbol, const string &sPath );
  void WrapUp( void );
protected:
  static const unsigned short nMaxInList = 10;  // maximum of 10 items in each list
  std::multimap<double, string> mapMaxPositives;
  std::multimap<double, string, MaxNegativesCompare> mapMaxNegatives;
private:
};

class CSelectSymbolWithVolatility: public CSymbolSelectionFilter {
public:
  CSelectSymbolWithVolatility( enumDayCalc dstype, int count, bool bUseStart, ptime dtStart, bool bUseEnd, ptime dtEnd);
  virtual ~CSelectSymbolWithVolatility(void );
  bool Validate( void );
  void Process( const string &sSymbol, const string &sPath );
  void WrapUp( void );
protected:
  static const unsigned short nMaxInList = 10;  // maximum of 10 items in list
  std::multimap<double, string> mapMaxVolatility;
private:
};

class CSelectSymbolWithStrikeCoverage: public CSymbolSelectionFilter {
public:
  CSelectSymbolWithStrikeCoverage( enumDayCalc dstype, int count, bool bUseStart, ptime dtStart, bool bUseEnd, ptime dtEnd);
  virtual ~CSelectSymbolWithStrikeCoverage(void );
//  bool Validate( void );
  void Process( const string &sSymbol, const string &sPath );
//  void WrapUp( void );
protected:
  static const unsigned short nMaxInList = 10;  // maximum of 10 items in list
  std::multimap<double, string> mapMaxStrikeCoverage;
private:
};


