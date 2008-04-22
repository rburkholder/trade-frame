#pragma once

#include "DatedDatum.h"

class CDarvas {
public:
  CDarvas(void);
  ~CDarvas(void);
  void Calc( const CBar &bar );
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
