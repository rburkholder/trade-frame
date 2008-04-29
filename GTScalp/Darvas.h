#pragma once

#include "DatedDatum.h"

// is a functor
class CDarvas: public std::unary_function<CBar &, void> {
public:
  CDarvas(void);
  ~CDarvas(void);
  void operator()( const CBar &bar ) { Calc( bar ); };
  void Calc( const CBar &bar );
  operator bool() { return bSignalBuy2; };
  bool SignalBuy( void ) { bool b = bSignalBuy; bSignalBuy = false; return b; };
  bool SignalSetStop( void ) { bool b = bSignalSetStop; bSignalSetStop = false; return b; };
  bool SignalExit( void ) { bool b = bSignalExit; bSignalExit = false; return b; };
  double StopLevel( void ) { return dblStop; };
  bool SignalDone( void ) { return bSignalDone; };
protected:
private:
 		bool bTop;
		bool bBottom;
		bool bSignalBuy; // reset when read
    bool bSignalBuy2;  //reset on each bar;
		bool bSignalSetStop;  //reset when read
		bool bSignalExit; // reset when read
		bool bSignalDone;

		int cntTop;
		int cntBottom;

		double dblTop;
		double dblBottom;
		double dblStop;
		double dblStopStep;
		double dblGhostTop;

		bool bDebug;
};
