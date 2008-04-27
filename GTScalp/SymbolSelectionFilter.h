#pragma once

#include "TimeSeries.h"

#include <string>
using namespace std;

class CSymbolSelectionFilter {
public:
  CSymbolSelectionFilter(void);
  virtual ~CSymbolSelectionFilter(void);
  CTimeSeries<CBar> *Bars( void ) { return &m_bars; };
  virtual void Process( const string &sSymbol ) = 0;
protected:
  //CBars m_bars;
  CTimeSeries<CBar> m_bars;
private:
};

// others:
//    volatile:  what I used to find ICE for trading:  volume and price range
//   find recently offered securities, which are trading flat, and run both sides : YHOO
//   use watch/unwatch to loop through securities looking for movers
//   compare with others in same SIC code to see if all are moving or just the one


class CSelectSymbolWithDarvas: public CSymbolSelectionFilter {
public:
  CSelectSymbolWithDarvas(void);
  virtual ~CSelectSymbolWithDarvas(void );
  void Process( const string &sSymbol );
protected:
private:
};

class CSelectSymbolWithBollinger: public CSymbolSelectionFilter {
public:
  CSelectSymbolWithBollinger(void);
  virtual ~CSelectSymbolWithBollinger(void );
  void Process( const string &sSymbol );
protected:
private:
};

class CSelectSymbolWithBreakout: public CSymbolSelectionFilter {
public:
  CSelectSymbolWithBreakout(void);
  virtual ~CSelectSymbolWithBreakout(void );
  void Process( const string &sSymbol );
protected:
private:
};
