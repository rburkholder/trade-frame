/************************************************************************
 * Copyright(c) 2010, One Unified. All rights reserved.                 *
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

#include <LibTimeSeries/DatedDatum.h>

class CDarvasResults {
public:
  CDarvasResults( bool bTrigger, double dblStopLevel ): m_bTrigger( bTrigger ), m_dblStopLevel( dblStopLevel ) {};
  ~CDarvasResults( void ) {};
  bool GetTrigger( void ) { return m_bTrigger; };
  double GetStopLevel( void ) { return m_dblStopLevel; };
protected:
  bool m_bTrigger;
  double m_dblStopLevel;
private:
};

// is a functor
class CDarvas: public std::unary_function<CBar &, void> {
public:
  CDarvas(void);
  ~CDarvas(void);
  void operator()( const CBar &bar ) { Calc( bar ); };
  void Calc( const CBar &bar );
  operator bool() { return bSignalBuy2; };
  operator CDarvasResults() { CDarvasResults results( bSignalBuy2, dblStop ); return results; };
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
