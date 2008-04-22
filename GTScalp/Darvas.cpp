#include "StdAfx.h"
#include "Darvas.h"

CDarvas::CDarvas(void):
  bTop( false ), bBottom( false ), bSignalBuy( false ), bSignalSetStop( false ),
    bSignalExit( false ), bSignalDone( false ), cntTop( 0 ), cntBottom( 0 ),
    dblTop( 0 ), dblBottom( 0 ), dblStop( 0 ), dblStopStep( 0 ), dblGhostTop( 0 ),
    bDebug( false )  {
}

CDarvas::~CDarvas(void) {
}

void CDarvas::Calc(const CBar &bar) {
  // Calculate Darvis Box
  if ( bar.m_dblHigh <= dblTop ) {
    ++cntTop;
    if ( 4 == cntTop ) { // we've reached the four day price pattern
      bTop = true;
    }
  }
  else { // top of box has been exceeded
    // perform trade if box completed
    if ( bTop && bBottom ) {  // we have a completed box
      if ( ( bar.m_dblClose > dblTop ) && ( bar.m_dblClose > dblStop ) ) {
        bSignalBuy = true;
        // calculate a new ghost box
        dblStop = dblTop;
        dblGhostTop = dblStop + dblStopStep;
        bSignalSetStop = true;
      }
    }
    // restart box calculation on new high
    dblTop = bar.m_dblHigh;
    dblBottom = bar.m_dblLow;
    cntTop = 1;
    cntBottom = 0;
    bTop = false;
    bBottom = false;
  }

  // calculate bottom of box
  if ( bBottom ) {
    if ( bar.m_dblClose < dblBottom ) {
      bSignalExit = true;
      bSignalDone = true;
    }
    else {
      ++cntBottom;
      if ( ( 4 < cntBottom ) && ( bar.m_dblClose > dblStop ) && ( bar.m_dblOpen < bar.m_dblClose ) ) {
        bSignalBuy = true;  // **
      }
    }

  }
  else { // see if can set the bottom yet
    if ( bar.m_dblLow >= dblBottom ) { // higher low
      cntBottom++;
      if ( 4 == cntBottom ) {
        bBottom = true;
        dblStop = max( dblBottom, dblStop );  // **
        //dblStop = dblBottom;
        dblStopStep = dblTop - dblBottom;
        dblGhostTop = dblTop;
        bSignalSetStop = true;
      }
    }
    else { // lower low so reset
      dblBottom = bar.m_dblLow;
      cntBottom = 1;
    }
  }

  // calculate Ghost box if no active box
  if ( !bTop && !bBottom && ( 0.0 < dblStop ) ) {
    if ( bar.m_dblClose > dblGhostTop ) {
      dblStop = dblGhostTop;
      dblGhostTop += dblStopStep;
      bSignalSetStop = true;
    }
  }

  //if ( null != dsTop) dsTop.Add( bar.DateTime, dblTop );
  //if ( null != dsBottom ) dsBottom.Add( bar.DateTime, dblBottom );
  //if ( 0.0 != dblStop ) if ( null != dsStop ) dsStop.Add( bar.DateTime, dblStop );
  //  if ( bDebug ) Console.WriteLine( "{0} cT {1} cB {2} stop {3} Top {4} Bottom {5} GhostTop {6} StopStep {7}", 
  //    bar.DateTime, cntTop, cntBottom, dblStop, dblTop, dblBottom, dblGhostTop, dblStopStep );
}

