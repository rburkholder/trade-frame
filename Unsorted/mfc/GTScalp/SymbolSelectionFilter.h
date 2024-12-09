#pragma once

#include <string>
using namespace std;

#include <TFTimeSeries/TimeSeries.h>

#include <OUCommon/FastDelegate.h>

class SymbolSelectionFilter {
public:
  enum enumDayCalc { NoDayCalc, DaySelect, BarCount, DayCount };
  SymbolSelectionFilter( enumDayCalc dstype, int count, bool bUseStart, ptime dtStart, bool bUseEnd, ptime dtEnd );
  virtual ~SymbolSelectionFilter(void);
  const ou::tf::Bars& Bars( void ) { return m_bars; };
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
  ou::tf::Bars m_bars;
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


class SelectSymbolWithDarvas: public SymbolSelectionFilter {
public:
  SelectSymbolWithDarvas( enumDayCalc dstype, int count, bool bUseStart, ptime dtStart, bool bUseEnd, ptime dtEnd);
  virtual ~SelectSymbolWithDarvas(void );
  bool Validate( void );
  void Process( const string &sSymbol, const string &sPath );
protected:
private:
};

class SelectSymbolWithBollinger: public SymbolSelectionFilter {
public:
  SelectSymbolWithBollinger( enumDayCalc dstype, int count, bool bUseStart, ptime dtStart, bool bUseEnd, ptime dtEnd);
  virtual ~SelectSymbolWithBollinger(void );
  void Process( const string &sSymbol, const string &sPath );
protected:
private:
};

class SelectSymbolWithBreakout: public SymbolSelectionFilter {
public:
  SelectSymbolWithBreakout( enumDayCalc dstype, int count, bool bUseStart, ptime dtStart, bool bUseEnd, ptime dtEnd);
  virtual ~SelectSymbolWithBreakout(void );
  void Process( const string &sSymbol, const string &sPath );
protected:
private:
};

class CSelectSymbolWithXWeekHigh: public SymbolSelectionFilter {
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

class SelectSymbolWith10Percent: public SymbolSelectionFilter {
public:
  SelectSymbolWith10Percent( enumDayCalc dstype, int count, bool bUseStart, ptime dtStart, bool bUseEnd, ptime dtEnd);
  virtual ~SelectSymbolWith10Percent(void );
  bool Validate( void );
  void Process( const string &sSymbol, const string &sPath );
  void WrapUp( void );
protected:
  static const unsigned short nMaxInList = 10;  // maximum of 10 items in each list
  std::multimap<double, string> mapMaxPositives;
  std::multimap<double, string, MaxNegativesCompare> mapMaxNegatives;
private:
};

class SelectSymbolWithVolatility public SymbolSelectionFilter {
public:
  SelectSymbolWithVolatility( enumDayCalc dstype, int count, bool bUseStart, ptime dtStart, bool bUseEnd, ptime dtEnd);
  virtual ~SelectSymbolWithVolatility(void );
  bool Validate( void );
  void Process( const string &sSymbol, const string &sPath );
  void WrapUp( void );
protected:
  static const unsigned short nMaxInList = 10;  // maximum of 10 items in list
  std::multimap<double, string> mapMaxVolatility;
private:
};

class SelectSymbolWithStrikeCoverage: public SymbolSelectionFilter {
public:
  SelectSymbolWithStrikeCoverage( enumDayCalc dstype, int count, bool bUseStart, ptime dtStart, bool bUseEnd, ptime dtEnd);
  virtual ~SelectSymbolWithStrikeCoverage(void );
//  bool Validate( void );
  void Process( const string &sSymbol, const string &sPath );
//  void WrapUp( void );
protected:
  static const unsigned short nMaxInList = 10;  // maximum of 10 items in list
  std::multimap<double, string> mapMaxStrikeCoverage;
private:
};


